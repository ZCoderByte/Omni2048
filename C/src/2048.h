#ifndef __2048_INTERNALS_INCLUDED
#define __2048_INTERNALS_INCLUDED

#include <stdbool.h>
#include <stdint.h>


#define DIRECTION_NONE 0
#define DIRECTION_UP 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3
#define DIRECTION_RIGHT 4


typedef struct {
    unsigned int y;
    unsigned int x;
} point;

/* Bit vectors used to denote which tiles move at a given movement stage. */
typedef struct {
    unsigned int move;
    unsigned int merge;
} flags_t;

typedef struct {
	unsigned long board[4][4];
	unsigned long score;
	unsigned long highest_tile;
	flags_t flags;
	unsigned char direction;
	unsigned char should_create_tile;
} state_t;

#define get_flag(flags, y, x) (((flags) >> ((y) * 4 + (x))) & 1)
#define set_flag(flags, y, x) ((flags) | (1 << ((y) * 4 + (x))))

#define updating_state(state) ((state.should_create_tile) || (state.flags.move))

#define max(a, b) ((a) ^ (((a) ^ (b)) & -((a) < (b))))

#define won_game(highest_tile) ((highest_tile) >= 2048)

state_t new_state();
void create_tile(unsigned long (*board)[4]);
unsigned long get_highest_tile(const unsigned long (*board)[4]);
unsigned int game_over(const unsigned long (*board)[4]);
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
    	.flags = {0, 0},
    	.direction = 0,
    	.should_create_tile = 0
    };
	create_tile(state.board);
	create_tile(state.board);
	state.highest_tile = get_highest_tile(state.board);
	return state;
}


