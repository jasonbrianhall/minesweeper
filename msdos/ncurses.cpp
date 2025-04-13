#include "ncurses.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

// Include conio.h last to avoid conflicts
#include <conio.h>

// Default stdscr window
static WINDOW default_stdscr = {0, 0, COLS, LINES, 0, 0, A_NORMAL};
WINDOW* stdscr = &default_stdscr;

// Color pair storage
#define COLOR_PAIRS 64
static int color_pairs[COLOR_PAIRS][2] = {{0}};

// Screen management
static unsigned short* screen_buffer = NULL;
static int cursor_visible = 1;
static int current_attr = 7; // Default white text on black background

// Converts ncurses attributes to conio attributes
static int attr_to_conio(int attrs) {
    int result = 7; // Default white on black
    
    // Extract color pair if present
    int pair = (attrs & 0xFF);
    if (pair > 0 && pair < COLOR_PAIRS) {
        result = (color_pairs[pair][1] << 4) | color_pairs[pair][0];
    }
    
    // Apply attributes
    if (attrs & A_BOLD) {
        result |= 0x08; // High intensity
    }
    if (attrs & A_BLINK) {
        result |= 0x80; // Blink
    }
    if (attrs & A_REVERSE) {
        // Swap foreground and background
        int fg = result & 0x0F;
        int bg = (result & 0xF0) >> 4;
        result = (fg << 4) | bg;
    }
    
    return result;
}

// Initialize the screen
WINDOW* initscr(void) {
    textmode(C80);
    clrscr();
    // Allocate screen buffer
    screen_buffer = (unsigned short*)malloc(COLS * LINES * sizeof(unsigned short));
    if (screen_buffer) {
        memset(screen_buffer, 0, COLS * LINES * sizeof(unsigned short));
    }
    return stdscr;
}

// End ncurses mode
int endwin(void) {
    textmode(C80);
    if (screen_buffer) {
        free(screen_buffer);
        screen_buffer = NULL;
    }
    return OK;
}

// Turn on keypad for the given window
int keypad(WINDOW* win, int bf) {
    // Not needed in conio - keys are always available
    return OK;
}

// Set terminal to raw mode
void raw(void) {
    // Not needed in conio
}

// Turn off echo
void noecho(void) {
    // Not needed in conio - characters are not echoed by default in getch()
}

// Clear the screen
void clear(void) {
    clrscr();
    if (screen_buffer) {
        memset(screen_buffer, 0, COLS * LINES * sizeof(unsigned short));
    }
    stdscr->curx = 0;
    stdscr->cury = 0;
}

// Refresh the screen to match internal buffer
int refresh(void) {
    // conio updates the screen immediately, nothing to do here
    return OK;
}

// Move the cursor
void move(int y, int x) {
    stdscr->cury = y;
    stdscr->curx = x;
    gotoxy(x + 1, y + 1); // CONIO uses 1-based coordinates
}

// Move and print formatted output
int mvprintw(int y, int x, const char* fmt, ...) {
    va_list args;
    char buffer[1024];
    
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    
    move(y, x);
    textcolor(current_attr & 0x0F);
    textbackground((current_attr >> 4) & 0x07);
    cputs(buffer);
    
    // Update cursor position
    stdscr->curx += strlen(buffer);
    
    return OK;
}

// Add a character to the screen
int addch(int ch) {
    // Calculate color based on current_attr
    textcolor(current_attr & 0x0F);
    textbackground((current_attr >> 4) & 0x07);
    
    putch(ch);
    stdscr->curx++;
    return OK;
}

// Clear to end of line
void clrtoeol(void) {
    int i;
    int x = stdscr->curx;
    int y = stdscr->cury;
    
    gotoxy(x + 1, y + 1);
    textcolor(7); // White
    textbackground(0); // Black
    
    for (i = x; i < COLS; i++) {
        putch(' ');
    }
    
    gotoxy(x + 1, y + 1);
}

// Turn on an attribute
int attron(int attrs) {
    // Store the original ncurses attribute in the window structure
    stdscr->attrs |= attrs;
    
    // Calculate the corresponding conio attribute
    current_attr = attr_to_conio(stdscr->attrs);
    
    return OK;
}

// Turn off an attribute
int attroff(int attrs) {
    // Remove the specified attribute from the window's attributes
    stdscr->attrs &= ~attrs;
    
    // Recalculate the conio attribute based on the updated window attributes
    current_attr = attr_to_conio(stdscr->attrs);
    
    return OK;
}

// Set cursor visibility (0=invisible, 1=normal, 2=very visible)
int curs_set(int visibility) {
    int old_vis = cursor_visible;
    cursor_visible = visibility;
    
    // Using direct BIOS access to change cursor visibility
    union REGS regs;
    regs.h.ah = 1;
    
    if (visibility == 0) {
        // Hide cursor
        regs.h.ch = 0x20; // Set bit 5 to hide cursor
        regs.h.cl = 0;
    } else {
        // Show cursor (normal or high visibility)
        regs.h.ch = (visibility == 2) ? 0 : 6; // Start scan line
        regs.h.cl = (visibility == 2) ? 14 : 7; // End scan line (bigger difference = more visible)
    }
    
    int86(0x10, &regs, &regs);
    return old_vis;
}

// Start using colors
void start_color(void) {
    // Initialize default color pairs
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    init_pair(8, COLOR_RED, COLOR_BLACK);     // mine
    init_pair(9, COLOR_YELLOW, COLOR_BLACK);  // flag
    init_pair(10, COLOR_WHITE, COLOR_BLUE);   // title
}

// Initialize a color pair
void init_pair(short pair, short fg, short bg) {
    if (pair > 0 && pair < COLOR_PAIRS) {
        color_pairs[pair][0] = fg;
        color_pairs[pair][1] = bg;
    }
}

// Get a color pair
int COLOR_PAIR(int pair) {
    return pair;
}

// Get a character from the keyboard - this handles the conflict with conio.h
int ncurses_getch(void) {
    int ch = ::getch();
    if (ch == 0 || ch == 0xE0) {  // Extended key
        ch = ::getch();
        switch (ch) {
            case 0x48: return KEY_UP;
            case 0x50: return KEY_DOWN;
            case 0x4B: return KEY_LEFT;
            case 0x4D: return KEY_RIGHT;
            case 0x47: return KEY_HOME;
            case 0x4F: return KEY_END;
            case 0x49: return KEY_PPAGE;
            case 0x51: return KEY_NPAGE;
            case 0x1C: return KEY_ENTER;
            default: return KEY_F0 + ch - 0x3B; // Function keys
        }
    }
    if (ch == 0x7F || ch == 0x08) {
        return KEY_BACKSPACE;
    }
    return ch;
}
