/*
 * minesweeper_gui.cpp - Minesweeper GUI using Allegro 4 (like sudoku_main.cpp)
 */

#include "minesweeper_gui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <allegro.h>

/* External globals from minesweeper_main.cpp */
extern BITMAP *active_buffer;
extern int screen_dirty;
extern Minesweeper *game;

/* Global GUI state */
MinesweeperGUI minesweeper_gui;

/* File menu items */
const char *file_menu_items[] = {
    "New Game      N",
    "",  /* separator */
    "Exit          Q"
};

const int NUM_FILE_MENU_ITEMS = (sizeof(file_menu_items) / sizeof(file_menu_items[0]));

/* Game menu items */
const char *game_menu_items[] = {
    "Easy          E",
    "Medium        M",
    "Hard          H",
    "Custom        C",
    "",  /* separator */
    "Pause         P",
    "Reset         R",
};

const int NUM_GAME_MENU_ITEMS = (sizeof(game_menu_items) / sizeof(game_menu_items[0]));

/* Help menu items */
const char *help_menu_items[] = {
    "How to Play",
    "Controls",
    "About",
};

const int NUM_HELP_MENU_ITEMS = (sizeof(help_menu_items) / sizeof(help_menu_items[0]));

/* Button definitions */
Button buttons[] = {
    {BUTTON_PANEL_X, BUTTON_PANEL_Y,                                    BUTTON_WIDTH, BUTTON_HEIGHT, "Easy", 1},
    {BUTTON_PANEL_X, BUTTON_PANEL_Y + 1*(BUTTON_HEIGHT + BUTTON_SPACING), BUTTON_WIDTH, BUTTON_HEIGHT, "Medium", 2},
    {BUTTON_PANEL_X, BUTTON_PANEL_Y + 2*(BUTTON_HEIGHT + BUTTON_SPACING), BUTTON_WIDTH, BUTTON_HEIGHT, "Hard", 3},
    {BUTTON_PANEL_X, BUTTON_PANEL_Y + 3*(BUTTON_HEIGHT + BUTTON_SPACING), BUTTON_WIDTH, BUTTON_HEIGHT, "Custom", 4},
    
    {BUTTON_PANEL_X, BUTTON_PANEL_Y + 5*(BUTTON_HEIGHT + BUTTON_SPACING), BUTTON_WIDTH, BUTTON_HEIGHT, "Undo", 10},
    {BUTTON_PANEL_X, BUTTON_PANEL_Y + 6*(BUTTON_HEIGHT + BUTTON_SPACING), BUTTON_WIDTH, BUTTON_HEIGHT, "Pause", 11},
    {BUTTON_PANEL_X, BUTTON_PANEL_Y + 7*(BUTTON_HEIGHT + BUTTON_SPACING), BUTTON_WIDTH, BUTTON_HEIGHT, "Reset", 12},
    {BUTTON_PANEL_X, BUTTON_PANEL_Y + 8*(BUTTON_HEIGHT + BUTTON_SPACING), BUTTON_WIDTH, BUTTON_HEIGHT, "Exit", 13},
};

int NUM_BUTTONS = (sizeof(buttons) / sizeof(buttons[0]));

extern BITMAP *active_buffer;  /* From main program */
extern Minesweeper *game;       /* Pointer to game instance */
extern int screen_dirty;

/**
 * Initialize the minesweeper GUI
 */
void init_minesweeper_gui() {
    memset(&minesweeper_gui, 0, sizeof(MinesweeperGUI));
    
    minesweeper_gui.selected_row = 0;
    minesweeper_gui.selected_col = 0;
    minesweeper_gui.status_timer = 0;
    
    minesweeper_gui.show_file_menu = false;
    minesweeper_gui.file_menu_selected = 0;
    minesweeper_gui.show_game_menu = false;
    minesweeper_gui.game_menu_selected = 0;
    minesweeper_gui.show_help_menu = false;
    minesweeper_gui.help_menu_selected = 0;
    
    minesweeper_gui.entering_name = false;
    minesweeper_gui.player_name_length = 0;
    memset(minesweeper_gui.player_name, 0, sizeof(minesweeper_gui.player_name));
    
    sprintf(minesweeper_gui.status_message, "Welcome to Minesweeper");
    minesweeper_gui.status_timer = 120;
    
    screen_dirty = 1;
}

