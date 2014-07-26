#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cerrno>
#include <cstring>
#include <map>
#include <boost/foreach.hpp>

#include "subprocess.hh"
#include "parse.hh"
#include "trace.hh"

typedef struct {
	int stdin[2] = { -1, -1 };
	int stdout[2] = { -1, -1 };
	int stderr[2] = { -1, -1 };
} pipes_t;

void child(dsh::Command& cmd) {
	char* argv[cmd.argc() + 1];
	for (unsigned int i = 0; i < cmd.argc(); i++) {
		argv[i] = strdup(cmd.at(i).c_str());  // To drop the const for use in exec*
	}
	argv[cmd.argc()] = NULL;

	dsh::Redirections rs = cmd.redirections;
	// input stream
	trace() << "child(" << getpid() << "): " << STDIN_FILENO << " > " << rs.find(STDIN_FILENO)->second->pipeReadFd << std::endl;
	dup2(rs.find(STDIN_FILENO)->second->pipeReadFd, STDIN_FILENO);
	trace() << "child(" << getpid() << "): close " << rs.find(STDIN_FILENO)->second->pipeWriteFd << std::endl;
	close(rs.find(STDIN_FILENO)->second->pipeWriteFd);
	// the rest are output streams
	for (auto p = rs.begin(); p != rs.end(); p++) {
		if (p->first == STDIN_FILENO) {
			continue;
		}
		trace() << "child(" << getpid() << "): " <<  p->second->fromFd << " > " << p->second->pipeWriteFd << std::endl;
		dup2(p->second->pipeWriteFd, p->second->fromFd);
		trace() << "child(" << getpid() << "): close " << p->second->pipeReadFd << std::endl;
		close(p->second->pipeReadFd);
	}
	trace() << "child(" << getpid() << "): execve " << argv[0] << std::endl;
	execvp(argv[0], &argv[0]);
	fprintf(stderr, "No can haz %s (%d)\n", argv[0], errno);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	exit(127);
}

void apply_default_redirections(dsh::Command& cmd) {
	// dsh::Redirection opens a pipe, even if input=output. Bad.
	dsh::Redirections& rs = cmd.redirections;
	if (rs.find(STDIN_FILENO) == rs.end()) {
		rs.redirectInput(dsh::Redirection(STDIN_FILENO));
	}
	if (rs.find(STDOUT_FILENO) == rs.end()) {
		rs.redirectOutput(dsh::Redirection(STDOUT_FILENO));
	}
	if (rs.find(STDERR_FILENO) == rs.end()) {
		rs.redirectOutput(dsh::Redirection(STDERR_FILENO));
	}
}

void master(dsh::CommandLine& cmdLine) {
	BOOST_FOREACH(dsh::Command& c, cmdLine) {
		for (auto p = c.redirections.begin(); p != c.redirections.end(); p++) {
			dsh::Redirection const* r = p->second;
			if (p->first == STDIN_FILENO) {
				fclose(r->pipeRead);
			} else {
				fclose(r->pipeWrite);
			}
		}
	}

	fd_set read_fds;
	struct timeval tv;

	int finishedChildCount = 0;
	while (finishedChildCount != cmdLine.size()) {
		BOOST_FOREACH(dsh::Command& c, cmdLine) {
			int status;
			if (waitpid(c.pid, &status, WNOHANG) == c.pid) {
				finishedChildCount += 1;
			}
		}

		FD_ZERO(&read_fds);
		int max_fd = 0;
		trace() << "master: will select on ";
		BOOST_FOREACH(dsh::Command& c, cmdLine) {
			for (auto p = c.redirections.begin(); p != c.redirections.end(); p++) {
				if (p->first != STDIN_FILENO) {
					int fd = p->second->pipeReadFd;
					trace() << fd << ' ';
					FD_SET(fd, &read_fds);
					if (fd > max_fd) {
						max_fd = fd;
					}
				}
			}
		}
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		trace() << ", max_fd=" << max_fd << std::endl;

		if (select(max_fd + 1, &read_fds, NULL, NULL, &tv) > 0) {
			int buf_size = 100;
			char buf[buf_size];
			BOOST_FOREACH(dsh::Command& c, cmdLine) {
				for (auto p = c.redirections.begin(); p != c.redirections.end(); p++) {
					dsh::Redirection const* r = p->second;
					if (FD_ISSET(r->pipeReadFd, &read_fds)) {
						trace()
								<< "master: forwarding " << r->pipeReadFd << "->" << r->toFd
								<< ", output of " << c.at(0) << " originally on fd " << r->fromFd << std::endl;
						while (memset(buf, 0, buf_size) && read(r->pipeReadFd, buf, buf_size - 1)) {
							write(r->toFd, buf, buf_size);
						}
					}
				}
			}
		}
	}
}

void run_in_foreground(dsh::CommandLine& cmdLine) {
	trace() << "Start\n";
	BOOST_FOREACH(dsh::Command& cmd, cmdLine) {
		apply_default_redirections(cmd);
		cmd.pid = fork();
		if (!cmd.pid) {
			child(cmd);
		} else {
			trace() << "master spawned child " << cmd.pid << " for " << cmd.at(0) << std::endl;
		}
	}

	master(cmdLine);
}
