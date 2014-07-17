#ifndef DSH_LOGIC_H
#define DSH_LOGIC_H

const char DEL;
const char* PS1;

typedef struct {
  char * cmdline;
} state_t;

state_t mk_state();
void state_set_cmdline(state_t* state, const char* cmdline);

typedef enum { APPEND_CHAR, RUN_IN_FOREGROUND,
               DELETE_LAST_CHAR, CLEAR_CMDLINE } operation_type;
typedef union {
  char c;
} operation_data;

typedef struct {
  operation_type type;
  operation_data data;
} operation;

unsigned int process_keypress(const char c, operation** out);
void free_operations(operation* ops);

typedef struct {
  int argc;
  char** argv;
  char* _tokenized_cmdline;
} parsed_cmdline;
parsed_cmdline parse_cmdline(const char* cmdline);
void free_parsed_cmdline(parsed_cmdline p);

void handle_input(const char c, state_t* state);
void render(state_t state);
#endif
