/*
 * minesweeper.h - Minesweeper game class header
 * Defines GameState, Difficulty, Timer, and Minesweeper class
 */

#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include "highscores.h"
#include <atomic>
#include <ctime>
#include <random>
#include <set>
#include <string>
#include <vector>

#define MAX_SCORES 10

enum class GameState { MENU, PLAYING, HELP, GAME_OVER, HIGHSCORES, ENTER_NAME };

enum class Difficulty {
    EASY,    // 9x9, 10 mines
    MEDIUM,  // 16x16, 40 mines
    HARD,    // 16x30, 99 mines
    CUSTOM   // User-defined
};

struct DifficultySettings {
    int height;
    int width;
    int mines;
};

extern const DifficultySettings DIFFICULTY_SETTINGS[];

/* Timer class for tracking game time */
class Timer {
private:
    clock_t startTime;
    bool running;
    int elapsedSeconds;

public:
    Timer() : running(false), elapsedSeconds(0) {}

    void start() {
        startTime = clock();
        running = true;
    }

    void stop() {
        if (running) {
            update();
            running = false;
        }
    }

    int getElapsedSeconds() const { return elapsedSeconds; }

    void update() {
        if (running) {
            clock_t now = clock();
            elapsedSeconds = (now - startTime) / CLOCKS_PER_SEC;
        }
    }

    std::string getTimeString() const {
        int minutes = elapsedSeconds / 60;
        int seconds = elapsedSeconds % 60;
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
        return std::string(buffer);
    }
};

/* Main Minesweeper game class */
class Minesweeper {
public:
    /* Public game state */
    GameState state;
    Difficulty difficulty;
    Timer timer;
    bool gameOver;
    bool won;
    bool firstMove;
    
    /* Board dimensions and content */
    int height;
    int width;
    int mines;
    int cursorY;
    int cursorX;
    int currentSeed;
    
    /* Game board arrays - accessible to GUI */
    std::vector<std::vector<bool>> minefield;   // true = mine
    std::vector<std::vector<bool>> revealed;    // true = revealed
    std::vector<std::vector<bool>> flagged;     // true = flagged
    
    /* Input and state flags */
    bool enteringCustom;
    bool enteringName;
    bool enteringSeed;
    
    /* Text input buffers */
    std::string playerName;
    std::string customWidth;
    std::string customHeight;
    std::string customMines;
    std::string seedInput;
    int customStep;
    
    /* High score management */
    Highscores highscores;
    
    /* Constructor and destructor */
    Minesweeper();
    ~Minesweeper();
    
    /* Game initialization and reset */
    void setDifficulty(Difficulty diff);
    void reset();
    void initializeMinefield(int firstY, int firstX, int seed = -1);
    
    /* Game state queries */
    bool isGameOver() const { return gameOver || won; }
    bool isHighScore(int time) const;
    bool isRunning() const { return state == GameState::PLAYING && !gameOver && !won; }
    
    /* Cell operations */
    void reveal(int x, int y);
    void toggleFlag(int x, int y);
    void revealCell(int y, int x);
    void revealAdjacentCells(int y, int x);
    void revealAllMines();
    int countAdjacentMines(int x, int y);
    int countAdjacentFlags(int y, int x);
    bool checkWin();
    
    /* Drawing and display (stubs for Allegro GUI) */
    void draw();
    void updateTitle();
    void drawMenu();
    void drawHelp();
    void drawHighscores();
    void drawEnterName();
    void drawTitle();
    void setupColors();
    
    /* Input handling */
    bool handleInput(int ch);
    void handleNameEntry(int ch);
    void handleCustomDifficulty(int ch);
    
    /* High score management */
    void saveHighscore();
    
    /* Utility */
    std::string getTimeString() const { return timer.getTimeString(); }
};

#endif /* MINESWEEPER_H */
