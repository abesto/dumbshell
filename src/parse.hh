#ifndef DSH_PARSE_H
#define DSH_PARSE_H

#include <string>
#include <vector>
#include <map>

#include <boost/ptr_container/ptr_vector.hpp>

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

  class Command: public std::vector<std::string> {
  private:
    std::map<unsigned int, std::vector<Redirection> > redirections;
  public:
    Command(std::string arg) { push_back(arg); }
    unsigned int argc() const { return size(); }
    const std::vector<std::string>& getArgv() const { return *this; }
  };

  class CommandLine: public std::vector<Command> {
  public:
    const std::vector<Command>& getCommands() { return *this; }
  };

  class CharHandler {
  public:
    virtual bool wants(CommandLine& cmdLine, const char c) const = 0;
    virtual void handle(CommandLine& cmdLine, const char c) const = 0;
  };

  class AppendToLastCommandLastArgv: public CharHandler {
  public:
    virtual bool wants(CommandLine& cmdLine, const char c) const;
    virtual void handle(CommandLine& cmdLine, const char c) const;
  };

  class NewArgOnSpace: public CharHandler {
  public:
    virtual bool wants(CommandLine& cmdLine, const char c) const;
    virtual void handle(CommandLine& cmdLine, const char c) const;
  };

  class NewCommandOnSemicolon: public CharHandler {
  public:
    virtual bool wants(CommandLine& cmdLine, const char c) const;
    virtual void handle(CommandLine& cmdLine, const char c) const;
  };

  class Parser {
    CommandLine cmdLine;
    boost::ptr_vector<CharHandler> charHandlers;
  public:
    Parser();
    CommandLine parse(const std::string& str);
  };

  CommandLine parse(const std::string& str);
}

#endif
