#include "check_parse.h"
#include "../src/parse.h"

START_TEST(test_parse_simple) {
  cmdline_t* cmdline = parse("  ls -a  -l ");
  ck_assert_uint_eq(1, cmdline->cmd_count);
  ck_assert_uint_eq(3, cmdline->cmds[0]->argc);
  ck_assert_str_eq("ls", cmdline->cmds[0]->argv[0]);
  ck_assert_str_eq("-a", cmdline->cmds[0]->argv[1]);
  ck_assert_str_eq("-l", cmdline->cmds[0]->argv[2]);
  free_cmdline(cmdline);
} END_TEST

START_TEST(test_multiple_commands) {
  char* inputs[] = {"echo; ls", " echo ; ls", ";ls", " ;  ls "};
  foreach(char** input, inputs) {
    cmdline_t* cmdline = parse(*input);
    printf("%s -> %d (%s)\n", *input, cmdline->cmd_count, cmdline->cmds[0]->argv[0]);
    ck_assert_uint_eq(2, cmdline->cmd_count);
    ck_assert_str_eq("ls", cmdline->cmds[1]->argv[0]);
    free_cmdline(cmdline);
  }
} END_TEST

Suite* parse_suite() {
  Suite *s = suite_create("Parse");
  Tests(s, "simple",
        test_parse_simple,
        test_multiple_commands);
  return s;
}
