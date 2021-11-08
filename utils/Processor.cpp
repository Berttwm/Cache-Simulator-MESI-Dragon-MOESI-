#include "Processor.h"

#include <sstream>
#include <string>

void Processor::initialize(int index, protocol prot, benchmark bm, int cache_size, int associativity, int block_size, int PID, Bus *bus, GlobalLock *gl) {
    this->bus = bus;
    this->gl = gl;
    this->PID = PID;
    switch (prot) {
    case protocol::MESI:
        cache = new Cache_MESI();
        cache->set_params(cache_size, associativity, block_size, PID, bus, gl);
        break;
    
    case protocol::Dragon:
        cache = new Cache_Dragon();
        cache->set_params(cache_size, associativity, block_size, PID, bus, gl);
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
    // !!! FOR TEST BRANCH ONLY !!!
    path = "test_four/test_"; 
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
    N = block_size / 4;

    return;
}

Cache* Processor::get_cache() {
    return cache;
}

void Processor::run() {
    std::string str_val;
    while(bm_file >> instr >> str_val) {
        std::stringstream ss;
        ss << std::hex << str_val;
        ss >> val;
            
        // std::cout << "[" << index_test << "]  " << instr << " | " << val << std::endl;
        
        if (instr == 0 || instr == 1) {
            int set_index = (val / N) % M;
            // std::cout << "[" << set_index << "]" << std::endl;
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
        std::cout << "Total cycles: " << total_cycle << std::endl;
    }     
    return; 
}