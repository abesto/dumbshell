#include "check_parse.h"
#include "../src/parse.h"

START_TEST(test_parse_simple) {
  char** output = parse("ls -a  -l").output;
  ck_assert_str_eq("ls", output[0]);
  ck_assert_str_eq("-a", output[1]);
  ck_assert_str_eq("-l", output[2]);
} END_TEST

Suite* parse_suite() {
  Suite *s = suite_create("Parse");
  Tests(s, "simple",
        test_parse_simple);
  return s;
}
