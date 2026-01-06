/*
 * minesweeper_gui.h - Minesweeper GUI using Allegro 4 (like sudoku_main.cpp)
 */

#ifndef MINESWEEPER_GUI_H
#define MINESWEEPER_GUI_H

#include <allegro.h>
#include "minesweeper.h"

/* UI Layout */
#define MENU_BAR_HEIGHT 25
#define BOARD_START_X 50
#define BOARD_START_Y (MENU_BAR_HEIGHT + 30)
#define CELL_SIZE 30
#define MAX_BOARD_WIDTH 30
#define MAX_BOARD_HEIGHT 30

#define BUTTON_PANEL_X (BOARD_START_X + MAX_BOARD_WIDTH * CELL_SIZE + 30)
#define BUTTON_PANEL_Y (BOARD_START_Y + 10)
#define BUTTON_WIDTH 110
#define BUTTON_HEIGHT 22
#define BUTTON_SPACING 5

#define STATUS_BAR_Y (BOARD_START_Y + MAX_BOARD_HEIGHT * CELL_SIZE + 20)
#define TIMER_DISPLAY_X (BOARD_START_X + 10)
#define TIMER_DISPLAY_Y (STATUS_BAR_Y + 5)

/* Colors - Allegro 4 palette colors */
#define COLOR_BLACK 0
#define COLOR_WHITE 15
#define COLOR_LIGHT_GRAY 7
#define COLOR_DARK_GRAY 8
#define COLOR_BLUE 1
#define COLOR_RED 4
#define COLOR_GREEN 2
#define COLOR_YELLOW 14
#define COLOR_CYAN 3

/* Menu items */
extern const char *file_menu_items[];
extern const int NUM_FILE_MENU_ITEMS;

extern const char *help_menu_items[];
extern const int NUM_HELP_MENU_ITEMS;

/* Button definitions */
struct Button {
    int x;
    int y;
    int width;
    int height;
    const char *label;
    int id;
};

extern Button buttons[];
extern int NUM_BUTTONS;

/* Minesweeper GUI State */
struct MinesweeperGUI {
    int selected_row;
    int selected_col;
    
    /* UI State */
    char status_message[256];
    int status_timer;
    
    /* Menu state */
    bool show_file_menu;
    int file_menu_selected;
    
    bool show_help_menu;
    int help_menu_selected;
    
    /* Name entry state */
    bool entering_name;
    char player_name[21];
    int player_name_length;
};

extern MinesweeperGUI minesweeper_gui;

/* Initialization */
void init_minesweeper_gui();

/* Drawing functions */
void draw_menu_bar();
void draw_game_board();
void draw_button_panel();
void draw_name_input_dialog();
void draw_minesweeper_screen();

/* Input handling */
void handle_minesweeper_input(int key);

/* Utility functions */
void display_status(const char *message);
void mark_screen_dirty();
bool point_in_button(int x, int y, const Button *btn);
int get_button_at(int x, int y);

#endif /* MINESWEEPER_GUI_H */
