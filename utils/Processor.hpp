#include <iostream>
#include <fstream>
#include <string>

#include "config.hpp"
#include "Cache.hpp"
#include "Bus.hpp"
class Processor {
private:    
    int total_cycle = 0;
    std::ifstream bm_file;
    int N = 1; // offset size
    int M = 1; // number of sets

    Cache* cache;
    Bus* bus;
    uint32_t instr;
    uint32_t val;
    std::string str_val;
    std::stringstream ss;
    int index_test; // PID


public:
    void initialize(int index, protocol prot, benchmark bm, int cache_size, int associativity, int block_size, Bus *bus) {
        switch (prot) {
        case protocol::MESI:
            cache = new Cache_MESI();
            cache->set_params(cache_size, associativity, block_size);
            break;
        
        case protocol::Dragon:
            cache = new Cache_Dragon();
            cache->set_params(cache_size, associativity, block_size);
            break;

        default:
            std::cout << "[ERROR] Protocol type wrong (in cache)." << std::endl; 
            return;   
        }
        
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

        if (cache_size % block_size != 0) {
            std::cout << "[ERROR] Cache size must be divisible by block size." << std::endl;
            return;
        } else if ((cache_size/block_size) % associativity != 0) {
            std::cout << "[ERROR] The total number of cache block must be divisible by associativity." << std::endl;
            return;
        }
        M = (cache_size / block_size) / associativity;
        N = block_size;

        this->bus = bus;

        return;
    }

    void run() {
        while(bm_file >> instr >> str_val) {
            ss.clear();
            ss << std::hex << str_val;
            ss >> val;
                
            // std::cout << "[" << index_test << "]  " << instr << " | " << val << std::endl;
            
            if (instr == 0 || instr == 1) {
                int set_index = (val / N) % M;
                std::cout << "[" << set_index << "]" << std::endl;
                int tag = (val / N) / M;
                if (instr == 0) { // read
                    total_cycle += cache->pr_read(set_index, tag);
                } else { // write
                    total_cycle += cache->pr_write(set_index, tag);
                }
            } else {
                if (instr != 2) {
                    std::cout << "[ERROR] Instr index value goes out of range." << std::endl;
                    return;
                }
                total_cycle += val;
            }
        }     
        return; 
    }
};