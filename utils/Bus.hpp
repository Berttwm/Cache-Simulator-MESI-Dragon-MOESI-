#ifndef _BUS_HPP
#define _BUS_HPP

#include <iostream>
#include <vector>
#include <unordered_set>

#include "GlobalLock.hpp"


class Bus {
   private:
   public:
    typedef int pid;
    const int num_cores = 4;

    // struct Bus_State {
    //     std::unordered_set<pid> PID_list; // if only 1 element in the list, implicitly means owner
    // };
    int num_blocks; 
    int associativity;
    int offset;
    GlobalLock *gl;
    // // latest_memory has the view of the latest memory written
    // std::vector<std::vector<int>> latest_memory;
    void init_bus(int cache_size, int associativity, int block_size, GlobalLock *gl){
        // Step 1: initialize variables
        this->num_blocks = (cache_size / block_size) / associativity;
        this->associativity = associativity;
        this->offset = block_size;
        this->gl = gl;
        std::cout << "Bus initialized" << std::endl;

    }

    // Cache to Bus transactions
    void BusRd(int PID, int i_set, int tag) {
        gl->gl_lock(i_set);

        gl->gl_unlock(i_set);
    }
    void BusRdX(int PID, int i_set, int tag) {
        /*
        * Step 1: Acquire lock to set index
        * Step 2: Invalidate all cache lines  
        * Step 3: Release lock
        */
        gl->gl_lock(i_set);
        for(int i = 0; i < num_cores; i++) {
            // flush_cacheline(i_set, tag);
        }
        gl->gl_unlock(i_set);
    }
    void BusUpd(int PID, int i_set, int tag) { 
        gl->gl_lock(i_set);

        gl->gl_unlock(i_set);
    }
};

#endif // BUS_HPP