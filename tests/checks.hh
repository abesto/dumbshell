#ifndef DSH_CHECKS_H
#define DSH_CHECKS_H

#include <check.h>
#include "../src/common.hh"

#define add_tests(tc, ...)                      \
  do{                                           \
    TFun _tests_[] = {__VA_ARGS__};             \
    foreach(TFun* f, _tests_) {                 \
      tcase_add_test(tc, *f);                   \
    }                                           \
  }while(0)

#define Tests(s, name, ...)                     \
  do{                                           \
    TCase *tc = tcase_create(name);             \
    add_tests(tc, __VA_ARGS__);                 \
    suite_add_tcase(s, tc);                     \
  }while(0)

#define times(n) for (int _i_ = 0; _i_ < n; _i_++)

#endif
