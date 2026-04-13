#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include "../io_stdio.h"
#include "../2048.h"
#include "../io_stdio.c"
#include "../2048.c"


int main() {
    
    state_t gamestate = {
    	.boards = {0, 0, 0, 0, 0},
    	.move_flags = {0, 0, 0},
    	.input = 0,
    	.direction = 0,
    	.current_state = 0
    };
    
    printf("Beginning the tests for move_tile()\n\n");
    
    gamestate.boards[4] = set_tile(gamestate.boards[4], 2, 2, 1);
    
    update_display(&gamestate);
    
    gamestate.boards[4] = move_tile(gamestate.boards[4], 2, 2, 1, 2);
    gamestate.boards[4] = move_tile(gamestate.boards[4], 2, 1, 1, 1);
    update_display(&gamestate);
    
    printf("End of tests for move_tile()\n\n");
    
    return 0;
}
