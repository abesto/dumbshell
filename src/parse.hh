#ifndef DSH_PARSE_H
#define DSH_PARSE_H

#include <string>
#include <vector>

#include "../lib/mpc/mpc.h"

namespace dsh {
  class Command {
  public:
    std::vector<std::string> argv;
    Command(std::vector<std::string> _argv): argv(_argv) {}
    unsigned int argc() const { return argv.size(); }
  };

  class CommandLine {
  public:
    std::vector<Command*> cmds;
    CommandLine() {}
    unsigned int cmdCount() const { return cmds.size(); }
  };

  CommandLine* parse(const std::string& str);
}

#endif
