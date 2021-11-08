#ifndef _CACHE_H
#define _CACHE_H

#include <vector>

#include "GlobalLock.hpp"

class Bus;

class Cache {
public:
    
    int PID;
    int num_sets;
    int num_ways;
    Bus *bus;
    GlobalLock *gl;
    std::vector<std::vector<std::vector<int>>> dummy_cache;

    // Statistics
    int num_cache_miss = 0;
    int num_data_traffic = 0;
    int num_update = 0;
    int num_access_private = 0;
    int num_access_shared = 0;

    // Construct a dummy cache with shape: associativity(num_ways), num of cache set, 2
    // 2: index 0 for states, index 1 for tag
    void set_params(int cache_size, int associativity, int blk_size, int PID, Bus *bus, GlobalLock *gl);

    /*
    ** To maintain LRU replacement policy, old data in the given cache set are shifted to left
    * 
    */
    void shift_cacheline_left(int i_set);
    void shift_cacheline_left_until(int i_set, int pos);

    virtual int pr_read(int i_set, int tag) = 0;
    virtual int pr_write(int i_set, int tag) = 0;
    /* Cache to bus transactions */

    virtual int get_status_cacheline(int i_set, int tag) = 0;
    virtual int set_status_cacheline(int i_set, int tag) = 0;
        
};

class Cache_MESI : public Cache {
private:
public:
    int pr_read(int i_set, int tag);
    int pr_write(int i_set, int tag);
    int get_status_cacheline(int i_set, int tag);
    int set_status_cacheline(int i_set, int tag);

};

class Cache_Dragon : public Cache {
private:
public:
    int pr_read(int i_set, int tag);
    int pr_write(int i_set, int tag);
    int get_status_cacheline(int i_set, int tag);
    int set_status_cacheline(int i_set, int tag);
};


#endif