/**
 * Mark screen as needing redraw
 */
void mark_screen_dirty() {
    screen_dirty = 1;
}

/**
 * Display a status message
 */
void display_status(const char *message) {
    strncpy(minesweeper_gui.status_message, message, sizeof(minesweeper_gui.status_message) - 1);
    minesweeper_gui.status_message[sizeof(minesweeper_gui.status_message) - 1] = '\0';
    minesweeper_gui.status_timer = 120;
    mark_screen_dirty();
}

/**
 * Check if point is in button
 */
bool point_in_button(int x, int y, const Button *btn) {
    return x >= btn->x && x <= btn->x + btn->width &&
           y >= btn->y && y <= btn->y + btn->height;
}

/**
 * Get button ID at given coordinates
 */
int get_button_at(int x, int y) {
    int i;
    for (i = 0; i < NUM_BUTTONS; i++) {
        if (point_in_button(x, y, &buttons[i])) {
            return buttons[i].id;
        }
    }
    return -1;
}

/**
 * Draw the menu bar with File, Game, Help menus
 */
void draw_menu_bar() {
    /* Menu bar background */
    rectfill(active_buffer, 0, 0, 640, MENU_BAR_HEIGHT, COLOR_BLUE);
    
    /* Menu buttons */
    textout_ex(active_buffer, font, "File", 5, 5, COLOR_WHITE, -1);
    textout_ex(active_buffer, font, "Game", 50, 5, COLOR_WHITE, -1);
    textout_ex(active_buffer, font, "Help", 125, 5, COLOR_WHITE, -1);
    
    /* Title */
    textout_ex(active_buffer, font, "Minesweeper - Allegro Edition", 200, 5, COLOR_YELLOW, -1);
    
    /* Draw File dropdown menu if active */
    if (minesweeper_gui.show_file_menu) {
        int menu_x = 0;
        int menu_y = MENU_BAR_HEIGHT;
        int menu_w = 200;
        int item_h = 20;
        int menu_h = NUM_FILE_MENU_ITEMS * item_h;
        
        /* Ensure menu is on top - draw with solid background */
        rectfill(active_buffer, menu_x, menu_y, menu_x + menu_w, menu_y + menu_h, COLOR_BLUE);
        rect(active_buffer, menu_x, menu_y, menu_x + menu_w, menu_y + menu_h, COLOR_WHITE);
        
        /* Menu items */
        for (int i = 0; i < NUM_FILE_MENU_ITEMS; i++) {
            int item_y = menu_y + 5 + i * item_h;
            
            /* Separator */
            if (strlen(file_menu_items[i]) == 0) {
                hline(active_buffer, menu_x + 5, item_y + item_h/2, menu_x + menu_w - 5, COLOR_DARK_GRAY);
                continue;
            }
            
            /* Highlight selected */
            if (i == minesweeper_gui.file_menu_selected) {
                rectfill(active_buffer, menu_x + 2, item_y + 2, 
                        menu_x + menu_w - 2, item_y + item_h - 2, COLOR_CYAN);
            }
            
            /* Draw menu item text */
            int text_color = (i == minesweeper_gui.file_menu_selected) ? COLOR_BLACK : COLOR_WHITE;
            textout_ex(active_buffer, font, file_menu_items[i], menu_x + 10, item_y + 3, text_color, -1);
        }
    }
    
    /* Draw Game dropdown menu if active */
    if (minesweeper_gui.show_game_menu) {
        int menu_x = 50;
        int menu_y = MENU_BAR_HEIGHT;
        int menu_w = 200;
        int item_h = 20;
        int menu_h = NUM_GAME_MENU_ITEMS * item_h;
        
        /* Ensure menu is on top - draw with solid background */
        rectfill(active_buffer, menu_x, menu_y, menu_x + menu_w, menu_y + menu_h, COLOR_BLUE);
        rect(active_buffer, menu_x, menu_y, menu_x + menu_w, menu_y + menu_h, COLOR_WHITE);
        
        /* Menu items */
        for (int i = 0; i < NUM_GAME_MENU_ITEMS; i++) {
            int item_y = menu_y + 5 + i * item_h;
            
            /* Separator */
            if (strlen(game_menu_items[i]) == 0) {
                hline(active_buffer, menu_x + 5, item_y + item_h/2, menu_x + menu_w - 5, COLOR_DARK_GRAY);
                continue;
            }
            
            /* Highlight selected */
            if (i == minesweeper_gui.game_menu_selected) {
                rectfill(active_buffer, menu_x + 2, item_y + 2, 
                        menu_x + menu_w - 2, item_y + item_h - 2, COLOR_CYAN);
            }
            
            /* Draw menu item text */
            int text_color = (i == minesweeper_gui.game_menu_selected) ? COLOR_BLACK : COLOR_WHITE;
            textout_ex(active_buffer, font, game_menu_items[i], menu_x + 10, item_y + 3, text_color, -1);
        }
    }
    
    /* Draw Help dropdown menu if active */
    if (minesweeper_gui.show_help_menu) {
        int menu_x = 125;
        int menu_y = MENU_BAR_HEIGHT;
        int menu_w = 200;
        int item_h = 20;
        int menu_h = NUM_HELP_MENU_ITEMS * item_h + 10;
        
        /* Ensure menu is on top - draw with solid background */
        rectfill(active_buffer, menu_x, menu_y, menu_x + menu_w, menu_y + menu_h + 50, COLOR_BLUE);
        rect(active_buffer, menu_x, menu_y, menu_x + menu_w, menu_y + menu_h, COLOR_WHITE);
        
        /* Menu items */
        for (int i = 0; i < NUM_HELP_MENU_ITEMS; i++) {
            int item_y = menu_y + 5 + i * item_h;
            
            /* Highlight selected */
            if (i == minesweeper_gui.help_menu_selected) {
                rectfill(active_buffer, menu_x + 2, item_y + 2, 
                        menu_x + menu_w - 2, item_y + item_h - 2, COLOR_CYAN);
            }
            
            /* Draw menu item text */
            int text_color = (i == minesweeper_gui.help_menu_selected) ? COLOR_BLACK : COLOR_WHITE;
            textout_ex(active_buffer, font, help_menu_items[i], menu_x + 10, item_y + 3, text_color, -1);
        }
    }
}

