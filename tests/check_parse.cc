#include "../src/parse.hh"

#include <cstdio>
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

BOOST_AUTO_TEST_CASE(Redirections_stdin) {
  dsh::Redirections rs;
  dsh::Redirection in = dsh::Redirection(stdin, stdout);
  rs.redirectInput(dsh::Redirection(stdin, stderr));
  rs.redirectInput(in);
  BOOST_CHECK_EQUAL(rs.count(STDIN_FILENO), 1);
  dsh::Redirection const* out = rs.find(STDIN_FILENO)->second;
  BOOST_CHECK_EQUAL(out->type, in.type);
  BOOST_CHECK_EQUAL(out->toFd, in.toFd);
}

BOOST_AUTO_TEST_CASE(Redirections_other) {
  dsh::Redirections rs;
  const unsigned int fd = 20;
  dsh::Redirection fooR = dsh::Redirection(fd, stdout);
  dsh::Redirection barR = dsh::Redirection(fd, stderr);
  rs.redirectOutput(fooR);
  rs.redirectOutput(barR);
  BOOST_CHECK_EQUAL(rs.count(fd), 2);

  bool foo_found = false, bar_found = false;
  for (auto p = rs.find(fd); p != rs.end(); p++) {
    foo_found |= (p->second->toFd == fooR.toFd);
    bar_found |= (p->second->toFd == barR.toFd);
  }
  BOOST_CHECK(foo_found);
  BOOST_CHECK(bar_found);
}

BOOST_AUTO_TEST_CASE(Command_redirect) {
  dsh::Command left = dsh::Command("echo foo");
  dsh::Command right = dsh::Command("less");
  left.redirect(STDOUT_FILENO, right);
  BOOST_CHECK_EQUAL(left.redirections.size(), 1);
  BOOST_CHECK_EQUAL(right.redirections.size(), 1);
  BOOST_CHECK_EQUAL(
    		left.redirections.find(STDOUT_FILENO)->second->pipeWriteFd,
    		right.redirections.find(STDIN_FILENO)->second->pipeWriteFd);

  FILE* out = left.redirections.find(STDOUT_FILENO)->second->pipeWrite;
  FILE* in = right.redirections.find(STDIN_FILENO)->second->pipeRead;

  fwrite("foo\0", 4, 1, out);
  fflush(out);
  char buf[4];
  fread(buf, 4, 1, in);
  BOOST_CHECK_EQUAL(buf, "foo");
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
    BOOST_CHECK(cmdline.at(0).redirections.empty());
  }
}

BOOST_AUTO_TEST_CASE(test_multiple_commands) {
  const char* inputs[] = {"echo; ls", " echo ; ls", "echo ;ls", "ls -a; ls -l"};
  BOOST_FOREACH(const char* input, inputs) {
    dsh::CommandLine cmdline = dsh::parse(input);
    BOOST_CHECK_EQUAL(cmdline.size(), 2);
    BOOST_CHECK_EQUAL(cmdline.at(1).at(0), "ls");
    BOOST_CHECK(cmdline.at(0).redirections.empty());
    BOOST_CHECK(cmdline.at(1).redirections.empty());
  }
}

BOOST_AUTO_TEST_CASE(test_pipe) {
  dsh::CommandLine cmdline = dsh::parse("ls | less");
  BOOST_CHECK_EQUAL(cmdline.size(), 2);
  BOOST_CHECK_EQUAL(cmdline.at(0).redirections.size(), 1);
}
