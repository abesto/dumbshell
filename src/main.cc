#include <stdio.h>

#include "mode.hh"
#include "logic.hh"

int main() {
	state_t state = mk_state();
	set_term_to_noncanonical_mode();

	while (1) {
		render(state);
		handle_input(fgetc(stdin), &state);
	}
}
