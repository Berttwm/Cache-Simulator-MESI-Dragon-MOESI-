#include<iostream>
#include <sstream>

#include <string.h>


int main(int argc, char* argv[]) {

    enum protocal {MESI, Dragon};
    protocal curr_protocal;
    enum benchmark {blackscholes, bodytrack, fluidanimate};
    benchmark input_file;
    int cache_size = 4096; // value in bytes
    int associativity = 2;
    int block_size = 32; // value in bytes

    if (argc < 6) {
        std::cout << "[ERROR] Only " << argc << " arguments in command line, need 5 arguments." << std::endl;
        return 0;
    } else {
        if (strcmp(argv[1],"MESI")) curr_protocal = MESI;
        else if (strcmp(argv[1],"Dragon")) curr_protocal = Dragon;
        else std::cout << "[ERROR] Wrong protocol. Only MESI and Dragon are supported. " << std::endl;

        if (strcmp(argv[2],"blackscholes")) input_file = blackscholes;
        else if (strcmp(argv[2],"bodytrack")) input_file = bodytrack;
        else if (strcmp(argv[2],"fluidanimate")) input_file = fluidanimate;
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
        std::cout << block_size << std::endl;
    }



    
    return 0;
}
