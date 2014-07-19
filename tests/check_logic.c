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
  times(3) handle_input(DEL, &state);
  ck_assert_str_eq(state.cmdline, "foo");
} END_TEST

START_TEST(test_handle_input_text) {
  state_t state = mk_state();
  foreach(char *c, "foob\x7fo") handle_input(*c, &state);
  ck_assert_str_eq(state.cmdline, "fooo");
} END_TEST

START_TEST(test_handle_newline_with_empty_cmdline) {
  state_t state = mk_state();
  handle_input('\n', &state);
  ck_assert_str_eq(state.cmdline, "");
} END_TEST

START_TEST(test_process_printable_characters) {
  const char cs[] =                 \
    "abcdefghijklmnopqrstuvfxyz"    \
    "ABCDEFGHIJKLMNOPQRSTUVFXYZ"    \
    "0123456789";
  foreach(const char* c, cs) {
    operation* ops;
    int ops_count = process_keypress(*c, &ops);
    ck_assert_uint_eq(ops_count, 1);
    ck_assert_uint_eq(ops[0].type, APPEND_CHAR);
    ck_assert_uint_eq(ops[0].data.c, *c);
    free_operations(ops);
  }
} END_TEST

START_TEST(test_process_newline) {
  operation* ops;
  int ops_count = process_keypress('\n', &ops);
  ck_assert_uint_eq(ops_count, 2);
  ck_assert_uint_eq(ops[0].type, RUN_IN_FOREGROUND);
  ck_assert_uint_eq(ops[1].type, CLEAR_CMDLINE);
  free_operations(ops);
} END_TEST

START_TEST(test_process_backspace) {
  operation* ops;
  int ops_count = process_keypress(DEL, &ops);
  ck_assert_uint_eq(ops_count, 1);
  ck_assert_uint_eq(ops[0].type, DELETE_LAST_CHAR);
  free_operations(ops);
} END_TEST

Suite* logic_suite() {
  Suite *s = suite_create("Logic");
  Tests(s, "State",
        test_mk_state,
        test_handle_input_backspace,
        test_handle_input_text,
        test_handle_newline_with_empty_cmdline);
  Tests(s, "process_keypress",
        test_process_printable_characters,
        test_process_newline,
        test_process_backspace);
  return s;
}
