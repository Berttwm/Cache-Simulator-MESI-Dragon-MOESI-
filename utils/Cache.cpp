#include<iostream>

class Cache {
private:
    int cache_size;
    int blk_size;
    int associativity; 
public:
    void set_params(int cache, int associat, int blk) {
        cache_size = cache;
        associativity = associat;
        blk_size = blk_size;
    }
    
};