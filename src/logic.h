#ifndef DSH_LOGIC_H
#define DSH_LOGIC_H

const char DEL;
const char* PS1;

typedef struct {
  char * cmdline;
} state_t;

state_t mk_state();
void state_set_cmdline(state_t* state, const char* cmdline);

void handle_input(const char c, state_t* state);
void render(state_t state);
#endif
