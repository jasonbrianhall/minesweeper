// ncurses.h - Minimal header for MS-DOS/DJGPP implementation
#ifndef _NCURSES_H
#define _NCURSES_H

#include <stdarg.h>

// Define screen dimensions
#define COLS 80
#define LINES 25

// Define color constants
#define COLOR_BLACK     0
#define COLOR_BLUE      1
#define COLOR_GREEN     2
#define COLOR_CYAN      3
#define COLOR_RED       4
#define COLOR_MAGENTA   5
#define COLOR_YELLOW    6
#define COLOR_WHITE     7

// Define attribute macros
#define A_NORMAL        0x0000
#define A_STANDOUT      0x0100
#define A_UNDERLINE     0x0200
#define A_REVERSE       0x0400
#define A_BLINK         0x0800
#define A_DIM           0x1000
#define A_BOLD          0x2000
#define A_PROTECT       0x4000
#define A_INVIS         0x8000

// Define key codes
#define KEY_UP          0x103
#define KEY_DOWN        0x102
#define KEY_LEFT        0x104
#define KEY_RIGHT       0x105
#define KEY_HOME        0x106
#define KEY_BACKSPACE   0x107
#define KEY_F0          0x108
#define KEY_ENTER       0x157
#define KEY_NPAGE       0x152
#define KEY_PPAGE       0x153
#define KEY_END         0x168

// Define standard return values
#define OK              0
#define ERR             -1

// Define window structure
typedef struct _win_st {
    int startx, starty;
    int width, height;
    int curx, cury;
    int attrs;
} WINDOW;

// Declare external stdscr
extern WINDOW* stdscr;

// Function declarations (use ncurses_ prefix to avoid conflicts)
WINDOW* initscr(void);
int endwin(void);
int refresh(void);
int keypad(WINDOW*, int);
void raw(void);
void noecho(void);
void start_color(void);
void init_pair(short, short, short);
int COLOR_PAIR(int);
int attron(int);
int attroff(int);
void clear(void);
void move(int, int);
int mvprintw(int, int, const char*, ...);
int addch(int);
void clrtoeol(void);
int curs_set(int);
int ncurses_getch(void); // Will conflict with conio.h, must be handled in implementation

#endif /* _NCURSES_H */
