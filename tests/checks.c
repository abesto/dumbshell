#include <check.h>
#include <stdlib.h>

#include "check_logic.h"
#include "check_parse.h"

int run_suite(Suite *s) {
  int number_failed;
  SRunner *sr = srunner_create(s);
  srunner_run_all (sr, CK_NORMAL);
  number_failed = srunner_ntests_failed (sr);
  srunner_free(sr);
  return number_failed;
}

#define run_suites(number_failed, ...)           \
  do{                                            \
    Suite* (*suites[])() = {__VA_ARGS__};        \
    foreach(Suite* (**suite)(), suites){           \
      number_failed += run_suite((*suite)());      \
    }                                              \
  }while(0)

int main (void) {
  int number_failed = 0;
  run_suites(number_failed, logic_suite, parse_suite);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
