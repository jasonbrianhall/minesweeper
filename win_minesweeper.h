#pragma once

#include <string>
#include <vector>
#include "highscores.h"

// Native C++ classes
enum class Difficulty {
    EASY,
    MEDIUM,
    HARD
};

class GameTimer {
public:
    void start();
    void stop();
    std::string getTimeString() const;
};

class Minesweeper {
public:
    bool firstMove;
    bool gameOver;
    bool won;
    int width;
    int height;
    std::string playerName;
    GameTimer timer;
    Highscores highscores;
    std::vector<std::vector<bool>> minefield;
    std::vector<std::vector<bool>> revealed;
    std::vector<std::vector<bool>> flagged;

    Minesweeper();
    void setDifficulty(Difficulty diff);
    void reset();
    void initializeMinefield(int row, int col);
    int countAdjacentMines(int row, int col);
    void revealCell(int row, int col);
    void revealAllMines();
    bool checkWin();
    bool isHighScore(int time);
    void saveHighscore();
};

// Forward declarations for managed classes
namespace MinesweeperGame {
    ref class MinesweeperWrapper;
    ref class MainForm;
}
