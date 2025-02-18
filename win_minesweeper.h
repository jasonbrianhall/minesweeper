#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "highscores.h"

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

class GameTimer {
private:
    std::chrono::steady_clock::time_point startTime;
    bool running;
    int elapsedSeconds;

public:
    void start();
    void stop();
    std::string getTimeString() const;
    int getElapsedSeconds() const;
};

class Minesweeper {
public:
    int height, width, mines;
    bool firstMove;
    bool gameOver;
    bool won;
    std::string playerName;
    GameTimer timer;
    Highscores highscores;
    std::vector<std::vector<bool>> minefield;
    std::vector<std::vector<bool>> revealed;
    std::vector<std::vector<bool>> flagged;
    int currentSeed;
    Minesweeper();
    void setDifficulty(Difficulty diff);
    void reset();
    void initializeMinefield(int row, int col, int seed);
    int countAdjacentMines(int row, int col);
    void revealCell(int row, int col);
    void revealAllMines();
    bool checkWin();
    bool isHighScore(int time, const std::string& difficulty);
    void saveHighscore();
};

namespace MinesweeperGame {
    ref class MinesweeperWrapper;
    ref class MainForm;
}

struct CustomGameSettings {
    int height;
    int width;
    int mines;
    static const int MIN_SIZE = 5;
    static const int MAX_SIZE = 50;
    static const int MIN_MINES = 1;
    
    static bool isValid(int h, int w, int m) {
        return h >= MIN_SIZE && h <= MAX_SIZE &&
               w >= MIN_SIZE && w <= MAX_SIZE &&
               m >= MIN_MINES && m < (h * w);
    }
};
