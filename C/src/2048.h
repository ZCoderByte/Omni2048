#ifndef __2048_INTERNALS_INCLUDED
#define __2048_INTERNALS_INCLUDED

#include <stdbool.h>
#include <stdint.h>


#define DIRECTION_NONE 0
#define DIRECTION_UP 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3
#define DIRECTION_RIGHT 4


struct state_struct {
	unsigned char board[4][4];
	unsigned long score;
	unsigned long delta_score;
	unsigned long highest_tile;
	/* Bit vectors used to denote which tiles move at a given movement stage. */
	unsigned int move_flags;
	unsigned int merge_flags;
	unsigned char direction;
	unsigned char should_create_tile;
};
typedef struct state_struct state_t;

#define get_flag(flags, y, x) (((flags) >> ((y) * 4 + (x))) & 1)
#define set_flag(flags, y, x) ((flags) | (1 << ((y) * 4 + (x))))

#define updating_state(state) ((state.should_create_tile) || (state.move_flags))

#define highest_value(value_1, value_2) (((unsigned long)(value_1) > (unsigned long)(value_2)) ? (unsigned long)(value_1) : (unsigned long)(value_2))

#define won_game(highest_tile) ((highest_tile) >= 2048)

state_t new_state();
void create_tile(state_t* state);
unsigned int game_over(const unsigned char board[4][4]);
void update_state(state_t* gamestate, unsigned int input);


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined IO_CURSES
#include "io_curses.h"
#else
#include "io_stdio.h"
#endif


/*
Creates a new state object with two tiles created on it.
*/
state_t new_state() {
	state_t state = {
    	.board = {{0, 0, 0, 0},
    			  {0, 0, 0, 0},
    			  {0, 0, 0, 0},
    			  {0, 0, 0, 0}},
    	.score = 0,
    	.highest_tile = 0,
    	.move_flags = 0,
    	.merge_flags = 0,
    	.direction = 0,
    	.should_create_tile = 0
    };
	create_tile(&state);
	create_tile(&state);
	return state;
}


unsigned int count_empty_tiles(const unsigned char board[4][4]) {
    unsigned int empty_tiles = 0;
    for (unsigned int y = 0; y < 4; y++) {
        for (unsigned int x = 0; x < 4; x++) {
            if (!board[y][x]) {
                empty_tiles++;
            }
        }
    }
    return empty_tiles;
}


/*
Randomely creates a tile with the value 2 or 4 on an open position on the board.
*/
void create_tile(state_t* state) {
    unsigned int counter = (rand() % count_empty_tiles(state->board)) + 1;
    unsigned char new_tile = (rand() % 2) + 1;
    unsigned int y = 0, x = 0;
    
    for (y = 0;; y = (y + 1) % 4) {
        for (x = 0; x < 4; x++) {
            if (state->board[y][x] == 0) {
                counter--;
            }
            if (counter == 0) {
                break;
            }
        }
        if (counter == 0) {
            break;
        }
    }
    state->board[y][x] = new_tile;
    state->highest_tile = highest_value(state->highest_tile, (1 << new_tile));
}


/*
Check if any tiles can be moved at all.  Returns true if no more tile movement
is possible (meaning the game has been lost).
*/
unsigned int game_over(const unsigned char board[4][4]) {
    // First checking if any tiles are zero (open spaces).
    if (count_empty_tiles(board)) {
    	return 0;
    }
    
    // Next checking if any horizontally ajacent tiles match.
    for (size_t y = 0; y < 4; y++) {
        for (size_t x = 0; x < 3; x++) {
            if (board[y][x] == board[y][x + 1]) {
                return 0;
            }
        }
    }
    
    // Next checking if any vertically ajacent tiles match.
    for (size_t y = 0; y < 3; y++) {
        for (size_t x = 0; x < 4; x++) {
            if (board[y][x] == board[y + 1][x]) {
                return 0;
            }
        }
    }
    // If program execution makes it this far, no tile movement is possible.
    return 1;
}


