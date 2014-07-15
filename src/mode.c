/**
 * Takes care of setting the terminal to the right mode, and restoring the original mode after exiting.
 */

#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>

#include "mode.h"

struct termios original_term_attributes;

void reset_term_attributes() {
  tcsetattr(STDIN_FILENO, TCSANOW, &original_term_attributes);
}

void set_term_to_noncanonical_mode() {
  struct termios tattr;
  struct termios original_term_attributes;

  if (!isatty(STDIN_FILENO)) {
    fprintf(stderr, "Not a terminal.\n");
    exit(EXIT_FAILURE);
  }

  tcgetattr(STDIN_FILENO, &original_term_attributes);
  atexit(reset_term_attributes);

  tcgetattr (STDIN_FILENO, &tattr);
  tattr.c_lflag &= ~(ICANON|ECHO);
  tattr.c_cc[VMIN] = 1;
  tattr.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);
}
