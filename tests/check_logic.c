#include <stdlib.h>
#include <stdarg.h>

#include "../src/logic.h"

#include "check_logic.h"

START_TEST(test_mk_state) {
  ck_assert_str_eq(mk_state().cmdline, "");
} END_TEST

START_TEST(test_handle_input_backspace) {
  state_t state = mk_state();
  state_set_cmdline(&state, "foobar");
  handle_input(DEL, &state);
  handle_input(DEL, &state);
  handle_input(DEL, &state);
  ck_assert_str_eq(state.cmdline, "foo");
} END_TEST

START_TEST(test_handle_input_text) {
  state_t state = mk_state();
  foreach(char *c, "foob") {
    handle_input(*c, &state);
  }
  handle_input(DEL, &state);
  handle_input('o', &state);
  ck_assert_str_eq(state.cmdline, "fooo");
} END_TEST

Suite* logic_suite() {
  Suite *s = suite_create("Logic");
  Tests(s, "Main",
        test_mk_state,
        test_handle_input_backspace,
        test_handle_input_text);
  return s;
}
