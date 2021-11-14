#include "Cache.h"
#include "Bus.h"
#include "config.hpp"


void Cache::set_params (int cache_size, int associativity, int blk_size, int PID, Bus *bus, GlobalLock *gl) {
    this->PID = PID;
    num_ways = associativity;
    num_sets = (cache_size / blk_size) / associativity;
    block_size = blk_size;
    std::vector<int> temp(2, 0);
    std::vector<std::vector<int>> temp2;
    for (int i = 0; i < num_sets; i++) {
        temp2.push_back(temp);
    }
    for (int i = 0; i < associativity; i++) {
        dummy_cache.push_back(temp2);
    }
    
    this->bus = bus;
    this->gl = gl;
}

/*
** To maintain LRU replacement policy, old data in the given cache set are shifted to left till pos
*  pos = 0 for read-miss
*  Output: return the cycle taken, once eviction occurs, the shifting takes 100 cycles
*/
int Cache::shift_cacheline_left_until(int i_set, int pos) {
    bool flush = false;
    // If flush back occurs
    if (pos == 0 && (dummy_cache[0][i_set][cache_line::status] == status_MESI::M || 
                    dummy_cache[0][i_set][cache_line::status] == status_Dragon::D ||
                    dummy_cache[0][i_set][cache_line::status] == status_Dragon::Sm)) {
        int test_status = dummy_cache[0][i_set][cache_line::status];
        std::cout << "[" << PID << "] " << "Eviction occurs with status: " << test_status << std::endl;
        num_data_traffic += 1;
        flush = true;
    }
    for (int i = pos; i < num_ways-1; i++) { 
        dummy_cache[i][i_set] = dummy_cache[i+1][i_set];
    }

    return flush? 100 : 0;
}
/* 
***************************************************************
MESI Cache Protocol APIs
***************************************************************
*/
int Cache_MESI::pr_read(int i_set, int tag) {
    int curr_op_cycle = 1;
    gl->gl_lock(i_set);
    for (int i = 0; i < num_ways; i++) {
        // Read hit
        if ((dummy_cache[i][i_set][cache_line::status] != status_MESI::I) && (dummy_cache[i][i_set][cache_line::tag] == tag)) {
            // Update LRU Policy - Read Hit
            std::cout << "[" << PID << "] " << "Read Hit" << std::endl;
            std::vector<int> temp = dummy_cache[i][i_set];
            shift_cacheline_left_until(i_set,i);
            dummy_cache[num_ways-1][i_set] = temp; // set last line to temp 
            // check the type of access
            switch (dummy_cache[i][i_set][cache_line::status]) {
            case status_MESI::M:
            case status_MESI::E_MESI:
                num_access_private += 1;
                break;           
            case status_MESI::S:
                num_access_shared += 1;
                break;
            }

            gl->gl_unlock(i_set);
            return curr_op_cycle;
        }
    }
    // Read miss
    // Update LRU Policy - Read miss
    std::cout << "[" << PID << "] " << "Read Miss" << std::endl;
    curr_op_cycle += shift_cacheline_left_until(i_set, 0); 
    dummy_cache[num_ways-1][i_set][cache_line::tag] = tag; // set last line to new 
    num_cache_miss += 1;
    num_data_traffic += 1;
    Cache *placeholder;
    if (bus->BusRd(PID, i_set, tag, placeholder) == status_MESI::I) {
        // I -> E
        // Fetching a block from memory to cache takes additional 100 cycles
        num_access_private += 1;
        curr_op_cycle += 100;
        dummy_cache[num_ways-1][i_set][cache_line::status] = status_MESI::E_MESI;
        std::cout << "[" << PID << "] " << "Read from main memory" << std::endl;
        
    } else {
        // I -> S
        // Fetching a block from other cache to my cache takes additional 2N cycles
        num_access_shared += 1;
        curr_op_cycle += 2 * (block_size/4);
        dummy_cache[num_ways-1][i_set][cache_line::status] = status_MESI::S;
        std::cout << "[" << PID << "] " << "Read from other caches" << std::endl;
    }
    gl->gl_unlock(i_set);
    return curr_op_cycle; // placeholder
}

