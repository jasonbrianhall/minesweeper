/*
 * minesweeper_main.cpp - Minesweeper Allegro 4 GUI Main Entry Point
 * Replaces the ncurses-based main with Allegro 4 graphics
 */

#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minesweeper.h"
#include "minesweeper_gui.h"

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
        buffers[i] = create_bitmap(640, 480);
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
        blit(active_buffer, screen, 0, 0, 0, 0, 640, 480);
        screen_dirty = 0;  /* Mark screen as clean after update */
    }
    
    /* Swap to next buffer for drawing */
    current_buffer = (current_buffer + 1) % NUM_BUFFERS;
    active_buffer = buffers[current_buffer];
    
    return active_buffer;
}

/**
 * Handle keyboard input and game state transitions
 */
void process_input(Minesweeper *game, bool &running) {
    if (!keypressed()) return;
    
    int key = readkey();
    int ascii = key & 0xFF;
    
    /* Global quit */
    if (ascii == 'q' || ascii == 'Q') {
        running = false;
        return;
    }
    
    /* Game state input */
    switch (game->state) {
        case GameState::MENU:
            /* Menu input */
            if (ascii == 'e' || ascii == 'E') {
                game->setDifficulty(Difficulty::EASY);
                game->reset();
                game->state = GameState::PLAYING;
                mark_screen_dirty();
            } else if (ascii == 'm' || ascii == 'M') {
                game->setDifficulty(Difficulty::MEDIUM);
                game->reset();
                game->state = GameState::PLAYING;
                mark_screen_dirty();
            } else if (ascii == 'h' || ascii == 'H') {
                game->setDifficulty(Difficulty::HARD);
                game->reset();
                game->state = GameState::PLAYING;
                mark_screen_dirty();
            } else if (ascii == 'c' || ascii == 'C') {
                game->state = GameState::PLAYING;
                game->enteringCustom = true;
                mark_screen_dirty();
            } else if (ascii == 's' || ascii == 'S') {
                game->state = GameState::HIGHSCORES;
                mark_screen_dirty();
            }
            display_status("Menu");
            break;
            
        case GameState::PLAYING:
            /* Gameplay input */
            handle_minesweeper_input(key);
            mark_screen_dirty();
            
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
            /* Return to menu on any key */
            if (game->won && game->isHighScore(game->timer.getElapsedSeconds())) {
                game->state = GameState::ENTER_NAME;
                minesweeper_gui.entering_name = true;
                minesweeper_gui.player_name_length = 0;
                memset(minesweeper_gui.player_name, 0, sizeof(minesweeper_gui.player_name));
            } else {
                game->state = GameState::MENU;
                display_status("Returning to menu...");
            }
            mark_screen_dirty();
            break;
            
        case GameState::ENTER_NAME:
            /* High score name entry */
            handle_minesweeper_input(key);
            mark_screen_dirty();
            break;
            
        case GameState::HIGHSCORES:
            /* Return to menu */
            game->state = GameState::MENU;
            display_status("Menu");
            mark_screen_dirty();
            break;
            
        case GameState::HELP:
            /* Return to menu */
            game->state = GameState::MENU;
            display_status("Menu");
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
    
    /* Set graphics mode - WINDOWED, not fullscreen */
    set_color_depth(8);
    if (set_gfx_mode(GFX_SAFE, 640, 480, 0, 0) != 0) {
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
    
    display_status("Welcome to Minesweeper! Select difficulty...");
    game->state = GameState::MENU;
    mark_screen_dirty();  /* Force initial draw */
    
    while (running) {
        /* Get next buffer for drawing */
        BITMAP *buffer = get_next_buffer_and_swap();
        
        /* Handle input */
        process_input(game, running);
        
        /* Update game logic */
        if (game->state == GameState::PLAYING) {
            game->timer.update();
            mark_screen_dirty();  /* Redraw every frame during gameplay for timer */
        }
        
        /* Draw screen */
        draw_minesweeper_screen();
        
        /* Always mark dirty to ensure updates - comment out if you want more efficient rendering */
        mark_screen_dirty();
        
        /* Frame rate cap (~60 FPS) */
        rest(16);
        
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
