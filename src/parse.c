#include "parse.h"

/**
 * Takes any number of strings
 * Concatenates them into an array, terminated with a NULL pointer
 */
mpc_val_t *str_array_fold(int n, mpc_val_t** xs) {
  char** vs = malloc(sizeof(char*) * (n + 1));
  for (int i = 0; i < n; i++) {
    vs[i] = strdup(xs[i]);
    free(xs[i]);
  }
  vs[n] = NULL;
  return vs;
}

mpc_parser_t* mk_parser() {
  mpc_parser_t* dsh_whitespace = mpc_expect(mpc_oneof(" \t"), "dsh_whitespace");
  mpc_parser_t* dsh_whitespaces = mpc_expect(mpc_many(mpcf_strfold, dsh_whitespace), "dsh_whitespaces");
  mpc_parser_t* dsh_blank = mpc_expect(mpc_apply(dsh_whitespaces, mpcf_free), "dsh_blank");

  mpc_parser_t* valid_char = mpc_expect(mpc_or(2, mpc_alphanum(), mpc_oneof("|-")), "valid_char");
  mpc_parser_t* valid_chars1 = mpc_expect(mpc_many1(mpcf_strfold, valid_char), "valid_chars");

  return mpc_many(str_array_fold,
                  mpc_and(2, mpcf_fst, valid_chars1, dsh_blank));
}

mpc_result_t parse(const char* str) {
  static mpc_parser_t* parser = NULL;
  if (parser == NULL) {
    parser = mk_parser();
  }
  mpc_result_t result;
  mpc_parse(str, str, parser, &result);
  return result;
}