int Cache_MESI::pr_write(int i_set, int tag) {
    int curr_op_cycle = 1;
    gl->gl_lock(i_set);
    for (int i = 0; i < num_ways; i++) {
        // Write hit
        if ((dummy_cache[i][i_set][cache_line::status] != status_MESI::I) && (dummy_cache[i][i_set][cache_line::tag] == tag)) {
            std::cout << "[" << PID << "] " << "Write Hit" << std::endl;            
            // 1. Update LRU Policy First - Write Hit
            std::vector<int> temp = dummy_cache[i][i_set];
            shift_cacheline_left_until(i_set,i);
            dummy_cache[num_ways-1][i_set] = temp; // set last line to temp 

            // 2. Set status
            switch (dummy_cache[num_ways-1][i_set][cache_line::status]) {
            case status_MESI::M:
                num_access_private += 1;
                break;
            case status_MESI::E_MESI:
                num_access_private += 1;
                dummy_cache[i][i_set][cache_line::status] = status_MESI::M;
                break;
            case status_MESI::S:
                num_access_shared += 1;
                dummy_cache[i][i_set][cache_line::status] = status_MESI::M;
                Cache *placeholder;
                num_update += bus->BusUpd(PID, i_set, tag, placeholder);
                break;
            }
            gl->gl_unlock(i_set);
            return curr_op_cycle;
        }
            
    }
    // Write miss policy: Write-back, write-allocate
    // Step 1: read line into cache block
    std::cout << "[" << PID << "] " << "Write Miss" << std::endl;
    num_cache_miss += 1;
    num_data_traffic += 1;
    Cache *placeholder;
    if (bus->BusRd(PID, i_set, tag, placeholder) == status_MESI::I) {
        // Fetching a block from memory to cache takes additional 100 cycles
        num_access_private += 1;
        curr_op_cycle += 100;
    } else {
        // Fetching a block from another cache to mine takes 2N cycles
        curr_op_cycle += 2 * (block_size/4);

        int curr_update = bus->BusUpd(PID, i_set, tag, placeholder);
        num_update += curr_update;
        num_access_shared += 1;
        // Invalidate the block in each other caches takes 2 
        curr_op_cycle += 2*curr_update;
    }
    // Step 2: Update LRU Policy - Write Miss
    curr_op_cycle += shift_cacheline_left_until(i_set, 0); 
    // Step 3: Set last line to new and modified
    dummy_cache[num_ways-1][i_set][cache_line::tag] = tag; 
    dummy_cache[num_ways-1][i_set][cache_line::status] = status_MESI::M; 
    
    // Step 3: Invalidate all other cachelines
    bus->BusUpd(PID, i_set, tag, placeholder);
    gl->gl_unlock(i_set);
    return curr_op_cycle; // placeholder
}

int Cache_MESI::get_status_cacheline(int i_set, int tag) {
    std::cout << "[" << PID << "] " << "Looking for address" << std::endl;
    int status = status_MESI::I;
    for (int i = 0; i < num_ways; i++) {
        if (dummy_cache[i][i_set][cache_line::tag] == tag) {
            status = dummy_cache[i][i_set][cache_line::status];
            break;
        }
    }
    // gl->gl_unlock(i_set);
    return status;
}

int Cache_MESI::set_status_cacheline(int i_set, int tag, int status, int op) {
    for (int i = 0; i < num_ways; i++) {
        if (dummy_cache[i][i_set][cache_line::tag] == tag) { // if tag found
            dummy_cache[i][i_set][cache_line::status] = status;
            break;
        }
    }
    return 1; // placeholder
}

/* 
***************************************************************
Dragon Cache Protocol APIs
***************************************************************
*/
int Cache_Dragon::pr_read(int i_set, int tag) {
    int curr_op_cycle = 1;
    gl->gl_lock(i_set);

    for (int i = 0; i < num_ways; i++) {
        // Read hit
        if ((dummy_cache[i][i_set][cache_line::status] != status_Dragon::not_found) && (dummy_cache[i][i_set][cache_line::tag] == tag)) {
            // Update LRU Policy - Read hit
            std::vector<int> temp = dummy_cache[i][i_set];
            shift_cacheline_left_until(i_set,i);
            dummy_cache[num_ways-1][i_set] = temp; // set last line to temp

            // update the type of access based on the block status
            switch (dummy_cache[i][i_set][cache_line::status]) {
            case status_Dragon::D:
            case status_Dragon::E_DRAGON:
                num_access_private += 1;
                break;
            case status_Dragon::Sm:
            case status_Dragon::Sc:
                num_access_shared += 1;
                break;
            }

            gl->gl_unlock(i_set);
            return curr_op_cycle;
        }
    }
    // Read miss
    num_data_traffic += 1;
    num_cache_miss += 1;
    // Update LRU Policy - Read miss
    curr_op_cycle += shift_cacheline_left_until(i_set, 0); 
    dummy_cache[num_ways-1][i_set][cache_line::tag] = tag; // set last line to new 

    Cache *placeholder;
    if (bus->BusRd(PID, i_set, tag, placeholder) == status_Dragon::not_found) {
        // not_found -> E
        // Fetching a block from memory to cache takes additional 100 cycles
        num_access_private += 1;
        curr_op_cycle += 100;
        dummy_cache[num_ways-1][i_set][cache_line::status] = status_Dragon::E_DRAGON;
    } else {
        // not_found -> Sc
        num_access_shared += 1;
        // Fetching a block from other cache to my cache takes additional 2N cycles
        curr_op_cycle += 2 * (block_size/4);
        dummy_cache[num_ways-1][i_set][cache_line::status] = status_Dragon::Sc;
    }
    gl->gl_unlock(i_set);
    return curr_op_cycle; // placeholder
}

