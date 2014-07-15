#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "logic.h"

const char DEL = 127;
const char* PS1 = "$ ";

state_t mk_state() {
  state_t state;
  asprintf(&state.cmdline, "");
  return state;
}

void state_set_cmdline(state_t* state, const char* cmdline) {
  char* new_cmdline = strdup(cmdline);
  free(state->cmdline);
  state->cmdline = new_cmdline;
}

void call(state_t* state) {
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
    execlp(state->cmdline, state->cmdline, 0);
    fprintf(stderr, "No can haz %s\n", state->cmdline);
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
      if (select(max_fd + 1, &read_fds, NULL, NULL, &tv) > 0) {
        if (FD_ISSET(child_stdout[0], &read_fds)) {
        char buf[100] = {0};
          while (read(child_stdout[0], buf, 99)) {
            printf("out %s", buf);
          }
        }
        if (FD_ISSET(child_stderr[0], &read_fds)) {
          char buf[100] = {0};
          while (read(child_stderr[0], buf, 99)) {
            fprintf(stderr, "err %s", buf);
          }
        }
      }
    } while (waitpid(pid, &st, WNOHANG) != pid);

    close(child_stdin[1]);
    close(child_stdout[0]);
    close(child_stderr[0]);
  }
}

void handle_input(const char c, state_t* state) {
  if (c == DEL) {
    state->cmdline[strlen(state->cmdline)-1] = '\0';
  } else if (c == '\n') {
    call(state);
    state_set_cmdline(state, "");
  } else {
    // Inefficient with memory allocations / copies
    // BUT: very simple.
    char* new_cmdline;
    asprintf(&new_cmdline, "%s%c", state->cmdline, c);
    state_set_cmdline(state, new_cmdline);
    free(new_cmdline);
  }
}

void render(state_t state) {
  printf("\33[2K\r%s(%lu) %s", PS1, strlen(state.cmdline), state.cmdline);
}
