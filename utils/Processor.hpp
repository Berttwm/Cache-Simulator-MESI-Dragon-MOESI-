#include<iostream>
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
public:
    void load_input_file(benchmark bm, int index) {
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
        bm_file.open(path, std::ifstream::in);
        return;
    }

    void run() {
        bm_file >> instr >> str_val;
        ss.clear();
        ss << std::hex << str_val;
        ss >> val;
            
        std::cout << instr << " | " << val << std::endl;        
        return; 
    }
};