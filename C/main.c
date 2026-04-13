#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include "2048.h"


int main() {
    srand(time(NULL));
    state_t state = new_state();
    display_t display = NEW_DISPLAY;
	init_io();
	unsigned int input = 0;
	
    render_display(&state, &display);
    
    do {
    	if (!updating_state(state) && !updating_display(display)) {
        	input = get_input();
        }
        if (!updating_display(display)) {
        	update_state(&state, input);
        }
        render_display(&state, &display);
    } while (!game_over(state.board) && (input != INPUT_QUIT));
    
    if (input != INPUT_QUIT) {
    	get_input();
    }
    
    close_io();
    return 0;
}
