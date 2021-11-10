#include "config.hpp"
#include "Bus.h"
#include "Cache.h"



void Bus::init_bus(int cache_size, int associativity, int block_size, GlobalLock *gl){
    // Step 1: initialize variables
    this->num_blocks = (cache_size / block_size) / associativity;
    this->associativity = associativity;
    this->offset = block_size;
    this->gl = gl;
    std::cout << "Bus initialized" << std::endl;

}

void Bus::init_cache(Cache *c0, Cache *c1, Cache *c2, Cache *c3) {
    cache_list.push_back(c0);
    cache_list.push_back(c1);
    cache_list.push_back(c2);
    cache_list.push_back(c3);
}

int Bus_MESI::BusRd(int PID, int i_set, int tag, Cache *cache) {
    int status = status_MESI::I;
    gl->gl_lock(i_set);
    for (int i = 0; i < num_cores; i++) {
        if (i == PID) continue;

        int curr_status = cache_list[i]->get_status_cacheline(i_set, tag);
        if (curr_status != status_MESI::I) {
            // Change M/E to S (Flush)
            if (curr_status == status_MESI::M || status_MESI::E_MESI)
                cache_list[i]->set_status_cacheline(i_set, tag, status_MESI::S);
            status = curr_status;
            break;
        }
        if (status != status_MESI::I) break;
    }
    gl->gl_unlock(i_set);
    return status;
}

void Bus_MESI::BusUpd(int PID, int i_set, int tag, Cache *cache) { 
    /*
    * Step 1: Acquire lock to set index
    * Step 2: Invalidate all cache lines  
    * Step 3: Release lock
    */
    gl->gl_lock(i_set);
    for(int i = 0; i < num_cores; i++) {
        if (i == PID) continue;

        int curr_status = cache_list[i]->get_status_cacheline(i_set, tag);
        if (curr_status != status_MESI::I) {
            // Invalidate all cache Lines
            cache_list[i]->set_status_cacheline(i_set, tag, status_MESI::I);
        }
    }
    gl->gl_unlock(i_set);
}


int Bus_Dragon::BusRd(int PID, int i_set, int tag, Cache *cache) {
    gl->gl_lock(i_set);

    gl->gl_unlock(i_set);
    return 1; // placeholder
}
void Bus_Dragon::BusUpd(int PID, int i_set, int tag, Cache *cache) { 
    gl->gl_lock(i_set);
    /*
    * Step 1: Acquire lock to set index
    * Step 2: Update all caches with existing tag, change states if necessary
    * Step 3: Release lock
    */
    for(int i = 0; i < num_cores; i++) {
        cache->set_status_cacheline(i_set, tag, 0);
    }
    gl->gl_unlock(i_set);
}