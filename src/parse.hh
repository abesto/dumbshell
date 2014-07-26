#ifndef DSH_PARSE_H
#define DSH_PARSE_H

#include <string>
#include <vector>
#include <map>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_map.hpp>

namespace dsh {
class Redirection {
	// TODO: needs to be split up into data description and logic classes
protected:
	bool shouldClosePipeOnDestruct = false;
	void openPipe() {
		int pipeFds[] = {-1, -1};
		assert(pipe(pipeFds) == 0);
		pipeWriteFd = pipeFds[1];
		pipeReadFd = pipeFds[0];
		pipeRead = fdopen(pipeReadFd, "r");
		pipeWrite = fdopen(pipeWriteFd, "w");
	}

public:
	enum {
		PIPE, FILE
	} type;
	std::FILE* pipeRead = NULL;
	std::FILE* pipeWrite = NULL;
	unsigned int pipeWriteFd = -1;
	unsigned int pipeReadFd = -1;
	unsigned int fromFd = -1;
	unsigned int toFd = -1;
	// toFd will have to be changed to a (tagged) union:
	//  - target is an fd
	//  - target is the stdin of a cmd

	Redirection(const unsigned int _fromFd, const unsigned int _toFd) {
		openPipe();
		fromFd = _fromFd;
		toFd = _toFd;
	}

	Redirection(std::FILE* from, std::FILE* to) {
		openPipe();
		fromFd = fileno(from);
		toFd = fileno(to);
	}

	Redirection(const unsigned int from, std::FILE* to) {
		openPipe();
		fromFd = from;
		toFd = fileno(to);
	}

	// No redirection
	Redirection(const unsigned int fd) {
		openPipe();
		fromFd = fd;
		toFd = fd;
	}

	~Redirection() {
		if (shouldClosePipeOnDestruct) {
			fclose(pipeRead);
			fclose(pipeWrite);
		}
	}

	void closePipeOnDestruct() {
		shouldClosePipeOnDestruct = true;
	}
};

class Redirections: public boost::ptr_multimap<unsigned int, Redirection> {
public:
	Redirections() {
	}

	Redirection* redirectInput(const Redirection& r);
	Redirection* redirectOutput(const Redirection& r);
};

// TODO: needs to be split up into at least two classes:
//  - result of parsing, data container
//  - in/output redirection implementation, pid
class Command: public std::vector<std::string> {
public:
	unsigned int pid = 0;
	Redirections redirections;
	Command(std::string arg) {
		push_back(arg);
	}
	unsigned int argc() const {
		return size();
	}
	const std::vector<std::string>&
	getArgv() const {
		return *this;
	}
	void
	redirect(const unsigned int fd, Command& to);
};

class CommandLine: public std::vector<Command> {};

class CharHandler {
public:
	virtual bool
	wants(CommandLine& cmdLine, const char c) const = 0;
	virtual void
	handle(CommandLine& cmdLine, const char c) const = 0;
	virtual ~CharHandler() {}
};

class AppendToLastCommandLastArgv: public CharHandler {
public:
	virtual bool
	wants(CommandLine& cmdLine, const char c) const;
	virtual void
	handle(CommandLine& cmdLine, const char c) const;
};

class NewArgOnSpace: public CharHandler {
public:
	virtual bool
	wants(CommandLine& cmdLine, const char c) const;
	virtual void
	handle(CommandLine& cmdLine, const char c) const;
};

class NewCommandOnSemicolon: public CharHandler {
public:
	virtual bool
	wants(CommandLine& cmdLine, const char c) const;
	virtual void
	handle(CommandLine& cmdLine, const char c) const;
};

class Pipe: public CharHandler {
public:
	virtual bool
	wants(CommandLine& cmdLine, const char c) const;
	virtual void
	handle(CommandLine& cmdLine, const char c) const;
};

class Parser {
	CommandLine cmdLine;
	boost::ptr_vector<CharHandler> charHandlers;
public:
	Parser();
	CommandLine
	parse(const std::string& str);
};

CommandLine
parse(const std::string& str);
}

#endif
