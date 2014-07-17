#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "subprocess.h"

void run_in_foreground(const char* cmdline) {
  int child_stdin[] = {-1, -1};
  int child_stdout[] = {-1, -1};
  int child_stderr[] = {-1, -1};
  pid_t pid;

  pipe(child_stdin);
  pipe(child_stdout);
  pipe(child_stderr);

  pid = fork();
  if (!pid) {
    dup2(child_stdin[0], STDIN_FILENO); close(child_stdin[1]);
    dup2(child_stdout[1], STDOUT_FILENO); close(child_stdout[0]);
    dup2(child_stderr[1], STDERR_FILENO); close(child_stderr[0]);
    execlp(cmdline, cmdline, 0);
    fprintf(stderr, "No can haz %s\n", cmdline);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    exit(127);
  } else {
    close(child_stdin[0]);
    close(child_stdout[1]);
    close(child_stderr[1]);

    fd_set read_fds;
    struct timeval tv;

    int st;
    do {
      printf("\n");
      FD_ZERO(&read_fds);
      FD_SET(child_stdout[0], &read_fds);
      FD_SET(child_stderr[0], &read_fds);
      tv.tv_sec = 5;
      tv.tv_usec = 0;
      int max_fd = child_stdout[0] > child_stderr[0] ? child_stdout[0] : child_stderr[0];
      int buf_size = 100;
      char buf[buf_size];
      if (select(max_fd + 1, &read_fds, NULL, NULL, &tv) > 0) {
        if (FD_ISSET(child_stdout[0], &read_fds)) {
          while (memset(buf, 0, buf_size) && read(child_stdout[0], buf, buf_size-1)) {
            printf("%s", buf);
          }
        }
        if (FD_ISSET(child_stderr[0], &read_fds)) {
          while (memset(buf, 0, buf_size) && read(child_stderr[0], buf, buf_size-1)) {
            fprintf(stderr, "%s", buf);
          }
        }
      }
    } while (waitpid(pid, &st, WNOHANG) != pid);

    close(child_stdin[1]);
    close(child_stdout[0]);
    close(child_stderr[0]);
  }
}