int Cache_Dragon::pr_write(int i_set, int tag) {
    int curr_op_cycle = 1;
    Cache *placeholder;
    gl->gl_lock(i_set);

    for (int i = 0; i < num_ways; i++) {
        // Write hit
        if ((dummy_cache[i][i_set][cache_line::status] != status_Dragon::not_found) && (dummy_cache[i][i_set][cache_line::tag] == tag)) {
            switch (dummy_cache[i][i_set][cache_line::status]) {
            case status_Dragon::E_DRAGON:
                // TODO
                dummy_cache[i][i_set][cache_line::status] = status_Dragon::D;
            case status_Dragon::D:
                // accessing E or D are both private data access
                num_access_private += 1;
                break;
            case status_Dragon::Sc:
                // Same as Sm
            case status_Dragon::Sm:
                num_access_shared += 1;
                if (bus->BusRd(PID, i_set, tag, placeholder) == status_Dragon::not_found) {
                    // not found in other cache
                    dummy_cache[num_ways-1][i_set][cache_line::status] = status_Dragon::D;
                    curr_op_cycle += 100;
                } else {
                    // found in other cache
                    // Each write to another cache block incurs 2N cycles
                    num_update += bus->BusUpd(PID, i_set, tag, placeholder);
                    num_data_traffic += num_update;
                    curr_op_cycle += num_update * 2 * (block_size/4);
                    dummy_cache[num_ways-1][i_set][cache_line::status] = status_Dragon::Sm;
                }
                break;

            }
            std::vector<int> temp = dummy_cache[i][i_set];
            shift_cacheline_left_until(i_set,i);
            dummy_cache[num_ways-1][i_set] = temp; // set last line to temp
            gl->gl_unlock(i_set);
            return curr_op_cycle;
        }
        
    }
    // Write-Miss 
    num_cache_miss += 1;
    // It takes 1 data traffic to fetch from either memory or other caches
    num_data_traffic += 1;
    // Step 1: Update LRU Policy - Write Hit
    curr_op_cycle += shift_cacheline_left_until(i_set, 0); 
    if (bus->BusRd(PID, i_set, tag, placeholder) == status_Dragon::not_found) {
        // Fetching a block from memory to cache 
        num_access_private += 1;
        // Step 2: Set last line to new and modified
        dummy_cache[num_ways-1][i_set][cache_line::tag] = tag; 
        dummy_cache[num_ways-1][i_set][cache_line::status] = status_Dragon::D; 

        curr_op_cycle += 100; // read from memory operation
    } else {
        // Fetching a block from another cache to mine takes 2N cycles
        num_access_private += 1;
        // Step 2: Set last line to new and modified
        dummy_cache[num_ways-1][i_set][cache_line::tag] = tag; 
        dummy_cache[num_ways-1][i_set][cache_line::status] = status_Dragon::Sm; 

        curr_op_cycle += 2*(block_size/4); // read from other cache lines first

        // Step 3: then update all included cache lines
        // For each BusUpd to other caches, it takes 2N cycles 
        // where N = num of word and 1 word == 4 bytes
        num_update += bus->BusUpd(PID, i_set, tag, placeholder);
        num_data_traffic += num_update;
        curr_op_cycle += num_update * 2 * (block_size/4);
    }

    gl->gl_unlock(i_set);
    return curr_op_cycle; // placeholder    // TODO: handle the case when the block not in cache
}

int Cache_Dragon::get_status_cacheline(int i_set, int tag) {
    int status = status_Dragon::not_found;
    for (int i = 0; i < num_ways; i++) {
        if (dummy_cache[i][i_set][cache_line::tag] == tag) {
            status = dummy_cache[i][i_set][cache_line::status];
            break;
        }
    }
    return status;
}
    
int Cache_Dragon::set_status_cacheline(int i_set, int tag, int status, int op) {

    for (int i = 0; i < num_ways; i++) {
        if (dummy_cache[i][i_set][cache_line::tag] == tag) {
            dummy_cache[i][i_set][cache_line::status] = status;
            if(op == op_type::write_op) {
                // Update LRU policy of other cache if set_status is a write for Dragon protocol
                std::vector<int> temp = dummy_cache[i][i_set];
                shift_cacheline_left_until(i_set,i);
                dummy_cache[num_ways-1][i_set] = temp; // set last line to temp
            }
            break;
        }
    }
    return 1; // placeholder
}