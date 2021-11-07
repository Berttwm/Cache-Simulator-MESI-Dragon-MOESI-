#ifndef _CACHE_HPP
#define _CACHE_HPP

#include <iostream>
#include <vector>

#include "config.hpp"
#include "GlobalLock.hpp"

class Bus;


// Abstract class Cache
class Cache {
public:
    enum cache_line {status, tag};
    int PID;
    int num_sets;
    int num_ways;
    Bus *bus;
    GlobalLock *gl;
    std::vector<std::vector<std::vector<int>>> dummy_cache;
    // // Construct a dummy cache with shape: associativity(num_ways), num of cache set, 2
    // 2: index 0 for states, index 1 for tag
    void set_params(int cache_size, int associativity, int blk_size, int PID, Bus *bus, GlobalLock *gl) {
        this->PID = PID;
        num_ways = associativity;
        num_sets = (cache_size / blk_size) / associativity;
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

    virtual int pr_read(int i_set, int tag) = 0;
    virtual int pr_write(int i_set, int tag) = 0;
    /* Cache to bus transactions */
    virtual void update_cacheline(int i_set, int tag) = 0;
        
};

class Cache_MESI : public Cache {
private:
     
public:
    int pr_read(int i_set, int tag) {
        for (int i = 0; i < num_ways; i++) {
            // Read hit
            if ((dummy_cache[i][i_set][cache_line::status] != status_MESI::I) && (dummy_cache[i][i_set][cache_line::tag] == tag))
                return 1;
        }
        // Read miss
        // TODO: check bus then update status
        return 1; // placeholder
    }

    int pr_write(int i_set, int tag) {
        for (int i = 0; i < num_ways; i++) {
            // Write hit
            if ((dummy_cache[i][i_set][cache_line::status] != status_MESI::I) && (dummy_cache[i][i_set][cache_line::tag] == tag)) {
                switch (dummy_cache[i][i_set][cache_line::status]) {
                case status_MESI::M:
                    return 1;                
                case status_MESI::E_MESI:
                    dummy_cache[i][i_set][cache_line::status] = status_MESI::M;
                    // TODO: update bus (no need to update bus on write)
                    return 1;
                case status_MESI::S:
                    // TODO: update bus on write
                    // bus->BusRdX();
                    break;

                }
            }
                
        }
        // Write miss
        return 1; // placeholder
    } 
    /* Cache to Bus transaction API 
    * 1) update_cacheline: invalidate all cacheline entries 
    */
    void update_cacheline(int i_set, int tag) {
        // access to this method means cache is already locked
        for(int i = 0; i < num_ways; i++) {
            if (dummy_cache[i][i_set][cache_line::tag] == tag) { // if tag found
                dummy_cache[i][i_set][cache_line::status] == status_MESI::I;
            }
        }
    }
    
};

class Cache_Dragon : public Cache {
public:
    int pr_read(int i_set, int tag) {
        for (int i = 0; i < num_ways; i++) {
            // Read hit
            if ((dummy_cache[i][i_set][cache_line::status] != 0) && (dummy_cache[i][i_set][cache_line::tag] == tag))
                return 1;
        }
        // Read miss
        return 1; // placeholder
    }

    int pr_write(int i_set, int tag) {
        for (int i = 0; i < num_ways; i++) {
            // Write hit
            if ((dummy_cache[i][i_set][cache_line::status] != 0) && (dummy_cache[i][i_set][cache_line::tag] == tag)) {
                switch (dummy_cache[i][i_set][cache_line::status]) {
                case status_Dragon::E_DRAGON:
                    // TODO
                    break;
                case status_Dragon::Sc:
                    // TODO update other caches thru bus
                    break;
                case status_Dragon::Sm:
                    // TODO: update other caches thru bus
                case status_Dragon::D:
                    return 1;
                }
            }
            
        }
        // TODO: handle the case when the block not in cache
        return 1;
    }
    /* Cache to Bus transaction API 
    * 1) update_cacheline: invalidate all cacheline entries 
    */
    void update_cacheline(int i_set, int tag) {
        // access to this method means cache is already locked
        for(int i = 0; i < num_ways; i++) {
            if (dummy_cache[i][i_set][cache_line::tag] == tag) { // if tag found
                switch (dummy_cache[i][i_set][cache_line::status]) {
                case status_Dragon::E_DRAGON:
                    // This case should not be possible 
                    break;
                case status_Dragon::Sc:
                    break;
                case status_Dragon::Sm:
                    // This case should not be possible 
                    break;
                case status_Dragon::D:
                    break;
                }
            }
        }
    }
    
};

#endif // _CACHE_HPP
