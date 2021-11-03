#include <iostream>
#include <vector>

#include "config.hpp"

class Cache {
private:
    int max_index;
    std::vector<std::vector<std::vector<int>>> dummy_cache; 
public:
    // Construct a dummy cache with shape: associativity, num of cache set, 2
    // 2: index 0 for states, index 1 for tag
    void set_params(int blk_size, int cache_size, int associativity) {
        max_index = (cache_size / blk_size) / associativity;
        std::vector<int> temp(2, 0);
        std::vector<std::vector<int>> temp2;
        for (int i = 0; i < max_index; i++) {
            temp2.push_back(temp);
        }
        for (int i = 0; i < associativity; i++) {
            dummy_cache.push_back(temp2);
        }
    }
    
};