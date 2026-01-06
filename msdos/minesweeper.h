#pragma once

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include "highscores.h"

/* Game state enum */
enum class GameState {
    MENU,
    PLAYING,
    GAME_OVER,
    HELP,
    HIGHSCORES,
    ENTER_NAME
};

/* Difficulty enum */
enum class Difficulty {
    EASY = 0,
    MEDIUM = 1,
    HARD = 2,
    CUSTOM = 3
};

/* Difficulty settings structure */
struct DifficultySettings {
    int height;
    int width;
    int mines;
};

/* Timer class */
class Timer {
public:
    Timer() : startTime(0), stopTime(0), isRunning(false) {}
    
    void start() {
        startTime = time(nullptr);
        isRunning = true;
        stopTime = 0;
    }
    
    void stop() {
        stopTime = time(nullptr);
        isRunning = false;
    }
    
    void update() {
        /* Timer updates itself via system time */
    }
    
    int getElapsedSeconds() const {
        if (startTime == 0) return 0;
        time_t current = stopTime > 0 ? stopTime : time(nullptr);
        return static_cast<int>(difftime(current, startTime));
    }
    
    std::string getTimeString() const {
        int elapsed = getElapsedSeconds();
        int minutes = elapsed / 60;
        int seconds = elapsed % 60;
        char buf[32];
        sprintf(buf, "%d:%02d", minutes, seconds);
        return std::string(buf);
    }

private:
    time_t startTime;
    time_t stopTime;
    bool isRunning;
};

/* Main Minesweeper Game Class */
class Minesweeper {
public:
    Minesweeper();
    ~Minesweeper();
    
    /* Game state */
    GameState state;
    Difficulty difficulty;
    bool gameOver;
    bool won;
    bool firstMove;
    
    /* Board dimensions */
    int height;
    int width;
    int mines;
    
    /* Board state */
    std::vector<std::vector<bool>> minefield;
    std::vector<std::vector<bool>> revealed;
    std::vector<std::vector<bool>> flagged;
    
    /* Cursor position */
    int cursorY;
    int cursorX;
    int currentSeed;
    
    /* Game mechanics */
    void setDifficulty(Difficulty diff);
    void reset();
    void initializeMinefield(int firstY, int firstX, int seed = -1);
    void reveal(int x, int y);
    void toggleFlag(int x, int y);
    void revealCell(int y, int x);
    void revealAdjacentCells(int y, int x);
    void revealAllMines();
    bool checkWin();
    int countAdjacentMines(int x, int y);
    int countAdjacentFlags(int y, int x);
    
    /* Input handling */
    bool handleInput(int ch);
    void handleNameEntry(int ch);
    void handleCustomDifficulty(int ch);
    
    /* High scores */
    bool isHighScore(int time) const;
    void saveHighscore();
    
    /* Drawing (handled by GUI) */
    void draw();
    void drawTitle();
    void drawMenu();
    void drawHelp();
    void drawHighscores();
    void drawEnterName();
    void setupColors();
    void updateTitle();
    
    /* Timing */
    Timer timer;
    
    /* Player name for high score entry */
    std::string playerName;
    
    /* Custom game input */
    bool enteringCustom;
    bool enteringName;
    bool enteringSeed;
    int customStep;
    std::string customWidth;
    std::string customHeight;
    std::string customMines;
    std::string seedInput;
    
    /* Highscores */
    Highscores highscores;
    
    /* Constants */
    static const int MAX_SCORES = 10;
};
