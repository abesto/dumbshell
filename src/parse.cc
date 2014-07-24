#include <cassert>
#include <sstream>
#include <boost/foreach.hpp>

#include "parse.hh"

bool dsh::AppendToLastCommandLastArgv::wants(CommandLine& cmdLine, const char c) const {
  return true;
}

void dsh::AppendToLastCommandLastArgv::handle(CommandLine& cmdLine, const char c) const {
  assert(wants(cmdLine, c));
  if (cmdLine.size() == 0) {
    std::stringstream ss;
    ss << c;
    cmdLine.push_back(dsh::Command(ss.str()));
  } else {
    cmdLine.back().back().push_back(c);
  }
}

bool dsh::NewArgOnSpace::wants(CommandLine& cmdLine, const char c) const {
  return c == ' ';
}

void dsh::NewArgOnSpace::handle(CommandLine& cmdLine, const char c) const {
  assert(wants(cmdLine, c));
  if (cmdLine.size() > 0 && cmdLine.back().argc() > 0 && cmdLine.back().back().length() > 0) {
    cmdLine.back().push_back("");
  }
}

bool dsh::NewCommandOnSemicolon::wants(CommandLine& cmdLine, const char c) const {
  return c == ';';
}

void dsh::NewCommandOnSemicolon::handle(CommandLine& cmdLine, const char c) const {
  assert(wants(cmdLine, c));
  if (cmdLine.size() > 0 && cmdLine.back().argc() > 0) {
    cmdLine.push_back(Command(""));
  }
}

dsh::Parser::Parser() {
  charHandlers.push_back(new NewArgOnSpace());
  charHandlers.push_back(new NewCommandOnSemicolon());
  charHandlers.push_back(new AppendToLastCommandLastArgv());
}

dsh::CommandLine dsh::Parser::parse(const std::string& str) {
  assert(this->cmdLine.size() == 0);
  // Run first handler that wants this character
  BOOST_FOREACH(char c, str) {
    BOOST_FOREACH(CharHandler& h, charHandlers) {
      if (h.wants(cmdLine, c)) {
        h.handle(cmdLine, c);
        break;
      }
    }
  }
  // For each command: strip any empty args we might have at the end
  // There will be an empty arg at the end if there was a space trailing the last arg
  BOOST_FOREACH(dsh::Command& c, cmdLine) {
    while (c.argc() > 0 && c.back().size() == 0) {
      c.pop_back();
    }
  }
  // Remove any commands that don't have any args (after the cleanup above)
  while (cmdLine.size() > 0 && cmdLine.back().argc() == 0) {
    cmdLine.pop_back();
  }
  dsh::CommandLine retval = this->cmdLine;
  this->cmdLine = CommandLine();  // Maintain invariant: when not in parse(), cmdLine is empty
  return retval;
}

dsh::CommandLine dsh::parse(const std::string& str) {
  static dsh::Parser p;
  return p.parse(str);
}
