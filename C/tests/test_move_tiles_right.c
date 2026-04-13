#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#define IO_STDIO
#include "../2048.h"
#include "../2048.c"


void print_move_flags(unsigned int move_flags);

int main() {
    
    state_t state = new_state();
    
    printf("Beginning the tests for move_tiles_right()\n\n");
    
    state.board[0][1] = 1;
    state.board[1][1] = 1;
    state.board[1][2] = 1;
    state.board[2][1] = 1;
    state.board[2][2] = 2;
    state.board[3][2] = 2;
    state.board[2][0] = 2;
    state.board[1][3] = 1;
    
    render_display(&state, &display);
    
    do {
        update_state(&state, INPUT_RIGHT);
        print_move_flags(state.move_flags);
        render_display(&state, &display);
    } while (updating_state(state));
    
    printf("End of tests for move_tiles_right()\n\n");
    
    return 0;
}

void print_move_flags(unsigned int move_flags) {
	for (unsigned int y = 0; y < 4; y++) {
		printf(" %u  %u  %u  %u\n\n", get_flag(move_flags, y, 0),
			get_flag(move_flags, y, 1), get_flag(move_flags, y, 2),
			get_flag(move_flags, y, 3));
	}
}
