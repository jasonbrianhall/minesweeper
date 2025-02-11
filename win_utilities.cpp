#include "win_minesweeper.h"
#include <sstream>
#include <iomanip>
#include <random>

// GameTimer implementation
void GameTimer::start() {
    startTime = std::chrono::steady_clock::now();
    running = true;
}

void GameTimer::stop() {
    if (running) {
        auto now = std::chrono::steady_clock::now();
        elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(
            now - startTime).count();
        running = false;
    }
}

int GameTimer::getElapsedSeconds() const {
    if (running) {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(
            now - startTime).count();
    }
    return elapsedSeconds;
}

std::string GameTimer::getTimeString() const {
    int totalSeconds = getElapsedSeconds();
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << minutes 
       << ":" << std::setfill('0') << std::setw(2) << seconds;
    return ss.str();
}

// Minesweeper implementation
Minesweeper::Minesweeper() : height(9), width(9), mines(10), 
                gameOver(false), won(false), firstMove(true) {
    reset();
}

void Minesweeper::setDifficulty(Difficulty diff) {
    const DifficultySettings& settings = DIFFICULTY_SETTINGS[static_cast<int>(diff)];
    height = settings.height;
    width = settings.width;
    mines = settings.mines;
    reset();
}

void Minesweeper::reset() {
    minefield = std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));
    revealed = std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));
    flagged = std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));
    gameOver = false;
    won = false;
    firstMove = true;
    timer.stop();
}

void Minesweeper::initializeMinefield(int firstY, int firstX, int seed) {
    std::mt19937 gen;
    if (seed < 0) {
        std::random_device rd;
        gen.seed(rd());
    } else {
        gen.seed(static_cast<unsigned int>(seed));
    }
    
    std::uniform_int_distribution<> disH(0, height - 1);
    std::uniform_int_distribution<> disW(0, width - 1);
    
    int placedMines = 0;
    while (placedMines < mines) {
        int y = disH(gen);
        int x = disW(gen);
        if (!minefield[y][x] && 
            (abs(y - firstY) > 1 || abs(x - firstX) > 1)) {
            minefield[y][x] = true;
            placedMines++;
        }
    }
}

int Minesweeper::countAdjacentMines(int y, int x) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int newY = y + dy;
            int newX = x + dx;
            if (newY >= 0 && newY < height && newX >= 0 && newX < width) {
                if (minefield[newY][newX]) count++;
            }
        }
    }
    return count;
}

void Minesweeper::revealCell(int y, int x) {
    if (y < 0 || y >= height || x < 0 || x >= width || 
        revealed[y][x] || flagged[y][x]) 
        return;

    revealed[y][x] = true;
    
    if (countAdjacentMines(y, x) == 0) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                revealCell(y + dy, x + dx);
            }
        }
    }
}

void Minesweeper::revealAllMines() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (minefield[y][x]) revealed[y][x] = true;
        }
    }
}

bool Minesweeper::checkWin() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (!minefield[y][x] && !revealed[y][x]) {
                return false;
            }
        }
    }
    return true;
}

bool Minesweeper::isHighScore(int time) {
    const auto& scores = highscores.getScores();
    std::string difficultyStr;
    
    switch (width) {
        case 9: difficultyStr = "Easy"; break;
        case 16: difficultyStr = (height == 16) ? "Medium" : "Hard"; break;
        default: return false;
    }
    
    int count = 0;
    for (const auto& score : scores) {
        if (score.difficulty == difficultyStr) {
            count++;
            if (count >= 10 && score.time <= time) {
                return false;
            }
        }
    }
    return count < 10 || time < scores.back().time;
}

void Minesweeper::saveHighscore() {
    std::string difficultyStr;
    switch (width) {
        case 9: difficultyStr = "Easy"; break;
        case 16: difficultyStr = (height == 16) ? "Medium" : "Hard"; break;
        default: return;
    }
    
    Score score;
    score.name = playerName;
    score.time = timer.getElapsedSeconds();
    score.difficulty = difficultyStr;
    highscores.addScore(score);
}

