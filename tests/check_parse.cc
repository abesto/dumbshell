#include "../src/parse.hh"
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>

BOOST_AUTO_TEST_CASE(AppendToLastCommandLastArgv_empty_CommandLine) {
  dsh::CommandLine cmdLine;
  dsh::AppendToLastCommandLastArgv h = dsh::AppendToLastCommandLastArgv();
  BOOST_CHECK(h.wants(cmdLine, 'x'));
  h.handle(cmdLine, 'x');
  BOOST_CHECK_EQUAL(cmdLine.size(), 1);
  BOOST_CHECK_EQUAL(cmdLine.at(0).argc(), 1);
  BOOST_CHECK_EQUAL(cmdLine.at(0).at(0), "x");
}

BOOST_AUTO_TEST_CASE(AppendToLastCommandLastArgv_first_command_first_argv) {
  dsh::CommandLine cmdLine;
  dsh::AppendToLastCommandLastArgv h = dsh::AppendToLastCommandLastArgv();
  cmdLine.push_back(dsh::Command("foo"));
  BOOST_CHECK(h.wants(cmdLine, 'x'));
  h.handle(cmdLine, 'x');
  BOOST_CHECK_EQUAL(cmdLine.size(), 1);
  BOOST_CHECK_EQUAL(cmdLine.at(0).argc(), 1);
  BOOST_CHECK_EQUAL(cmdLine.at(0).at(0), "foox");
}

BOOST_AUTO_TEST_CASE(AppendToLastCommandLastArgv_nonfirst) {
  dsh::CommandLine cmdLine;
  dsh::AppendToLastCommandLastArgv h = dsh::AppendToLastCommandLastArgv();
  cmdLine.push_back(dsh::Command("foo"));
  cmdLine.push_back(dsh::Command("bar"));
  BOOST_CHECK(h.wants(cmdLine, 'x'));
  h.handle(cmdLine, 'x');
  BOOST_CHECK_EQUAL(cmdLine.size(), 2);
  BOOST_CHECK_EQUAL(cmdLine.at(0).argc(), 1);
  BOOST_CHECK_EQUAL(cmdLine.at(0).at(0), "foo");
  BOOST_CHECK_EQUAL(cmdLine.at(1).argc(), 1);
  BOOST_CHECK_EQUAL(cmdLine.at(1).at(0), "barx");
}

BOOST_AUTO_TEST_CASE(NewArgOnSpace) {
  dsh::CommandLine cmdLine;
  dsh::NewArgOnSpace h = dsh::NewArgOnSpace();
  cmdLine.push_back(dsh::Command("foo"));
  cmdLine.push_back(dsh::Command("bar"));
  BOOST_CHECK_EQUAL(h.wants(cmdLine, 'x'), false);
  BOOST_CHECK_EQUAL(h.wants(cmdLine, ' '), true);
  h.handle(cmdLine, ' ');
  h.handle(cmdLine, ' ');
  BOOST_CHECK_EQUAL(cmdLine.size(), 2);
  BOOST_CHECK_EQUAL(cmdLine.at(0).argc(), 1);
  BOOST_CHECK_EQUAL(cmdLine.at(0).at(0), "foo");
  BOOST_CHECK_EQUAL(cmdLine.at(1).argc(), 2);
  BOOST_CHECK_EQUAL(cmdLine.at(1).at(0), "bar");
  BOOST_CHECK_EQUAL(cmdLine.at(1).at(1), "");
}

BOOST_AUTO_TEST_CASE(NewCommandOnSemicolon) {
  dsh::CommandLine cmdLine;
  dsh::NewCommandOnSemicolon h = dsh::NewCommandOnSemicolon();
  cmdLine.push_back(dsh::Command("foo"));
  BOOST_CHECK_EQUAL(h.wants(cmdLine, 'x'), false);
  BOOST_CHECK_EQUAL(h.wants(cmdLine, ' '), false);
  BOOST_CHECK_EQUAL(h.wants(cmdLine, ';'), true);
  h.handle(cmdLine, ';');
  BOOST_CHECK_EQUAL(cmdLine.size(), 2);
  BOOST_CHECK_EQUAL(cmdLine.at(0).argc(), 1);
  BOOST_CHECK_EQUAL(cmdLine.at(0).at(0), "foo");
  BOOST_CHECK_EQUAL(cmdLine.at(1).argc(), 1);
  BOOST_CHECK_EQUAL(cmdLine.at(1).at(0), "");
}

BOOST_AUTO_TEST_CASE(one_command) {
  const char* inputs[] = {"  ls -a -l "/*, "; ls  -a -l", "  ;ls -a -l", " ; ls -a -l"*/};
  BOOST_FOREACH(const char* input, inputs) {
    dsh::CommandLine cmdline = dsh::parse(input);
    BOOST_CHECK_EQUAL(cmdline.size(), 1);
    BOOST_CHECK_EQUAL(cmdline.at(0).argc(), 3);
    BOOST_CHECK_EQUAL(cmdline.at(0).at(0), "ls");
    BOOST_CHECK_EQUAL(cmdline.at(0).at(1), "-a");
    BOOST_CHECK_EQUAL(cmdline.at(0).at(2), "-l");
    //BOOST_CHECK(cmdline.cmds[0].redirections.empty());
  }
}

BOOST_AUTO_TEST_CASE(test_multiple_commands) {
  const char* inputs[] = {"echo; ls", " echo ; ls", "echo ;ls", "ls -a; ls -l"};
  BOOST_FOREACH(const char* input, inputs) {
    dsh::CommandLine cmdline = dsh::parse(input);
    BOOST_CHECK_EQUAL(cmdline.size(), 2);
    BOOST_CHECK_EQUAL(cmdline.at(1).at(0), "ls");
    //BOOST_CHECK(cmdline.cmds[0].redirections.empty());
    //BOOST_CHECK(cmdline.cmds[1].redirections.empty());
  }
}

/*
BOOST_AUTO_TEST_CASE(test_pipe) {
  dsh::CommandLine cmdline = dsh::parse("ls | less");
  BOOST_CHECK_EQUAL(cmdline.size(), 2);
}
*/
