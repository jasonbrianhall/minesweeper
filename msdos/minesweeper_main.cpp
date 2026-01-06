/*
 * minesweeper_main.cpp - Minesweeper Allegro 4 GUI Main Entry Point
 * Handles all window, input, and rendering
 */

#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minesweeper.h"
#include "minesweeper_gui.h"

/* Extern declarations for menu items */
extern const char *file_menu_items[];
extern const int NUM_FILE_MENU_ITEMS;
extern const char *help_menu_items[];
extern const int NUM_HELP_MENU_ITEMS;

/* Platform detection */
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    #define LINUX_BUILD 1
#else
    #define DOS_BUILD 1
#endif

/* Double buffering setup */
#define NUM_BUFFERS 2
BITMAP *buffers[NUM_BUFFERS];
int current_buffer = 0;
BITMAP *active_buffer = NULL;

/* Game instance - accessible to GUI */
Minesweeper *game = NULL;

/* Screen dirty tracking */
int screen_dirty = 1;

/* Global mouse button state to prevent multiple clicks */
static int prev_mouse_b = 0;

/* Window close button flag */
static volatile int want_to_quit = 0;

/**
 * Callback for window close button
 */
void close_button_callback() {
    want_to_quit = 1;
}

/* Forward declarations */
void handle_file_menu_click(int item_index);

/* Allegro keyboard constants */
#ifndef KEY_UP
    #define KEY_UP 0x48
#endif
#ifndef KEY_DOWN
    #define KEY_DOWN 0x50
#endif
#ifndef KEY_LEFT
    #define KEY_LEFT 0x4B
#endif
#ifndef KEY_RIGHT
    #define KEY_RIGHT 0x4D
#endif
#ifndef KEY_ESC
    #define KEY_ESC 0x01
#endif
#ifndef KEY_ENTER
    #define KEY_ENTER 0x1C
#endif
#ifndef KEY_BACKSPACE
    #define KEY_BACKSPACE 0x0E
#endif

/**
 * Initialize double buffering
 */
void init_double_buffers() {
    for (int i = 0; i < NUM_BUFFERS; i++) {
        buffers[i] = create_bitmap(1024, 768);
        if (!buffers[i]) {
            allegro_exit();
            fprintf(stderr, "Failed to allocate buffer %d\n", i);
            exit(1);
        }
        /* Initialize buffers to white */
        clear_to_color(buffers[i], COLOR_WHITE);
    }
    current_buffer = 0;
    active_buffer = buffers[0];
}

/**
 * Swap buffers and get the next one to draw into
 * Only blits to screen if screen_dirty is true
 */
BITMAP* get_next_buffer_and_swap() {
    /* Only update screen if it's marked as dirty */
    if (screen_dirty) {
        vsync();  /* Wait for vertical sync */
        blit(active_buffer, screen, 0, 0, 0, 0, 1024, 768);
        screen_dirty = 0;  /* Mark screen as clean after update */
    }
    
    /* Swap to next buffer for drawing */
    current_buffer = (current_buffer + 1) % NUM_BUFFERS;
    active_buffer = buffers[current_buffer];
    
    return active_buffer;
}

/**
 * Handle File menu item clicks
 */
void handle_file_menu_click(int item_index) {
    if (!game) return;
    
    switch (item_index) {
        case 0:  /* New Game */
            game->currentSeed = -1;  /* Generate new seed */
            game->setDifficulty(Difficulty::EASY);
            game->reset();
            game->state = GameState::PLAYING;
            display_status("New Game - Easy Mode");
            mark_screen_dirty();
            break;
        case 2:  /* Easy */
            game->currentSeed = -1;  /* Generate new seed */
            game->setDifficulty(Difficulty::EASY);
            game->reset();
            game->state = GameState::PLAYING;
            display_status("Easy Mode - Left click to reveal, Right click to flag");
            mark_screen_dirty();
            break;
        case 3:  /* Medium */
            game->currentSeed = -1;  /* Generate new seed */
            game->setDifficulty(Difficulty::MEDIUM);
            game->reset();
            game->state = GameState::PLAYING;
            display_status("Medium Mode - Left click to reveal, Right click to flag");
            mark_screen_dirty();
            break;
        case 4:  /* Hard */
            game->currentSeed = -1;  /* Generate new seed */
            game->setDifficulty(Difficulty::HARD);
            game->reset();
            game->state = GameState::PLAYING;
            display_status("Hard Mode - Left click to reveal, Right click to flag");
            mark_screen_dirty();
            break;
        case 6:  /* Reset */
            /* Keep currentSeed the same - replay same board */
            game->reset();
            game->state = GameState::PLAYING;
            display_status("Game reset - Left click to reveal, Right click to flag");
            mark_screen_dirty();
            break;
        case 7:  /* High Scores */
            game->state = GameState::HIGHSCORES;
            display_status("Viewing high scores");
            mark_screen_dirty();
            break;
        case 9:  /* Exit */
            allegro_exit();
            exit(0);
            break;
    }
}