/*
Checks if it's possible to move a tile from position (src_x, src_y) to
(dest_x, dest_y).  Returns a nonzero value if the tile at (src_x, src_y) is
nonzero and the tile at (dest_x, dest_y) is zero.  Otherwise returns zero.
*/
unsigned int can_move_tile(state_t* restrict state, const unsigned int src_y,
				const unsigned int src_x, const unsigned int dest_y, const unsigned int dest_x) {

	const unsigned int src_tile  = state->board[src_y][src_x];
	const unsigned int dest_tile = state->board[dest_y][dest_x];

	if (((dest_tile == 0) || get_flag(state->move_flags, dest_y, dest_x))
		&& (src_tile != 0) && !get_flag(state->merge_flags, dest_y, dest_x)
		&& !get_flag(state->merge_flags, src_y, src_x)) {
		return set_flag(0, src_y, src_x);
    }
    return 0;
}


/*
Checks if it's possible to merge tiles from position (src_x, src_y) and
(dest_x, dest_y) to produce a tile with double the value.  Returns a nonzero
value if tiles can be merged, otherwise returns 0.
*/
unsigned long can_merge_tiles(state_t* restrict state, const unsigned int src_y, const unsigned int src_x,
                       const unsigned int dest_y, const unsigned int dest_x) {
	
	const unsigned int src_tile  = state->board[src_y][src_x];
	const unsigned int dest_tile = state->board[dest_y][dest_x];
	
	if ((src_tile != 0) && (src_tile == dest_tile) &&
		!get_flag(state->merge_flags, dest_y, dest_x) &&
		!get_flag(state->merge_flags, src_y, src_x)) {
		return 2 << dest_tile;
	}
	return 0;
}


/*

*/
void set_next_flag(state_t* restrict state, const unsigned int src_y, const unsigned int src_x,
                       const unsigned int dest_y, const unsigned int dest_x) {
	
	if (get_flag(state->merge_flags, dest_y, dest_x)) {
		return;
	}
	else if (can_move_tile(state, src_y, src_x, dest_y, dest_x)) {
		state->move_flags = set_flag(state->move_flags, src_y, src_x);
	}
	else if (can_merge_tiles(state, src_y, src_x, dest_y, dest_x)) {
		state->merge_flags = set_flag(state->merge_flags, dest_y, dest_x);
		state->delta_score += can_merge_tiles(state, src_y, src_x, dest_y, dest_x);
		state->move_flags = set_flag(state->move_flags, src_y, src_x);
	}
}


/*
Moves all tiles up.  Combines ajacent matching tiles.
*/
void set_flags_move_up(state_t* restrict state) {
    for (unsigned int x = 0; x < 4; x++) {
        for (unsigned int y = 0; y < 3; y++) {
        	set_next_flag(state, y + 1, x, y, x);
        }
    }
}


/*
Moves all tiles down.  Combines ajacent matching tiles.
*/
void set_flags_move_down(state_t* restrict state) {
    for (unsigned int x = 0; x < 4; x++) {
        for (unsigned int y = 3; y > 0; y--) {
            set_next_flag(state, y - 1, x, y, x);
        }
    }
}


/*
Moves all tiles right.  Combines ajacent matching tiles.
*/
void set_flags_move_right(state_t* restrict state) {
    for (unsigned int y = 0; y < 4; y++) {
        for (unsigned int x = 3; x > 0; x--) {
			set_next_flag(state, y, x - 1, y, x);
        }
    }
}


/*
Moves all tiles left.  Combines ajacent matching tiles.
*/
void set_flags_move_left(state_t* restrict state) {
    for (unsigned int y = 0; y < 4; y++) {
        for (unsigned int x = 0; x < 3; x++) {
			set_next_flag(state, y, x + 1, y, x);
        }
    }
}


void get_next_moves(state_t* restrict state) {
	switch (state->direction) {
		case DIRECTION_UP:
			set_flags_move_up(state);
			break;
		case DIRECTION_DOWN:
			set_flags_move_down(state);
			break;
		case DIRECTION_LEFT:
			set_flags_move_left(state);
			break;
		case DIRECTION_RIGHT:
			set_flags_move_right(state);
	}
}


