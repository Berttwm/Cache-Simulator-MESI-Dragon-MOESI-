#ifndef _CONFIG_HPP
#define _CONFIG_HPP

#include <iostream>

enum protocol {MESI, Dragon};
enum benchmark {blackscholes, bodytrack, fluidanimate};
enum status_MESI {M = 1, E_MESI = 2, S = 3, I = 0};
enum status_Dragon {E_DRAGON = 2, Sc = 3, Sm = 4, D = 1, not_found = 0};

enum cache_line {status, tag};
enum op_type {read_op, write_op};

#endif