/**
 * Handle keyboard input and game state transitions
 */
void process_input(Minesweeper *game, bool &running) {
    if (!keypressed()) return;
    
    int key = readkey();
    int ascii = key & 0xFF;
    
    /* If entering name, skip all shortcuts and go directly to name input */
    if (game->state == GameState::ENTER_NAME) {
        handle_minesweeper_input(ascii);
        mark_screen_dirty();
        return;
    }
    
    /* Global quit */
    if (ascii == 'q' || ascii == 'Q') {
        running = false;
        return;
    }
    
    /* New Game shortcut */
    if (ascii == 'n' || ascii == 'N') {
        if (game->state == GameState::PLAYING || game->state == GameState::GAME_OVER) {
            game->currentSeed = -1;  /* Generate new seed */
            game->setDifficulty(Difficulty::EASY);
            game->reset();
            game->state = GameState::PLAYING;
            display_status("New Game - Easy Mode");
            mark_screen_dirty();
        }
        return;
    }
    
    /* Menu bar keyboard shortcut: A to toggle File menu */
    if (ascii == 'a' || ascii == 'A') {
        minesweeper_gui.show_file_menu = !minesweeper_gui.show_file_menu;
        mark_screen_dirty();
        return;
    }
    
    /* Game state input */
    switch (game->state) {
        case GameState::MENU:
            /* Menu input */
            if (ascii == 'e' || ascii == 'E' || ascii == '1') {
                game->currentSeed = -1;  /* Generate new seed */
                game->setDifficulty(Difficulty::EASY);
                game->reset();
                game->state = GameState::PLAYING;
                mark_screen_dirty();
            } else if (ascii == 'm' || ascii == 'M' || ascii == '2') {
                game->currentSeed = -1;  /* Generate new seed */
                game->setDifficulty(Difficulty::MEDIUM);
                game->reset();
                game->state = GameState::PLAYING;
                mark_screen_dirty();
            } else if (ascii == 'h' || ascii == 'H' || ascii == '3') {
                game->currentSeed = -1;  /* Generate new seed */
                game->setDifficulty(Difficulty::HARD);
                game->reset();
                game->state = GameState::PLAYING;
                mark_screen_dirty();
            } else if (ascii == 's' || ascii == 'S' || ascii == 'k' || ascii == 'K') {
                game->state = GameState::HIGHSCORES;
                mark_screen_dirty();
            }
            display_status("Menu");
            break;
            
        case GameState::PLAYING:
            /* Check for menu/difficulty shortcuts first */
            if (ascii == 'n' || ascii == 'N') {
                /* New Game */
                game->currentSeed = -1;  /* Generate new seed */
                game->setDifficulty(Difficulty::EASY);
                game->reset();
                game->state = GameState::PLAYING;
                display_status("New Game - Easy Mode");
                mark_screen_dirty();
            } else if (ascii == '1') {
                /* Easy */
                game->currentSeed = -1;  /* Generate new seed */
                game->setDifficulty(Difficulty::EASY);
                game->reset();
                game->state = GameState::PLAYING;
                display_status("Easy Mode");
                mark_screen_dirty();
            } else if (ascii == '2') {
                /* Medium */
                game->currentSeed = -1;  /* Generate new seed */
                game->setDifficulty(Difficulty::MEDIUM);
                game->reset();
                game->state = GameState::PLAYING;
                display_status("Medium Mode");
                mark_screen_dirty();
            } else if (ascii == '3') {
                /* Hard */
                game->currentSeed = -1;  /* Generate new seed */
                game->setDifficulty(Difficulty::HARD);
                game->reset();
                game->state = GameState::PLAYING;
                display_status("Hard Mode");
                mark_screen_dirty();
            } else if (ascii == 'r' || ascii == 'R') {
                /* Reset */
                game->reset();
                game->state = GameState::PLAYING;
                display_status("Game reset");
                mark_screen_dirty();
            } else if (ascii == 'k' || ascii == 'K') {
                /* High Scores */
                game->state = GameState::HIGHSCORES;
                mark_screen_dirty();
            } else {
                /* Regular gameplay input */
                handle_minesweeper_input(key);
                mark_screen_dirty();
            }
            
            /* Game state checks */
            if (game->won) {
                game->state = GameState::GAME_OVER;
                display_status("You won! Press any key...");
                mark_screen_dirty();
            } else if (game->gameOver) {
                game->state = GameState::GAME_OVER;
                display_status("Game Over! Press any key...");
                mark_screen_dirty();
            }
            break;
            
        case GameState::GAME_OVER:
            /* Allow menu interaction in GAME_OVER state */
            if (ascii == 'a' || ascii == 'A') {
                /* Toggle File menu */
                minesweeper_gui.show_file_menu = !minesweeper_gui.show_file_menu;
                mark_screen_dirty();
            } else if (ascii == 'n' || ascii == 'N') {
                /* New Game */
                game->currentSeed = -1;  /* Generate new seed */
                game->setDifficulty(Difficulty::EASY);
                game->reset();
                game->state = GameState::PLAYING;
                display_status("New Game - Easy Mode");
                mark_screen_dirty();
            } else if (ascii == '1') {
                /* Easy */
                game->currentSeed = -1;  /* Generate new seed */
                game->setDifficulty(Difficulty::EASY);
                game->reset();
                game->state = GameState::PLAYING;
                display_status("Easy Mode");
                mark_screen_dirty();
            } else if (ascii == '2') {
                /* Medium */
                game->currentSeed = -1;  /* Generate new seed */
                game->setDifficulty(Difficulty::MEDIUM);
                game->reset();
                game->state = GameState::PLAYING;
                display_status("Medium Mode");
                mark_screen_dirty();
            } else if (ascii == '3') {
                /* Hard */
                game->currentSeed = -1;  /* Generate new seed */
                game->setDifficulty(Difficulty::HARD);
                game->reset();
                game->state = GameState::PLAYING;
                display_status("Hard Mode");
                mark_screen_dirty();
            } else if (ascii == 'r' || ascii == 'R') {
                /* Reset */
                /* Keep currentSeed the same - replay same board */
                game->reset();
                game->state = GameState::PLAYING;
                display_status("Game reset");
                mark_screen_dirty();
            } else if (ascii == 'k' || ascii == 'K') {
                /* High Scores */
                game->state = GameState::HIGHSCORES;
                mark_screen_dirty();
            } else {
                /* Any other key: go back to playing (old behavior) */
                if (!game->gameOver && game->won && game->isHighScore(game->timer.getElapsedSeconds())) {
                    game->state = GameState::ENTER_NAME;
                    minesweeper_gui.entering_name = true;
                    minesweeper_gui.player_name_length = 0;
                    memset(minesweeper_gui.player_name, 0, sizeof(minesweeper_gui.player_name));
                } else {
                    /* Just reset the game state flags, go back to playing. Board stays visible. */
                    game->gameOver = false;
                    game->won = false;
                    game->firstMove = true;
                    game->state = GameState::PLAYING;
                    display_status("Minesweeper - Left click to reveal, Right click to flag");
                }
                mark_screen_dirty();
            }
            break;
            
        case GameState::HIGHSCORES:
            /* Return to playing */
            game->currentSeed = -1;  /* Generate new seed */
            game->state = GameState::PLAYING;
            game->reset();
            display_status("Press N for new game or use File menu");
            mark_screen_dirty();
            break;
            
        case GameState::HELP:
            /* Return to playing */
            game->currentSeed = -1;  /* Generate new seed */
            game->state = GameState::PLAYING;
            game->reset();
            display_status("Press N for new game or use File menu");
            mark_screen_dirty();
            break;
    }
}