/**
 * Draw the game board
 */
void draw_game_board() {
    if (!game) return;
    
    /* Only draw board during gameplay */
    if (game->state != GameState::PLAYING) {
        return;
    }
    
    /* Board background */
    rectfill(active_buffer, BOARD_START_X - 5, BOARD_START_Y - 5, 
             BOARD_START_X + (game->width * CELL_SIZE) + 5, 
             BOARD_START_Y + (game->height * CELL_SIZE) + 5, COLOR_LIGHT_GRAY);
    
    /* Border */
    rect(active_buffer, BOARD_START_X - 5, BOARD_START_Y - 5, 
         BOARD_START_X + (game->width * CELL_SIZE) + 5, 
         BOARD_START_Y + (game->height * CELL_SIZE) + 5, COLOR_BLACK);
    
    /* Draw each cell */
    for (int y = 0; y < game->height; y++) {
        for (int x = 0; x < game->width; x++) {
            int cell_x = BOARD_START_X + x * CELL_SIZE;
            int cell_y = BOARD_START_Y + y * CELL_SIZE;
            
            /* Draw cell background */
            int cell_color = COLOR_LIGHT_GRAY;
            
            /* Selection highlight */
            if (x == minesweeper_gui.selected_col && y == minesweeper_gui.selected_row) {
                cell_color = COLOR_YELLOW;
            }
            
            rectfill(active_buffer, cell_x, cell_y, cell_x + CELL_SIZE, cell_y + CELL_SIZE, cell_color);
            
            /* Cell border */
            rect(active_buffer, cell_x, cell_y, cell_x + CELL_SIZE, cell_y + CELL_SIZE, COLOR_BLACK);
            
            /* Draw cell content */
            if (game->revealed[y][x]) {
                if (game->minefield[y][x]) {
                    /* Mine - draw red circle */
                    circlefill(active_buffer, cell_x + CELL_SIZE/2, cell_y + CELL_SIZE/2, 
                              CELL_SIZE/4, COLOR_RED);
                } else {
                    /* Safe cell - show mine count */
                    int count = game->countAdjacentMines(x, y);
                    if (count > 0) {
                        char count_str[4];
                        sprintf(count_str, "%d", count);
                        textout_ex(active_buffer, font, count_str, 
                                  cell_x + CELL_SIZE/3, cell_y + CELL_SIZE/3, 
                                  COLOR_BLACK, -1);
                    }
                }
            } else if (game->flagged[y][x]) {
                /* Flagged cell - draw flag */
                textout_ex(active_buffer, font, "F", 
                          cell_x + CELL_SIZE/3, cell_y + CELL_SIZE/3, 
                          COLOR_RED, -1);
            } else {
                /* Unrevealed cell - draw slightly raised */
                rectfill(active_buffer, cell_x + 2, cell_y + 2, 
                        cell_x + CELL_SIZE - 2, cell_y + CELL_SIZE - 2, COLOR_DARK_GRAY);
            }
        }
    }
}

