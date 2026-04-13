#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include "../io_stdio.h"
#include "../2048.h"
#include "../io_stdio.c"
#include "../2048.c"


int main() {
    
    state_t state = new_state();
    
    printf("Beginning the tests for combine_tiles()\n\n");
    
    state.boards[4] = set_tile(state.boards[4], 2, 2, 1);
    state.boards[4] = set_tile(state.boards[4], 2, 1, 1);
    
    update_display(&state);
    
    state.boards[4] = combine_tiles(state.boards[4], 2, 2, 2, 1);
    update_display(&state);
    
    printf("End of tests for combine_tiles()\n\n");
    
    return 0;
}
