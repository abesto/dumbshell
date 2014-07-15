#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "logic.h"

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

void handle_input(const char c, state_t* state) {
  if (c == DEL) {
    state->cmdline[strlen(state->cmdline)-1] = '\0';
  } else {
    // Inefficient with memory allocations / copies
    // BUT: very simple.
    char* new_cmdline;
    asprintf(&new_cmdline, "%s%c", state->cmdline, c);
    state_set_cmdline(state, new_cmdline);
    free(new_cmdline);
  }
}

void render(state_t state) {
  printf("\33[2K\r%s(%lu) %s", PS1, strlen(state.cmdline), state.cmdline);
}
