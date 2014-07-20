#ifndef DSH_PARSE_H
#define DSH_PARSE_H

#include <string>
#include <vector>
#include <map>

namespace dsh {
  class Command;

  class Redirection {
  public:
    enum {PIPE, FILE} type;
    union {
      const Command* cmd;
      const char** filename;
    } target;

    Redirection(const Command* cmd): type(PIPE) {
      target.cmd = cmd;
    }

    Redirection(const char** filename): type(FILE) {
      target.filename = filename;
    }
  };

  class Command {
  public:
    std::vector<std::string> argv;
    std::map<unsigned int, std::vector<Redirection> > redirections;

    Command(std::vector<std::string> _argv): argv(_argv) {}
    unsigned int argc() const { return argv.size(); }
  };

  class CommandLine {
  public:
    std::vector<Command> cmds;
    CommandLine() {}
    CommandLine(Command cmd) { cmds.push_back(cmd); }
    CommandLine(std::vector<std::string> argv) { cmds.push_back(Command(argv)); }
    unsigned int cmdCount() const { return cmds.size(); }
  };

  CommandLine parse(const std::string& str);
}

#endif
