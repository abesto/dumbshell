#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "common.h"
#include "logic.h"
#include "subprocess.h"

const char DEL = 127;
const char* PS1 = "$ ";

state_t mk_state() {
  state_t state;
  asprintf(&state.cmdline, "");
  return state;
}

void state_set_cmdline(state_t* state, const char* cmdline) {
  char* new_cmdline = strdup(cmdline);
  free(state->cmdline);
  state->cmdline = new_cmdline;
}

operation* new_operations(int count) {
  return malloc(sizeof(operation) * count);
}

unsigned int process_keypress(const char c, operation** out) {
  operation* ops;
  int count;
  if (c == '\n') {
    count = 2;
    ops = new_operations(count);
    ops[0].type = RUN_IN_FOREGROUND;
    ops[1].type = CLEAR_CMDLINE;
  } else if (c == DEL) {
    count = 1;
    ops = new_operations(count);
    ops[0].type = DELETE_LAST_CHAR;
  } else {
    count = 1;
    ops = new_operations(count);
    ops[0].type = APPEND_CHAR;
    ops[0].data.c = c;
  }
  *out = ops;
  return count;
}

void free_operations(operation* ops) {
  free(ops);
}

parsed_cmdline parse_cmdline(const char* cmdline) {
  parsed_cmdline out;
  out.argc = 0;
  out.argv = malloc(sizeof(char*));
  out._tokenized_cmdline = strdup(cmdline);
  char* token = strtok(out._tokenized_cmdline, " ");
  while (token != NULL) {
    out.argv = realloc(out.argv, sizeof(char*) * (++out.argc + 1));
    out.argv[out.argc-1] = token;
    token = strtok(NULL, " ");
  }
  out.argv[out.argc] = NULL;
  return out;
}

void free_parsed_cmdline(parsed_cmdline p) {
  free(p._tokenized_cmdline);
  free(p.argv);
}

void handle_input(const char c, state_t* state) {
  operation* ops;
  int op_count = process_keypress(c, &ops);
  for (int i = 0; i < op_count; i++) {
    operation op = ops[i];
    if (op.type == APPEND_CHAR) {
      // Inefficient with memory allocations / copies
      // BUT: very simple.
      char* new_cmdline;
      asprintf(&new_cmdline, "%s%c", state->cmdline, op.data.c);
      state_set_cmdline(state, new_cmdline);
      free(new_cmdline);
    } else if (op.type == DELETE_LAST_CHAR) {
      state->cmdline[strlen(state->cmdline)-1] = '\0';
    } else if (op.type == RUN_IN_FOREGROUND) {
      printf("\n");
      parsed_cmdline pc = parse_cmdline(state->cmdline);
      if (pc.argc > 0) {
        run_in_foreground(pc.argv);
      }
      free_parsed_cmdline(pc);
    } else if (op.type == CLEAR_CMDLINE) {
      state_set_cmdline(state, "");
    }
  }
  free_operations(ops);
}

void render(state_t state) {
  printf("\33[2K\r%s(%lu) %s", PS1, strlen(state.cmdline), state.cmdline);
}
