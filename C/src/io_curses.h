/*
This is an input/output backend for 2048 built upon Curses, NCurses, PDCurses,
etc.
*/

#ifndef __IO_BACKEND_CURSES_INCLUDED
#define __IO_BACKEND_CURSES_INCLUDED

#include <curses.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "2048.h"

#define INPUT_OTHER 0
#define INPUT_UP 1
#define INPUT_DOWN 2
#define INPUT_LEFT 3
#define INPUT_RIGHT 4
#define INPUT_QUIT 5


struct display_struct {
	unsigned int frames;
	unsigned int total_frames;
};
typedef struct display_struct display_t;

#define NEW_DISPLAY { .frames = 0, .total_frames = 0 }

#define updating_display(display) (display.frames)

/*
Defines a dummy macro for set_escdelay when compiling for Windows.
*/
#ifdef _WIN32
#define set_escdelay(ms)
#endif

/*
Sets up curses and the terminal.  Also sets up color when availible.
*/
void init_io() {
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    set_escdelay(100);
    
    if (has_colors()) {
    	start_color();
    	init_pair(0, COLOR_BLACK, COLOR_BLACK);
    	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    	init_pair(2, COLOR_GREEN, COLOR_BLACK);
    	init_pair(3, COLOR_BLUE, COLOR_BLACK);
    	init_pair(4, COLOR_CYAN, COLOR_BLACK);
    	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    	init_pair(6, COLOR_WHITE, COLOR_BLACK);
    }
}

/*
Restores the terminal to it's original state.
*/
void close_io() {
    echo();
    endwin();
}

/*
Reads key input.  Filters out whitespace and EOF.
*/
unsigned int get_input() {
	flushinp();
        switch (getch()) {
            case 'w':
            case 'W':
            case KEY_UP:
                return INPUT_UP;
            case 's':
            case 'S':
            case KEY_DOWN:
                return INPUT_DOWN;
            case 'a':
            case 'A':
            case KEY_LEFT:
                return INPUT_LEFT;
            case 'd':
            case 'D':
            case KEY_RIGHT:
                return INPUT_RIGHT;
            case 'q':
            case 'Q':
            case 27:  /* Esc Key */
                return INPUT_QUIT;
        }
    return INPUT_OTHER;
}

/*
Sets the color to be used when drawing a tile.
Uses the text attribute A_BOLD to brighten a color.
*/
void set_tile_color(unsigned int tile) {
	if (has_colors()) {
		unsigned int highest_bit;
		for (highest_bit = 0; tile >= (1 << highest_bit); highest_bit++) {}
		int text_attribute = (highest_bit + 1) % 2 ? A_NORMAL : A_BOLD;
		attrset(COLOR_PAIR(highest_bit / 2) | text_attribute);
		
	}
}


unsigned int get_draw_y(unsigned int y, unsigned int frame,
						unsigned int direction) {
	unsigned int draw_y = (y * 5) + ((getmaxy(stdscr) - 22) / 5) + 2;
	switch (direction) {
		case DIRECTION_UP:
			draw_y -= frame;
			break;
		case DIRECTION_DOWN:
			draw_y += frame;
	}
	return draw_y;
}


unsigned int get_draw_x(unsigned int x, unsigned int frame,
						unsigned int direction) {
	unsigned int draw_x = (x * 10) + ((getmaxx(stdscr) - 42) / 2) + 1;
	switch (direction) {
		case DIRECTION_LEFT:
			draw_x -= frame;
			break;
		case DIRECTION_RIGHT:
			draw_x += frame;
	}
	return draw_x;
}


void draw_tile_outline(unsigned int y, unsigned int x) {
	mvaddch(y, x, ACS_ULCORNER);
	mvhline(y, x + 1, 0, 7);
	mvaddch(y, x + 8, ACS_URCORNER);
	mvvline(y + 1, x, 0, 3);
	mvvline(y + 1, x + 8, 0, 3);
	mvaddch(y + 4, x, ACS_LLCORNER);
	mvhline(y + 4, x + 1, 0, 7);
	mvaddch(y + 4, x + 8, ACS_LRCORNER);
}


