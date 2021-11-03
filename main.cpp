#include<iostream>
#include <sstream>
#include<thread>

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

    Processor core0;
    Processor core1;
    Processor core2;
    Processor core3;
    core0.load_input_file(input_file, 0);
    core1.load_input_file(input_file, 1);
    core2.load_input_file(input_file, 2);
    core3.load_input_file(input_file, 3);



    th1.join();
    
    return 0;
}
