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

mpc_val_t* cmds_to_cmdline_fold(int n, mpc_val_t** x) {
  cmdline_t* cmdline = malloc(sizeof(cmdline_t));
  cmdline->cmd_count = n;
  cmdline->cmds = malloc(sizeof(cmd_t) * n);
  for (int i = 0; i < n; i++) {
    cmdline->cmds[i] = x[i];
  }
  return cmdline;
}

mpc_parser_t* mk_parser() {
  mpc_parser_t* dsh_whitespace = mpc_expect(mpc_oneof(" \t"), "dsh_whitespace");
  mpc_parser_t* dsh_whitespaces = mpc_expect(mpc_many(mpcf_strfold, dsh_whitespace), "dsh_whitespaces");
  mpc_parser_t* dsh_blank = mpc_expect(mpc_apply(dsh_whitespaces, mpcf_free), "dsh_blank");

  mpc_parser_t* cmd_char = mpc_expect(mpc_or(2, mpc_alphanum(), mpc_oneof("|-")), "valid_char");
  mpc_parser_t* cmd_chars1 = mpc_expect(mpc_many1(mpcf_strfold, cmd_char), "valid_chars");
  mpc_parser_t* cmd_tokens = mpc_expect(mpc_and(2, mpcf_fst,
                                                mpc_many1(str_array_fold, mpc_and(3, mpcf_snd,
                                                                                  dsh_blank, cmd_chars1, dsh_blank,
                                                                                  mpcf_dtor_null)),
                                                mpc_maybe(mpc_char(';'))), "cmd");
  mpc_parser_t* cmd = mpc_apply(cmd_tokens, argv_to_cmd);

  mpc_parser_t* cmds = mpc_expect(mpc_many1(cmds_to_cmdline_fold, cmd), "cmds");

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
