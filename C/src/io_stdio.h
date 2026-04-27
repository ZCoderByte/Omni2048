/*
This is a simple input/output backend for 2048 that uses stdio.
*/

#ifndef __IO_BACKEND_STDIO_INCLUDED
#define __IO_BACKEND_STDIO_INCLUDED

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "2048.h"

#define INPUT_OTHER 0
#define INPUT_UP 'u'
#define INPUT_DOWN 'd'
#define INPUT_LEFT 'l'
#define INPUT_RIGHT 'r'
#define INPUT_QUIT 'q'


typedef unsigned int display_t;
#define NEW_DISPLAY 0

#define updating_display(display) 0

/* Dummy macros, but some other io backends have and use these functions. */
#define init_io()
#define close_io() printf("\n")


/* Reads key input.  Filters out whitespace and EOF. */
unsigned int get_input() {
    int c;
    for (c = getchar(); !strchr("udlrq", tolower(c)); c = getchar()) {}
    return tolower(c);
}


void draw_row(const unsigned char row[4]) {
	
	printf("| ");
	for (unsigned int x = 0; x < 4; x++) {
		if (row[x]) {
			printf("+-------+ ");
		}
		else {
			printf("          ");
		}
	}
	printf("|\n| ");
	for (unsigned int x = 0; x < 4; x++) {
		if (row[x]) {
			printf("|       | ");
		}
		else {
			printf("          ");
		}
	}
	printf("|\n| ");
	for (unsigned int x = 0; x < 4; x++) {
		if (!row[x]) {
			printf("          ");
		}
		else if (row[x] < 7) {
			printf("|  %2u   | ", 2 << (row[x] - 1));
		}
		else if (row[x] < 10) {
			printf("|  %3u  | ", 2 << (row[x] - 1));
		}
		else if (row[x] < 14) {
			printf("| %4u  | ", 2 << (row[x] - 1));
		}
		else if (row[x] < 17) {
			printf("| %5u | ", 2 << (row[x] - 1));
		}
		else {
			printf("|%6u | ", 2 << (row[x] - 1));
		}
	}
	printf("|\n| ");
	for (unsigned int x = 0; x < 4; x++) {
		if (row[x]) {
			printf("|       | ");
		}
		else {
			printf("          ");
		}
	}
	printf("|\n| ");
	for (unsigned int x = 0; x < 4; x++) {
		if (row[x]) {
			printf("+-------+ ");
		}
		else {
			printf("          ");
		}
	}
	printf("|\n");
}


/* This um... updates the display every time a move is made. */
void render_display(const state_t* const restrict state) {
	/*if (state->should_create_tile || state->move_flags) {
		return;
	}*/
	
	printf("\n\nScore: %lu, Highest Tile: %u\n", state->score, 2 << (state->highest_tile - 1));
    printf("+-----------------------------------------+\n");
    for (unsigned int y = 0; y < 4; y++) {
    	draw_row(state->board[y]);
    }
    printf("+-----------------------------------------+\n");
    
    if (won_game(state->highest_tile) && game_over(state->board)) {
        printf("Awesome.  You win!\n\n");
    }
    else if (game_over(state->board)) {
        printf("No more moves left.  Too bad.  You lose!\n\n");
    }
    else {
    	printf("Next move (u/d/l/r/q): ");
    }
    fflush(stdout);
}

#define render_display(state, display) render_display(state)

#endif
