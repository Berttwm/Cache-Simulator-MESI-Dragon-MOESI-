#include <iostream>
#include <sstream>
#include <thread>

#include <cstring>
#include <string>

#include "utils/Processor.h"
#include "utils/Bus.h"
#include "utils/GlobalLock.hpp"
#include "utils/LogGenerator.hpp"



int main(int argc, char* argv[]) {

    protocol curr_protocol;
    benchmark input_file;
    int cache_size = 4096; // value in bytes
    int associativity = 2;
    int block_size = 32; // value in bytes
    std::stringstream ss;
    std::string input_info;

    if (argc < 6) {
        std::cout << "[ERROR] Only " << argc << " arguments in command line, need 5 arguments." << std::endl;
        return 0;
    } else {
        if (strcmp(argv[1],"MESI") == 0) curr_protocol = protocol::MESI;
        else if (strcmp(argv[1],"Dragon") == 0) curr_protocol = protocol::Dragon;
        else std::cout << "[ERROR] Wrong protocol. Only MESI and Dragon are supported. " << std::endl;

        if (strcmp(argv[2],"blackscholes") == 0) input_file = benchmark::blackscholes;
        else if (strcmp(argv[2],"bodytrack") == 0) input_file = benchmark::bodytrack;
        else if (strcmp(argv[2],"fluidanimate") == 0) input_file = benchmark::fluidanimate;
        else std::cout << "[ERROR] Wrong benchmark. Only blackscholes, bodytrack and fluidanimate are supported. " << std::endl; 
        
        ss.clear();
        ss << argv[3];
        ss >> cache_size;
        ss.clear();
        ss << argv[4];
        ss >> associativity;
        ss.clear();
        ss << argv[5];
        ss >> block_size;

        
        std::string temp;
        for (int i = 1; i < 6; i++) {
            ss.clear();
            ss << argv[i];
            ss >> temp;
            if (i == 1) {
                input_info = temp;
            } else {
                input_info = input_info + "_" + temp;
            }
        }
    }
    // Initialize GlobalLock
    GlobalLock *gl = new GlobalLock(cache_size, associativity, block_size);

    // Initialize Bus:
    Bus *bus;
    switch (curr_protocol) {
        case protocol::MESI:
            bus = new Bus_MESI();
            break;
        case protocol::Dragon:
            bus = new Bus_Dragon();
            break;
        default:
            std::cout << "[ERROR] Protocol type wrong (in bus)." << std::endl; 
            return -1; 
    }
    bus->init_bus(cache_size, associativity, block_size, gl);

    LogGenerator *log_generator = new LogGenerator();

    // Initialize Cores:
    Processor* core0 = new Processor();
    Processor* core1 = new Processor();
    Processor* core2 = new Processor();
    Processor* core3 = new Processor();
    core0->initialize(0, curr_protocol, input_file, cache_size, associativity, block_size, 0, bus, gl);
    core1->initialize(1, curr_protocol, input_file, cache_size, associativity, block_size, 1, bus, gl);
    core2->initialize(2, curr_protocol, input_file, cache_size, associativity, block_size, 2, bus, gl);
    core3->initialize(3, curr_protocol, input_file, cache_size, associativity, block_size, 3, bus, gl);

    bus->init_cache(core0->get_cache(), core1->get_cache(), core2->get_cache(), core3->get_cache());
    log_generator->initialize(core0, core1, core2, core3, input_info);

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
