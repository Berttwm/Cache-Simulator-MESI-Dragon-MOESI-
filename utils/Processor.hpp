#include <iostream>
#include <fstream>
#include <string>

#include "config.hpp"
#include "Cache.hpp"

class Processor {
private:    
    int total_cycle = 0;
    std::ifstream bm_file;
    uint32_t instr;
    uint32_t val;
    std::string str_val;
    std::stringstream ss;
    int index_test;

    Cache* cache = new Cache();
public:
    void initialize(int index, benchmark bm, int cache_size, int associativity, int block_size) {
        std::string path;
        switch(bm) {
            case benchmark::blackscholes:
                path = "blackscholes_four/blackscholes_";
                break;
            case benchmark::bodytrack:
                path = "bodytrack_four/bodytrack_";
                break;
            case benchmark::fluidanimate:
                path = "fluidanimate_four/fluidanimate_";
                break;  
        }
        path += std::to_string(index) + ".data";
        index_test = index;
        bm_file.open(path, std::ifstream::in);
        // std::cout << bm_file.is_open() << std::endl;

        cache->set_params(cache_size, associativity, block_size);

        return;
    }

    void run() {
        while(bm_file >> instr >> str_val) {
            ss.clear();
            ss << std::hex << str_val;
            ss >> val;
                
            std::cout << "[" << index_test << "]  " << instr << " | " << val << std::endl;   
        }     
        return; 
    }
};