void draw_tile(unsigned long tile, unsigned int frame,
                     unsigned int direction, unsigned int y, unsigned int x) {
	if (tile == 0) {
		return;
	}
	
	unsigned int draw_y = get_draw_y(y, frame, direction);
	unsigned int draw_x = get_draw_x(x, frame, direction);
	
	set_tile_color(tile);
	draw_tile_outline(draw_y, draw_x);
	
	move(draw_y + 2, draw_x + 1);
	if (tile < 100) {
		printw("%4lu  ", tile);
	}
	else {
		printw("%5lu ", tile);
	}
	
	attrset(A_NORMAL);
}


void draw_empty_board() {
	unsigned int y_offset = ((getmaxy(stdscr) - 23) / 5) + 1;
	unsigned int x_offset = ((getmaxx(stdscr) - 42) / 2) - 1;
	
	for (unsigned int y = 0; y < (y_offset + 22); y++) {
		move (y, 0);
		clrtoeol();
	}
	
	mvaddch(y_offset, x_offset, ACS_ULCORNER);
	mvhline(y_offset, x_offset + 1, 0, 41);
	mvaddch(y_offset, x_offset + 42, ACS_URCORNER);
	mvvline(y_offset + 1, x_offset, 0, 20);
	mvvline(y_offset + 1, x_offset + 42, 0, 20);
	mvaddch(y_offset + 21, x_offset, ACS_LLCORNER);
	mvhline(y_offset + 21, x_offset + 1, 0, 41);
	mvaddch(y_offset + 21, x_offset + 42, ACS_LRCORNER);
}


void draw_board(const state_t* const restrict state, unsigned int frame) {
	draw_empty_board();
	
	for (unsigned int i = 0; i < 16; i++) {
		unsigned int y = i / 4, x = i % 4;
		unsigned int tile = state->board[i / 4][i % 4];
		unsigned int working_direction = get_flag(state->move_flags, y, x) ? state->direction : DIRECTION_NONE;
		draw_tile(tile, frame, working_direction, y, x);
	}
}


void print_messages(const state_t* const restrict state) {
	if (getmaxy(stdscr) < 24) {
		return;
	}
	
	char* message;
	if (won_game(state->highest_tile) && game_over(state->board)) {
		message = "Awesome.  You win!  Press any key to exit.";
	}
	else if (game_over(state->board)) {
		message = "No more moves left.  You lose!  Press any key to exit.";
	}
	else {
		message = "Use WASD or Arrow Keys to move tiles.  Press q or Esc to quit.";
	}
    
    move((getmaxy(stdscr) - 23) / 5 + 23, 0);
    clrtobot();
    unsigned int x_offset = (getmaxx(stdscr) - strlen(message)) / 2;
	unsigned int y_offset = (getmaxy(stdscr) - 23) / 3 + 23;
    mvprintw(y_offset, x_offset, "%s", message);
}


/*
Checks if the terminal window is too small to draw the game board and tiles.
*/
unsigned char terminal_too_small() {
	if (getmaxy(stdscr) < 22 || getmaxx(stdscr) < 44) {
		clear();
		move(getmaxy(stdscr) / 2 - 1, (getmaxx(stdscr) - 32) / 2);
		printw("Error: Terminal window too small");
		move((getmaxy(stdscr) / 2), (getmaxx(stdscr) - 30) / 2);
		printw("Please resize terminal window");
		return true;
	}
	return false;
}


void print_score(const state_t* const restrict state) {
	move(0, 0);
	clrtoeol();
	mvprintw(0, ((getmaxx(stdscr) - 44) / 2 + 1), "Score: %lu", state->score);
	mvprintw(0, (getmaxx(stdscr) / 2), "Highest Tile: %lu", state->highest_tile);
}


/*
This um... updates the display every time a move is made.
*/
void render_display(const state_t* const restrict state, display_t* restrict display) {
	
	if (state->move_flags && (display->frames == 0)) {
		switch (state->direction) {
			case INPUT_UP:
			case INPUT_DOWN:
				display->total_frames = 5;
				break;
			case INPUT_LEFT:
			case INPUT_RIGHT:
				display->total_frames = 10;
		}
		display->frames = display->total_frames;
	}
	
	if (display->frames) {
		draw_board(state, display->total_frames - display->frames);
		display->frames--;
		napms(300 / display->total_frames);
	}
	else {
		draw_board(state, 0);
	}
	print_score(state);
    print_messages(state);
	refresh();
}


#endif
