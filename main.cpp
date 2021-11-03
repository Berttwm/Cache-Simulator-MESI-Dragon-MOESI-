#include <iostream>
#include <sstream>
#include <thread>

#include <string.h>

#include "utils/Processor.hpp"


int main(int argc, char* argv[]) {

    protocal curr_protocal;
    benchmark input_file;
    int cache_size = 4096; // value in bytes
    int associativity = 2;
    int block_size = 32; // value in bytes

    if (argc < 6) {
        std::cout << "[ERROR] Only " << argc << " arguments in command line, need 5 arguments." << std::endl;
        return 0;
    } else {
        if (strcmp(argv[1],"MESI") == 0) curr_protocal = MESI;
        else if (strcmp(argv[1],"Dragon") == 0) curr_protocal = Dragon;
        else std::cout << "[ERROR] Wrong protocol. Only MESI and Dragon are supported. " << std::endl;

        if (strcmp(argv[2],"blackscholes") == 0) input_file = blackscholes;
        else if (strcmp(argv[2],"bodytrack") == 0) input_file = bodytrack;
        else if (strcmp(argv[2],"fluidanimate") == 0) input_file = fluidanimate;
        else std::cout << "[ERROR] Wrong benchmark. Only blackscholes, bodytrack and fluidanimate are supported. " << std::endl; 
        
        std::stringstream ss;
        ss << argv[3];
        ss >> cache_size;
        ss.clear();
        ss << argv[4];
        ss >> associativity;
        ss.clear();
        ss << argv[5];
        ss >> block_size;
    }

    Processor* core0 = new Processor();
    Processor* core1 = new Processor();
    Processor* core2 = new Processor();
    Processor* core3 = new Processor();
    core0->initialize(0, input_file, cache_size, associativity, block_size);
    core1->initialize(1, input_file, cache_size, associativity, block_size);
    core2->initialize(2, input_file, cache_size, associativity, block_size);
    core3->initialize(3, input_file, cache_size, associativity, block_size);

    std::thread th0(&Processor::run, core0);
    std::thread th1(&Processor::run, core1);
    std::thread th2(&Processor::run, core2);
    std::thread th3(&Processor::run, core3);

    th0.join();
    th1.join();
    th2.join();
    th3.join();
    

    return 0;
}
