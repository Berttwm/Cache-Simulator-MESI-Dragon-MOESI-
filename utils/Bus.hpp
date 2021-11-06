#ifndef _BUS_HPP
#define _BUS_HPP

#include <iostream>
#include <cmath>
#include <vector>
#include <unordered_set>
#include <mutex>

class Bus {
   private:
   public:
    typedef int pid;
    struct Bus_State {
        std::unordered_set<pid> PID_list; // if only 1 element in the list, implicitly means owner
    };
    int virtual_mem_bits = 32;
    int num_blocks; // total number of unique blocks held in bus = number of unique entries
    int associativity;
    int offset;
    // bus_states to keep track of the processes within a unique block
    std::vector<Bus_State> bus_states;
    std::vector<std::mutex> mutexes;
    void init_bus(int cache_size, int associativity, int block_size){
        // Step 1: find number of unique addresses in memory (simple remove offset bits)
        double offset_bits = sqrt(1.0*block_size);
        this->num_blocks = 1 << (virtual_mem_bits-(int) offset_bits); // 32 = bits in 32-bit virtual mem system
        this->associativity = associativity;
        this->offset = block_size;
        std::cout << "bus num_blocks = " << num_blocks << std::endl;
        // Step 2: Initialize size of bus_states to number of unique blocks
        // bus_states.resize(num_blocks);
        // for(int i = 0; i < num_blocks; i++) {
        //     Bus_State bs;
        //     bus_states.push_back(bs);
        // }
        // std::cout << "bus" 
        // Step 3: Initialize list of mutexes to number of unique blocks
        // std::vector<std::mutex> temp_locks_list(num_blocks);
        // mutexes.swap(temp_locks_list);
    }
};

class Bus_MESI : public Bus {
   public:
    void BusRd() {

    }
    void BusRdX(int PID, int value) {
        int curr_idx = value / this->offset;
        /*
        * Step 1: Acquire lock to bus memory region
        * Step 2: Invalidate all cache lines: Clear PID_list  
        * Step 3: Add PID into PID_list
        * Step 4: Release lock
        */
        // mutexes[curr_idx].lock();
        bus_states[curr_idx].PID_list.clear();
        
        // mutexes[curr_idx].unlock();
    }

};

class Bus_Dragon : public Bus {
   public:
    void BusRd() {

    }
    // void BusRdX() {
        // No more BusRdX in Dragon
    // }
    void BusUpd() { 
        /* TODO:: Verify how we are implementing BusUpd 
        * Suggestion 1: Do it the proper way, go to every owner process and update state of cache
        * Suggestion 2: Potentially do it the same way as Bus_MESI(?)
        */
    }
};

#endif // BUS_HPP