/**
 * Main game loop
 */
int main() {
    /* Initialize Allegro */
    allegro_init();
    install_keyboard();
    install_timer();
    install_mouse();  /* Install mouse driver */
    
    /* Set up window close button callback */
    set_close_button_callback(close_button_callback);
    
    /* Set graphics mode - WINDOWED, not fullscreen */
    set_color_depth(8);
    if (set_gfx_mode(GFX_SAFE, 1024, 768, 0, 0) != 0) {
        allegro_exit();
        fprintf(stderr, "Failed to set graphics mode\n");
        return 1;
    }
    
    /* Initialize double buffering */
    init_double_buffers();
    
    /* Create and initialize game */
    game = new Minesweeper();
    if (!game) {
        allegro_exit();
        fprintf(stderr, "Failed to create game instance\n");
        return 1;
    }
    
    /* Initialize GUI */
    init_minesweeper_gui();
    
    /* Main loop */
    bool running = true;
    int frame_count = 0;
    
    /* Start game directly on Easy difficulty */
    game->setDifficulty(Difficulty::EASY);
    game->reset();
    game->state = GameState::PLAYING;
    display_status("Minesweeper - Left click to reveal, Right click to flag");
    mark_screen_dirty();  /* Force initial draw */
    
    while (running) {
        /* Check if window close button (X) was pressed */
        if (want_to_quit) {
            running = false;
            break;
        }
        
        /* Get next buffer for drawing */
        get_next_buffer_and_swap();
        
        /* Check for win/loss at the start of each frame (independent of input) */
        if (game && game->state == GameState::PLAYING) {
            /* Actually check the win condition (in case flagging changed the state) */
            if (game->checkWin()) {
                game->won = true;
            }
            
            if (game->won) {
                game->state = GameState::GAME_OVER;
                display_status("You won! Press any key...");
                mark_screen_dirty();
            } else if (game->gameOver) {
                game->state = GameState::GAME_OVER;
                display_status("Game Over! Press any key...");
                mark_screen_dirty();
            }
        }
        
        /* Show mouse cursor after buffer operations */
        show_mouse(screen);
        
        /* Update menu hover states based on mouse position */
        if (minesweeper_gui.show_file_menu) {
            int menu_y = MENU_BAR_HEIGHT;
            int item_h = 20;
            
            if (mouse_x >= 0 && mouse_x <= 200 && mouse_y >= menu_y) {
                int item_index = (mouse_y - menu_y) / item_h;
                if (item_index >= 0 && item_index < NUM_FILE_MENU_ITEMS && strlen(file_menu_items[item_index]) > 0) {
                    minesweeper_gui.file_menu_selected = item_index;
                    mark_screen_dirty();
                } else {
                    minesweeper_gui.file_menu_selected = -1;
                }
            } else {
                minesweeper_gui.file_menu_selected = -1;
            }
        } else {
            minesweeper_gui.file_menu_selected = -1;
        }
        
        /* Handle mouse clicks - only register when button is PRESSED, not held */
        if ((mouse_b & 1) && !(prev_mouse_b & 1)) {  /* Left click - button pressed */
            int mx = mouse_x;
            int my = mouse_y;
            
            /* Check if click is on menu bar */
            if (my >= 0 && my < 30) {
                /* File menu at x=5-40 */
                if (mx >= 5 && mx <= 40) {
                    minesweeper_gui.show_file_menu = !minesweeper_gui.show_file_menu;
                    mark_screen_dirty();
                }
            }
            /* Check if click is on menu items when menu is open */
            if (minesweeper_gui.show_file_menu && mx >= 0 && mx <= 180 && my >= 30) {
                int item_index = (my - 30) / 20;
                if (item_index >= 0 && item_index < NUM_FILE_MENU_ITEMS) {
                    handle_file_menu_click(item_index);
                    minesweeper_gui.show_file_menu = false;
                    mark_screen_dirty();
                }
            }
            /* Check if click is on game board during gameplay (only in PLAYING state) */
            else if (game->state == GameState::PLAYING) {
                if (mx >= BOARD_START_X && mx < BOARD_START_X + (game->width * CELL_SIZE) &&
                    my >= BOARD_START_Y && my < BOARD_START_Y + (game->height * CELL_SIZE)) {
                    
                    int col = (mx - BOARD_START_X) / CELL_SIZE;
                    int row = (my - BOARD_START_Y) / CELL_SIZE;
                    
                    if (col >= 0 && col < game->width && row >= 0 && row < game->height) {
                        game->reveal(col, row);
                        mark_screen_dirty();
                        
                        /* Check for win/loss after revealing */
                        if (game->won) {
                            game->state = GameState::GAME_OVER;
                            display_status("You won! Press any key...");
                            mark_screen_dirty();
                        } else if (game->gameOver) {
                            game->state = GameState::GAME_OVER;
                            display_status("Game Over! Press any key...");
                            mark_screen_dirty();
                        }
                    }
                }
            }
        }
        
        /* Handle right mouse button - flag or chord */
        if ((mouse_b & 2) && !(prev_mouse_b & 2)) {  /* Right click - button pressed */
            int mx = mouse_x;
            int my = mouse_y;
            
            if (game->state == GameState::PLAYING) {
                if (mx >= BOARD_START_X && mx < BOARD_START_X + (game->width * CELL_SIZE) &&
                    my >= BOARD_START_Y && my < BOARD_START_Y + (game->height * CELL_SIZE)) {
                    
                    int col = (mx - BOARD_START_X) / CELL_SIZE;
                    int row = (my - BOARD_START_Y) / CELL_SIZE;
                    
                    if (col >= 0 && col < game->width && row >= 0 && row < game->height) {
                        /* If cell is already revealed, do chord click (reveal adjacent) */
                        if (game->revealed[row][col]) {
                            game->revealAdjacentCells(row, col);
                            mark_screen_dirty();
                            
                            /* Check for win/loss after revealing adjacent */
                            if (game->won) {
                                game->state = GameState::GAME_OVER;
                                display_status("You won! Press any key...");
                                mark_screen_dirty();
                            } else if (game->gameOver) {
                                game->state = GameState::GAME_OVER;
                                display_status("Game Over! Press any key...");
                                mark_screen_dirty();
                            }
                        } else {
                            /* Otherwise toggle flag */
                            game->toggleFlag(col, row);
                            mark_screen_dirty();
                        }
                    }
                }
            }
        }
        
        /* Store previous mouse button state for next frame */
        prev_mouse_b = mouse_b;
        
        /* Check for win/lose conditions - just flag it, don't change state yet */
        if (game->state == GameState::PLAYING) {
            if (game->gameOver || game->won) {
                display_status(game->gameOver ? "Game Over! Press any key..." : "You won! Press any key...");
                mark_screen_dirty();
            }
        }
        
        /* Update game logic */
        if (game->state == GameState::PLAYING) {
            game->timer.update();
            mark_screen_dirty();  /* Redraw every frame during gameplay for timer */
        } else if (game->state == GameState::GAME_OVER) {
            mark_screen_dirty();  /* Keep redrawing board in GAME_OVER to show mines/mistakes */
        }
        
        /* Draw screen */
        draw_minesweeper_screen();
        
        /* NOW change state to GAME_OVER after drawing */
        if (game->state == GameState::PLAYING && (game->gameOver || game->won)) {
            game->state = GameState::GAME_OVER;
        }
        
        /* Always mark dirty to ensure updates */
        mark_screen_dirty();
        
        /* Handle keyboard input - AFTER drawing so board displays before state change */
        process_input(game, running);
        
        /* 30 FPS frame rate cap (33ms per frame) */
        rest(33);
        
        frame_count++;
    }
    
    /* Cleanup */
    if (game) {
        delete game;
    }
    
    for (int i = 0; i < NUM_BUFFERS; i++) {
        if (buffers[i]) {
            destroy_bitmap(buffers[i]);
        }
    }
    
    allegro_exit();
    
    return 0;
}

END_OF_MAIN()
