#ifndef DSH_PARSE_H
#define DSH_PARSE_H

#include "../lib/mpc/mpc.h"

mpc_parser_t* mk_parser();
mpc_result_t parse(const char* str);

#endif
