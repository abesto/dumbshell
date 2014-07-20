#include <assert.h>

#include "parse.h"

/**
 * Takes any number of strings
 * Concatenates them into an array, terminated with a NULL pointer
 */
mpc_val_t* str_array_fold(int n, mpc_val_t** xs) {
  char** vs = malloc(sizeof(char*) * (n + 1));
  for (int i = 0; i < n; i++) {
    vs[i] = strdup(xs[i]);
    free(xs[i]);
  }
  vs[n] = NULL;
  return vs;
}

mpc_val_t* argv_to_cmd(mpc_val_t* x) {
  cmd_t* cmd = malloc(sizeof(cmd_t));
  cmd->argc = 0;
  cmd->argv = x;
  while (cmd->argv[cmd->argc] != NULL) {
    cmd->argc++;
  }
  return cmd;
}

mpc_val_t* cmd_to_cmdline(mpc_val_t* x) {
  cmdline_t* cmdline = malloc(sizeof(cmdline_t));
  cmdline->cmd_count = 1;
  cmdline->cmds = malloc(sizeof(cmd_t));
  cmdline->cmds[0] = x;
  return cmdline;
}

mpc_val_t* cmdlines_fold(int n, mpc_val_t** x) {
  cmdline_t** input = (cmdline_t**)x;
  cmdline_t* cmdline = malloc(sizeof(cmdline_t));
  cmdline->cmd_count = 0;
  cmdline->cmds = NULL;
  for(int i = 0; i < n; i++) {
    //printf("%d\n", i);
    assert(input[i] != NULL);
    const int old_count = cmdline->cmd_count, new_count = old_count + input[i]->cmd_count;
    if (old_count != new_count) {
      assert(old_count < new_count);
      cmdline->cmds = realloc(cmdline->cmds, sizeof(cmdline_t) * new_count);
      for (int j = 0; j < new_count - old_count; j++) {
        cmdline->cmds[old_count + j] = input[i]->cmds[j];
      }
    }
    cmdline->cmd_count = new_count;
    free(input[i]);
  }
  return cmdline;
}

mpc_val_t* empty_cmdline() {
  cmdline_t* cmdline = malloc(sizeof(cmdline_t));
  cmdline->cmd_count = 0;
  cmdline->cmds = NULL;
  return cmdline;
}

mpc_parser_t* mk_parser() {
  mpc_parser_t* dsh_whitespace = mpc_expect(mpc_oneof(" \t"), "dsh_whitespace");
  mpc_parser_t* dsh_whitespaces = mpc_expect(mpc_many(mpcf_strfold, dsh_whitespace), "dsh_whitespaces");
  mpc_parser_t* dsh_blank = mpc_expect(mpc_apply(dsh_whitespaces, mpcf_free), "dsh_blank");

  mpc_parser_t* cmd_char = mpc_expect(mpc_or(2, mpc_alphanum(), mpc_oneof("|-")), "valid_char");
  mpc_parser_t* cmd_chars1 = mpc_expect(mpc_many1(mpcf_strfold, cmd_char), "valid_chars");
  mpc_parser_t* cmd = mpc_expect(mpc_apply(mpc_many(str_array_fold,
                                                    mpc_and(3, mpcf_snd,
                                                            dsh_blank, cmd_chars1, dsh_blank,
                                                            mpcf_dtor_null)), argv_to_cmd), "cmd");
  mpc_parser_t* one_cmd = mpc_apply(cmd, cmd_to_cmdline);
  mpc_parser_t* more_cmds = mpc_and(2, cmdlines_fold,
                                    one_cmd,
                                    mpc_many1(cmdlines_fold, mpc_and(2, mpcf_snd, mpc_char(';'), one_cmd, empty_cmdline)),
                                    empty_cmdline);
  mpc_parser_t* cmds = mpc_or(2, mpc_and(2, mpcf_fst, one_cmd, mpc_eoi()), more_cmds);

  return cmds;
}

cmdline_t* parse(const char* str) {
  static mpc_parser_t* parser = NULL;
  if (parser == NULL) {
    parser = mk_parser();
  }
  mpc_result_t result;
  mpc_parse(str, str, parser, &result);
  return result.output;
}

void free_cmd(cmd_t* cmd) {
  for (unsigned int i = 0; i < cmd->argc; i++) {
    free(cmd->argv[i]);
  }
  free(cmd);
}

void free_cmdline(cmdline_t* cmdline) {
  for (unsigned int i = 0; i < cmdline->cmd_count; i++) {
    free_cmd(cmdline->cmds[i]);
  }
  free(cmdline);
}
