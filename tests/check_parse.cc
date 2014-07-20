#include "../src/parse.hh"
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>

BOOST_AUTO_TEST_CASE(one_command) {
  const char* inputs[] = {"  ls -a -l ", "; ls  -a -l", "  ;ls -a -l", " ; ls -a -l"};
  BOOST_FOREACH(const char* input, inputs) {
    dsh::CommandLine* cmdline = dsh::parse(input);
    BOOST_CHECK_EQUAL(cmdline->cmdCount(), 1);
    BOOST_CHECK_EQUAL(cmdline->cmds[0]->argc(), 3);
    BOOST_CHECK_EQUAL(cmdline->cmds[0]->argv[0], "ls");
    BOOST_CHECK_EQUAL(cmdline->cmds[0]->argv[1], "-a");
    BOOST_CHECK_EQUAL(cmdline->cmds[0]->argv[2], "-l");
    delete(cmdline);
  }
}

BOOST_AUTO_TEST_CASE(test_multiple_commands) {
  const char* inputs[] = {"echo; ls", " echo ; ls", "echo ;ls"};
  BOOST_FOREACH(const char* input, inputs) {
    dsh::CommandLine* cmdline = dsh::parse(input);
    BOOST_CHECK_EQUAL(cmdline->cmdCount(), 2);
    BOOST_CHECK_EQUAL(cmdline->cmds[1]->argv[0], "ls");
    delete(cmdline);
  }
}
