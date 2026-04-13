#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include "../io_stdio.h"
#include "../2048.h"
//#include "../io_stdio.c"
#include "../2048.c"


void print_board(uint64_t board);
void print_move_flags(unsigned int move_flags);

int main() {
    
    state_t gamestate = {
    	.boards = {0, 0, 0, 0, 0},
    	.move_flags = {0, 0, 0},
    	.input = 0,
    	.direction = 0,
    	.current_state = 0
    };
    
    printf("Beginning the tests for move_tiles_up()\n\n");
    
    gamestate.boards[0] = set_tile(gamestate.boards[0], 0, 1, 1);
    gamestate.boards[0] = set_tile(gamestate.boards[0], 1, 1, 1);
    gamestate.boards[0] = set_tile(gamestate.boards[0], 1, 2, 1);
    gamestate.boards[0] = set_tile(gamestate.boards[0], 2, 1, 1);
    gamestate.boards[0] = set_tile(gamestate.boards[0], 2, 2, 2);
    gamestate.boards[0] = set_tile(gamestate.boards[0], 3, 2, 2);
    gamestate.boards[0] = set_tile(gamestate.boards[0], 2, 0, 2);
    gamestate.boards[0] = set_tile(gamestate.boards[0], 1, 3, 1);
    
    move_tiles_up(gamestate.boards, gamestate.move_flags);
    
    print_board(gamestate.boards[0]);
    print_move_flags(gamestate.move_flags[0]);
    print_board(gamestate.boards[1]);
    print_move_flags(gamestate.move_flags[1]);
    print_board(gamestate.boards[2]);
    print_move_flags(gamestate.move_flags[2]);
    print_board(gamestate.boards[3]);
    
    printf("End of tests for move_tiles_up()\n\n");
    
    return 0;
}

void print_board(uint64_t board) {
	for (unsigned int y = 0; y < 4; y++) {
        printf("#############################\n");
        printf("#      #      #      #      #\n");
        
        for (unsigned int x = 0; x < 4; x++) {
            unsigned int next_value = get_tile(board, y, x);
            
            if (next_value == 0) {
                printf("#      ");
            }
            else if (next_value < 10) {
                printf("#%4u  ", 1 << next_value);
            }
            else {
                printf("#%5u ", 1 << next_value);
            }
        }
        printf("#\n");
        printf("#      #      #      #      #\n");
    }
    printf("#############################\n\n");
}

void print_move_flags(unsigned int move_flags) {
	for (unsigned int y = 0; y < 4; y++) {
		printf(" %u  %u  %u  %u\n\n", get_move_flag(move_flags, y, 0),
			get_move_flag(move_flags, y, 1), get_move_flag(move_flags, y, 2),
			get_move_flag(move_flags, y, 3));
	}
}
