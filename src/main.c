#include <stdio.h>

#include "mode.h"
#include "logic.h"

int main() {
  state_t state = mk_state();
  set_term_to_noncanonical_mode();

  while (1) {
    render(state);
    handle_input(fgetc(stdin), &state);
  }
}