/**
 * Draw the button panel
 */
void draw_button_panel() {
    /* Only show buttons during gameplay */
    if (!game || game->state != GameState::PLAYING) {
        return;
    }
    
    /* Panel background */
    rectfill(active_buffer, BUTTON_PANEL_X - 10, BUTTON_PANEL_Y - 10, 
             BUTTON_PANEL_X + BUTTON_WIDTH + 10, 
             BUTTON_PANEL_Y + (NUM_BUTTONS + 1) * (BUTTON_HEIGHT + BUTTON_SPACING) + 10, 
             COLOR_LIGHT_GRAY);
    
    /* Border */
    rect(active_buffer, BUTTON_PANEL_X - 10, BUTTON_PANEL_Y - 10, 
         BUTTON_PANEL_X + BUTTON_WIDTH + 10, 
         BUTTON_PANEL_Y + (NUM_BUTTONS + 1) * (BUTTON_HEIGHT + BUTTON_SPACING) + 10, 
         COLOR_BLACK);
    
    /* Draw buttons */
    for (int i = 0; i < NUM_BUTTONS; i++) {
        Button *btn = &buttons[i];
        
        /* Button background */
        rectfill(active_buffer, btn->x, btn->y, btn->x + btn->width, btn->y + btn->height, COLOR_BLUE);
        
        /* Button border */
        rect(active_buffer, btn->x, btn->y, btn->x + btn->width, btn->y + btn->height, COLOR_BLACK);
        
        /* Button text */
        textout_ex(active_buffer, font, btn->label, btn->x + 5, btn->y + 5, COLOR_WHITE, -1);
    }
}

/**
 * Draw name input dialog
 */
void draw_name_input_dialog() {
    if (!minesweeper_gui.entering_name) {
        return;
    }
    
    int box_x = 150;
    int box_y = 150;
    int box_w = 340;
    int box_h = 120;
    
    /* Semi-transparent overlay */
    rectfill(active_buffer, 0, 0, 640, 480, COLOR_BLACK);
    
    /* Dialog box */
    rectfill(active_buffer, box_x, box_y, box_x + box_w, box_y + box_h, COLOR_BLUE);
    rect(active_buffer, box_x, box_y, box_x + box_w, box_y + box_h, COLOR_WHITE);
    
    /* Title */
    textout_ex(active_buffer, font, "New High Score!", box_x + 100, box_y + 15, COLOR_YELLOW, -1);
    
    /* Input prompt */
    textout_ex(active_buffer, font, "Enter your name:", box_x + 20, box_y + 45, COLOR_WHITE, -1);
    
    /* Input field background */
    rectfill(active_buffer, box_x + 20, box_y + 65, box_x + box_w - 20, box_y + 85, COLOR_WHITE);
    
    /* Input text */
    textout_ex(active_buffer, font, minesweeper_gui.player_name, box_x + 25, box_y + 70, COLOR_BLACK, -1);
    
    /* Cursor */
    if ((clock() / 250) % 2) {  /* Blinking cursor */
        int cursor_x = box_x + 25 + minesweeper_gui.player_name_length * 8;
        vline(active_buffer, cursor_x, box_y + 65, box_y + 85, COLOR_BLACK);
    }
    
    /* Instructions */
    textout_ex(active_buffer, font, "Press Enter to submit (ESC to cancel)", 
              box_x + 35, box_y + 95, COLOR_LIGHT_GRAY, -1);
}