unsigned int count_empty_tiles(const unsigned long (*board)[4]) {
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


unsigned long get_highest_tile(const unsigned long (*board)[4]) {
	unsigned long highest_tile = 0;
	for (unsigned int y = 0; y < 4; y++) {
        for (unsigned int x = 0; x < 4; x++) {
            highest_tile = max(highest_tile, board[y][x]);
        }
    }
    return highest_tile;
}


/*
Randomely creates a tile with the value 2 or 4 on an open position on the board.
*/
void create_tile(unsigned long (*board)[4]) {
    unsigned int counter = (rand() % count_empty_tiles(board)) + 1;
    unsigned char new_tile = ((rand() % 2) + 1) * 2;
    unsigned int y = 0, x = 0;
    
    for (y = 0;; y = (y + 1) % 4) {
        for (x = 0; x < 4; x++) {
            if (board[y][x] == 0) {
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
    board[y][x] = new_tile;
}


/*
Check if any tiles can be moved at all.  Returns true if no more tile movement
is possible (meaning the game has been lost).
*/
unsigned int game_over(const unsigned long (*board)[4]) {
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
Checks if it's possible to move a tile from position (src.x, src.y) to
(dest.x, dest.y).  Returns a nonzero value if the tile at (src.x, src.y) is
nonzero and the tile at (dest.x, dest.y) is zero.  Otherwise returns zero.
*/
unsigned int can_move_tile(const unsigned long (*board)[4],
	const flags_t flags, const point src, const point dest) {

	const unsigned int src_tile  = board[src.y][src.x];
	const unsigned int dest_tile = board[dest.y][dest.x];

	if (((dest_tile == 0) || get_flag(flags.move, dest.y, dest.x))
		&& (src_tile != 0) && !get_flag(flags.merge, dest.y, dest.x)
		&& !get_flag(flags.merge, src.y, src.x)) {
		return set_flag(0, src.y, src.x);
    }
    return 0;
}


/*
Checks if it's possible to merge tiles from position (src.x, src.y) and
(dest.x, dest.y) to produce a tile with double the value.  Returns a nonzero
value if tiles can be merged, otherwise returns 0.
*/
unsigned long can_merge_tiles(const unsigned long (*board)[4],
	const unsigned int merge_flags, const point src, const point dest) {
	
	const unsigned int src_tile  = board[src.y][src.x];
	const unsigned int dest_tile = board[dest.y][dest.x];
	
	if ((src_tile != 0) && (src_tile == dest_tile) &&
		!get_flag(merge_flags, dest.y, dest.x) &&
		!get_flag(merge_flags, src.y, src.x)) {
		return 2 << dest_tile;
	}
	return 0;
}


/*

*/
void set_next_flag(state_t* restrict state, const point src, const point dest) {
	
	if (get_flag(state->flags.merge, dest.y, dest.x)) {
		return;
	}
	else if (can_move_tile(state->board, state->flags, src, dest)) {
		state->flags.move = set_flag(state->flags.move, src.y, src.x);
	}
	else if (can_merge_tiles(state->board, state->flags.merge, src, dest)) {
		state->flags.merge = set_flag(state->flags.merge, dest.y, dest.x);
		state->flags.move = set_flag(state->flags.move, src.y, src.x);
	}
}


/*
Moves all tiles up.  Combines ajacent matching tiles.
*/
void set_flags_move_up(state_t* restrict state) {
    for (unsigned int x = 0; x < 4; x++) {
        for (unsigned int y = 0; y < 3; y++) {
        	set_next_flag(state, (point){y + 1, x}, (point){y, x});
        }
    }
}


/*
Moves all tiles down.  Combines ajacent matching tiles.
*/
void set_flags_move_down(state_t* restrict state) {
    for (unsigned int x = 0; x < 4; x++) {
        for (unsigned int y = 3; y > 0; y--) {
            set_next_flag(state, (point){y - 1, x}, (point){y, x});
        }
    }
}


/*
Moves all tiles right.  Combines ajacent matching tiles.
*/
void set_flags_move_right(state_t* restrict state) {
    for (unsigned int y = 0; y < 4; y++) {
        for (unsigned int x = 3; x > 0; x--) {
			set_next_flag(state, (point){y, x - 1}, (point){y, x});
        }
    }
}


/*
Moves all tiles left.  Combines ajacent matching tiles.
*/
void set_flags_move_left(state_t* restrict state) {
    for (unsigned int y = 0; y < 4; y++) {
        for (unsigned int x = 0; x < 3; x++) {
			set_next_flag(state, (point){y, x + 1}, (point){y, x});
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


unsigned long move_next_tile(unsigned long (*board)[4], const flags_t flags,
	const point src, const point dest) {
	
	if (get_flag(flags.merge, dest.y, dest.x) && get_flag(flags.move, src.y, src.x)) {
        board[dest.y][dest.x] += board[src.y][src.x];
        board[src.y][src.x] = 0;
        return board[dest.y][dest.x];
    }
    else if ((board[dest.y][dest.x] == 0) && get_flag(flags.move, src.y, src.x)) {
        board[dest.y][dest.x] = board[src.y][src.x];
        board[src.y][src.x] = 0;
    }
    return 0;
}


unsigned long update_board_up(unsigned long (*board)[4], const flags_t flags) {
    unsigned long delta_score = 0;
    for (unsigned int x = 0; x < 4; x++) {
        for (unsigned int y = 0; y < 3; y++) {
        	delta_score += move_next_tile(board, flags,
        		(point){y + 1, x}, (point){y, x});
        }
    }
    return delta_score;
}


unsigned long update_board_down(unsigned long (*board)[4], const flags_t flags) {
    unsigned long delta_score = 0;
    for (unsigned int x = 0; x < 4; x++) {
        for (unsigned int y = 3; y > 0; y--) {
        	delta_score += move_next_tile(board, flags,
        		(point){y - 1, x}, (point){y, x});
        }
    }
    return delta_score;
}


unsigned long update_board_right(unsigned long (*board)[4], const flags_t flags) {
    unsigned long delta_score = 0;
    for (unsigned int y = 0; y < 4; y++) {
        for (unsigned int x = 3; x > 0; x--) {
        	delta_score += move_next_tile(board, flags,
        		(point){y, x - 1}, (point){y, x});
        }
    }
    return delta_score;
}


unsigned long update_board_left(unsigned long (*board)[4], const flags_t flags) {
    unsigned long delta_score = 0;
    for (unsigned int y = 0; y < 4; y++) {
        for (unsigned int x = 0; x < 3; x++) {
        	delta_score += move_next_tile(board, flags,
        		(point){y, x + 1}, (point){y, x});
        }
    }
    return delta_score;
}


unsigned long update_board(unsigned long (*board)[4], const flags_t flags, const unsigned int direction) {
	switch (direction) {
		case DIRECTION_UP:
			return update_board_up(board, flags);
			break;
		case DIRECTION_DOWN:
			return update_board_down(board, flags);
			break;
		case DIRECTION_LEFT:
			return update_board_left(board, flags);
			break;
		case DIRECTION_RIGHT:
			return update_board_right(board, flags);
	}
	return 0;
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
    if (state->flags.move) {
		state->score += update_board(state->board, state->flags, state->direction);
		state->flags.move = 0;
		get_next_moves(state);
    }
    else if (state->should_create_tile) {
        create_tile(state->board);
        state->should_create_tile = 0;
        state->flags.merge = 0;
    }
    else {
    	state->direction = get_direction(input);
    	get_next_moves(state);
    	if (state->flags.move) {
    		state->should_create_tile = 1;
    	}
    }
}


#endif