void update_board_up(state_t* restrict state) {
    for (unsigned int x = 0; x < 4; x++) {
        for (unsigned int y = 0; y < 3; y++) {
        	if (get_flag(state->merge_flags, y, x) && get_flag(state->move_flags, y + 1, x)) {
        		state->board[y][x]++;
        		state->board[y + 1][x] = 0;
        		state->highest_tile = highest_value(state->highest_tile, (1 << state->board[y][x]));
        		state->score += (1 << state->board[y][x]);
        	}
        	else if ((state->board[y][x] == 0) && get_flag(state->move_flags, y + 1, x)) {
        		state->board[y][x] = state->board[y + 1][x];
        		state->board[y + 1][x] = 0;
        	}
        }
    }
}


void update_board_down(state_t* restrict state) {
    for (unsigned int x = 0; x < 4; x++) {
        for (unsigned int y = 3; y > 0; y--) {
        	if (get_flag(state->merge_flags, y, x) && get_flag(state->move_flags, y - 1, x)) {
        		state->board[y][x]++;
        		state->board[y - 1][x] = 0;
        		state->highest_tile = highest_value(state->highest_tile, (1 << state->board[y][x]));
        		state->score += (1 << state->board[y][x]);
        	}
        	else if ((state->board[y][x] == 0) && get_flag(state->move_flags, y - 1, x)) {
        		state->board[y][x] = state->board[y - 1][x];
        		state->board[y - 1][x] = 0;
        	}
        }
    }
}


void update_board_right(state_t* restrict state) {
    for (unsigned int y = 0; y < 4; y++) {
        for (unsigned int x = 3; x > 0; x--) {
        	if (get_flag(state->merge_flags, y, x) && get_flag(state->move_flags, y, x - 1)) {
        		state->board[y][x]++;
        		state->board[y][x - 1] = 0;
        		state->highest_tile = highest_value(state->highest_tile, (1 << state->board[y][x]));
        		state->score += (1 << state->board[y][x]);
        	}
        	else if ((state->board[y][x] == 0) && get_flag(state->move_flags, y, x - 1)) {
        		state->board[y][x] = state->board[y][x - 1];
        		state->board[y][x - 1] = 0;
        	}
        }
    }
}


void update_board_left(state_t* restrict state) {
    for (unsigned int y = 0; y < 4; y++) {
        for (unsigned int x = 0; x < 3; x++) {
        	if (get_flag(state->merge_flags, y, x) && get_flag(state->move_flags, y, x + 1)) {
        		state->board[y][x]++;
        		state->board[y][x + 1] = 0;
        		state->highest_tile = highest_value(state->highest_tile, (1 << state->board[y][x]));
        		state->score += (1 << state->board[y][x]);
        	}
        	else if ((state->board[y][x] == 0) && get_flag(state->move_flags, y, x + 1)) {
        		state->board[y][x] = state->board[y][x + 1];
        		state->board[y][x + 1] = 0;
        	}
        }
    }
}


void update_board(state_t* restrict state) {
	switch (state->direction) {
		case DIRECTION_UP:
			update_board_up(state);
			break;
		case DIRECTION_DOWN:
			update_board_down(state);
			break;
		case DIRECTION_LEFT:
			update_board_left(state);
			break;
		case DIRECTION_RIGHT:
			update_board_right(state);
	}
}


/*
Determines the direction of tile movement based on player input.
Returns the direction if player inputted a direction move, otherwise returns 0.
*/
unsigned int get_direction(unsigned int input) {
	switch (input) {
		case INPUT_UP:
			return DIRECTION_UP;
		case INPUT_DOWN:
			return DIRECTION_DOWN;
		case INPUT_LEFT:
			return DIRECTION_LEFT;
		case INPUT_RIGHT:
			return DIRECTION_RIGHT;
	}
	return 0;
}


/*
Um... this updates the game state!
*/
void update_state(state_t* restrict state, unsigned int input) {
    if (state->move_flags) {
		update_board(state);
		state->delta_score = 0;
		state->move_flags = 0;
		get_next_moves(state);
    }
    else if (state->should_create_tile) {
        create_tile(state);
        state->should_create_tile = 0;
        state->merge_flags = 0;
    }
    else {
    	state->direction = get_direction(input);
    	get_next_moves(state);
    	if (state->move_flags) {
    		state->should_create_tile = 1;
    	}
    }
}


#endif
