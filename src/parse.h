#ifndef DSH_PARSE_H
#define DSH_PARSE_H

#include "../lib/mpc/mpc.h"

typedef struct {
  char** argv;
  unsigned int argc;
} cmd_t;

typedef struct {
  cmd_t** cmds;
  unsigned int cmd_count;
} cmdline_t;

mpc_parser_t* mk_parser();
cmdline_t* parse(const char* str);
void free_cmdline(cmdline_t* cmdline);

#endif
