#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <vector>
#include <random>
#include <chrono>
#include <string>
#include <thread>
#include <atomic>
#include "highscores.h"

enum class GameState {
    MENU,
    PLAYING,
    HELP,
    GAME_OVER,
    HIGHSCORES,
    ENTER_NAME
};

enum class Difficulty {
    EASY,    // 9x9, 10 mines
    MEDIUM,  // 16x16, 40 mines
    HARD     // 16x30, 99 mines
};

struct DifficultySettings {
    int height;
    int width;
    int mines;
};

const DifficultySettings DIFFICULTY_SETTINGS[] = {
    {9, 9, 10},    // EASY
    {16, 16, 40},  // MEDIUM
    {16, 30, 99}   // HARD
};

class GameTimer {  // Renamed from Timer to avoid conflict with Windows::Forms::Timer
private:
    std::chrono::steady_clock::time_point startTime;
    bool running;
    int elapsedSeconds;

public:
    GameTimer();
    void start();
    void stop();
    int getElapsedSeconds() const;
    void update();
    std::string getTimeString() const;
};

class Minesweeper {
public:
    int height, width, mines;
    std::vector<std::vector<bool>> minefield;
    std::vector<std::vector<bool>> revealed;
    std::vector<std::vector<bool>> flagged;
    bool gameOver;
    bool won;
    bool firstMove;
    GameTimer timer;
    int currentSeed;
    std::string playerName;
    Highscores highscores;

    Minesweeper();
    void setDifficulty(Difficulty diff);
    void reset();
    void initializeMinefield(int firstY, int firstX, int seed = -1);
    int countAdjacentMines(int y, int x);
    void revealCell(int y, int x);
    void revealAllMines();
    bool checkWin();
    bool isHighScore(int time);
    void saveHighscore();
};

#endif 