/**
 * Draw the complete minesweeper screen
 */
void draw_minesweeper_screen() {
    clear_to_color(active_buffer, COLOR_WHITE);
    
    draw_game_board();
    draw_button_panel();
    draw_name_input_dialog();
    
    /* Status bar */
    rectfill(active_buffer, 0, STATUS_BAR_Y, 640, STATUS_BAR_Y + 30, COLOR_LIGHT_GRAY);
    
    if (minesweeper_gui.status_timer > 0) {
        textout_ex(active_buffer, font, minesweeper_gui.status_message, 10, STATUS_BAR_Y + 8, COLOR_BLACK, -1);
        minesweeper_gui.status_timer--;
    }
    
    /* Timer display */
    if (game && game->isRunning()) {
        char timer_str[20];
        sprintf(timer_str, "Time: %s", game->getTimeString().c_str());
        textout_ex(active_buffer, font, timer_str, TIMER_DISPLAY_X, TIMER_DISPLAY_Y, COLOR_BLACK, -1);
    }
    
    /* Mine counter */
    if (game) {
        int flags = 0;
        for (int y = 0; y < game->height; y++) {
            for (int x = 0; x < game->width; x++) {
                if (game->flagged[y][x]) flags++;
            }
        }
        char mine_str[20];
        sprintf(mine_str, "Mines: %d/%d", flags, game->mines);
        textout_ex(active_buffer, font, mine_str, TIMER_DISPLAY_X + 200, TIMER_DISPLAY_Y, COLOR_BLACK, -1);
    }
    
    /* Draw menu bar LAST so it appears on top */
    draw_menu_bar();
}

/**
 * Handle keyboard input
 */
void handle_minesweeper_input(int key) {
    if (minesweeper_gui.entering_name) {
        /* Name input mode */
        if (key == KEY_BACKSPACE) {
            if (minesweeper_gui.player_name_length > 0) {
                minesweeper_gui.player_name[--minesweeper_gui.player_name_length] = '\0';
                mark_screen_dirty();
            }
        } else if (key == KEY_ENTER) {
            if (minesweeper_gui.player_name_length > 0) {
                /* Copy name to game's playerName, then save */
                game->playerName = minesweeper_gui.player_name;
                game->saveHighscore();
                minesweeper_gui.entering_name = false;
                display_status("High score saved!");
                mark_screen_dirty();
            }
        } else if (isprint(key) && minesweeper_gui.player_name_length < 20) {
            minesweeper_gui.player_name[minesweeper_gui.player_name_length++] = (char)key;
            minesweeper_gui.player_name[minesweeper_gui.player_name_length] = '\0';
            mark_screen_dirty();
        }
    } else {
        /* Game mode input */
        if (key == KEY_UP) {
            if (minesweeper_gui.selected_row > 0) {
                minesweeper_gui.selected_row--;
                mark_screen_dirty();
            }
        } else if (key == KEY_DOWN) {
            if (minesweeper_gui.selected_row < game->height - 1) {
                minesweeper_gui.selected_row++;
                mark_screen_dirty();
            }
        } else if (key == KEY_LEFT) {
            if (minesweeper_gui.selected_col > 0) {
                minesweeper_gui.selected_col--;
                mark_screen_dirty();
            }
        } else if (key == KEY_RIGHT) {
            if (minesweeper_gui.selected_col < game->width - 1) {
                minesweeper_gui.selected_col++;
                mark_screen_dirty();
            }
        } else if (key == ' ' || key == KEY_ENTER) {
            /* Reveal cell */
            game->reveal(minesweeper_gui.selected_col, minesweeper_gui.selected_row);
            mark_screen_dirty();
        } else if (key == 'f' || key == 'F') {
            /* Toggle flag */
            game->toggleFlag(minesweeper_gui.selected_col, minesweeper_gui.selected_row);
            mark_screen_dirty();
        } else if (key == 27) {  /* ESC to close menus */
            minesweeper_gui.show_file_menu = false;
            minesweeper_gui.show_game_menu = false;
            minesweeper_gui.show_help_menu = false;
            mark_screen_dirty();
        }
    }
}
