#ifndef _LOGGENERATOR_HPP
#define _LOGGENERATOR_HPP

#include <fstream>

#include "Processor.h"

class LogGenerator {
private:
    std::ofstream output_log;
    std::vector<Processor*> core_list;
    std::vector<Cache*> cache_list;
public:
    void initialize(Processor* core0, Processor* core1, Processor* core2, Processor* core3) {
        core_list.push_back(core0);
        core_list.push_back(core1);
        core_list.push_back(core2);
        core_list.push_back(core3);

        cache_list.push_back(core0->get_cache());
        cache_list.push_back(core1->get_cache());
        cache_list.push_back(core2->get_cache());
        cache_list.push_back(core3->get_cache());
    }

    


};



#endif