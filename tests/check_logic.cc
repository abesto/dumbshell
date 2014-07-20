#include <boost/test/unit_test.hpp>

#include <stdlib.h>
#include <string>

#include <boost/foreach.hpp>

#include "../src/logic.hh"


BOOST_AUTO_TEST_CASE(test_mk_state) {
  BOOST_CHECK_EQUAL(mk_state().cmdline, "");
}

BOOST_AUTO_TEST_CASE(test_handle_input_backspace) {
  state_t state = mk_state();
  state_set_cmdline(&state, "foobar");
  handle_input(DEL, &state); handle_input(DEL, &state); handle_input(DEL, &state);
  BOOST_CHECK_EQUAL(state.cmdline, "foo");
}

BOOST_AUTO_TEST_CASE(test_handle_input_text) {
  state_t state = mk_state();
  std::string input = "foob\x7fo";
  BOOST_FOREACH(const char c, input) {
    handle_input(c, &state);
  }
  BOOST_CHECK_EQUAL(state.cmdline, "fooo");
}

BOOST_AUTO_TEST_CASE(test_handle_newline_with_empty_cmdline) {
  state_t state = mk_state();
  handle_input('\n', &state);
  BOOST_CHECK_EQUAL(state.cmdline, "");
}

BOOST_AUTO_TEST_CASE(test_process_printable_characters) {
  std::string cs =                  \
    "abcdefghijklmnopqrstuvfxyz"    \
    "ABCDEFGHIJKLMNOPQRSTUVFXYZ"    \
    "0123456789";
  BOOST_FOREACH(const char c, cs) {
    operation* ops;
    int ops_count = process_keypress(c, &ops);
    BOOST_CHECK_EQUAL(ops_count, 1);
    BOOST_CHECK_EQUAL(ops[0].type, APPEND_CHAR);
    BOOST_CHECK_EQUAL(ops[0].data.c, c);
    free_operations(ops);
  }
}

BOOST_AUTO_TEST_CASE(test_process_newline) {
  operation* ops;
  int ops_count = process_keypress('\n', &ops);
  BOOST_CHECK_EQUAL(ops_count, 2);
  BOOST_CHECK_EQUAL(ops[0].type, RUN_IN_FOREGROUND);
  BOOST_CHECK_EQUAL(ops[1].type, CLEAR_CMDLINE);
  free_operations(ops);
}

BOOST_AUTO_TEST_CASE(test_process_backspace) {
  operation* ops;
  int ops_count = process_keypress(DEL, &ops);
  BOOST_CHECK_EQUAL(ops_count, 1);
  BOOST_CHECK_EQUAL(ops[0].type, DELETE_LAST_CHAR);
  free_operations(ops);
}
