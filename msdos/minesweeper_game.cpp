/*
 * minesweeper_game.cpp - Minesweeper Game Logic Only (No UI)
 * Pure game mechanics without any ncurses or Allegro code
 */

#include "minesweeper.h"
#include "highscores.h"
#include <random>
#include <ctime>
#include <algorithm>
#include <cmath>

// Difficulty settings constants
const DifficultySettings DIFFICULTY_SETTINGS[] = {
    {9, 9, 10},    // EASY
    {16, 16, 40},  // MEDIUM
    {16, 30, 99}   // HARD
};

/* Minesweeper Constructor */
Minesweeper::Minesweeper() 
    : state(GameState::MENU), difficulty(Difficulty::EASY), 
      gameOver(false), won(false), firstMove(true),
      height(9), width(9), mines(10), cursorY(0), cursorX(0), currentSeed(-1),
      enteringCustom(false), enteringName(false), enteringSeed(false),
      customStep(0) {
    setDifficulty(Difficulty::EASY);
}

Minesweeper::~Minesweeper() {
    // Cleanup if needed
}

/* Set difficulty and initialize board */
void Minesweeper::setDifficulty(Difficulty diff) {
    difficulty = diff;
    const auto &settings = DIFFICULTY_SETTINGS[static_cast<int>(diff)];
    height = settings.height;
    width = settings.width;
    mines = settings.mines;
    reset();
}

/* Reset game state */
void Minesweeper::reset() {
    revealed = std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));
    flagged = std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));
    minefield = std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));
    
    firstMove = true;
    gameOver = false;
    won = false;
    cursorY = 0;
    cursorX = 0;
    timer = Timer();
    seedInput.clear();
    enteringSeed = false;
    playerName.clear();
    enteringName = false;
}

/* Initialize minefield with mines */
void Minesweeper::initializeMinefield(int firstY, int firstX, int seed) {
    minefield = std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));

    if (seed == -1) {
        unsigned int timeSeed = static_cast<unsigned int>(time(nullptr));
        currentSeed = timeSeed ^ (timeSeed << 16);
        currentSeed = ((currentSeed * 1103515245 + 12345) * 65535) & 0xFFFFFFFF;
    } else {
        currentSeed = seed;
    }

    std::mt19937 gen(currentSeed);
    std::uniform_int_distribution<> disH(0, height - 1);
    std::uniform_int_distribution<> disW(0, width - 1);

    int placedMines = 0;
    while (placedMines < mines) {
        int y = disH(gen);
        int x = disW(gen);
        if (!minefield[y][x] && (abs(y - firstY) > 1 || abs(x - firstX) > 1)) {
            minefield[y][x] = true;
            placedMines++;
        }
    }
}

/* Count adjacent mines */
int Minesweeper::countAdjacentMines(int x, int y) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int newY = y + dy;
            int newX = x + dx;
            if (newY >= 0 && newY < height && newX >= 0 && newX < width) {
                if (minefield[newY][newX]) {
                    count++;
                }
            }
        }
    }
    return count;
}

/* Count adjacent flags */
int Minesweeper::countAdjacentFlags(int y, int x) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int newY = y + dy;
            int newX = x + dx;
            if (newY >= 0 && newY < height && newX >= 0 && newX < width) {
                if (flagged[newY][newX]) {
                    count++;
                }
            }
        }
    }
    return count;
}

/* Reveal a cell (recursive for empty cells) */
void Minesweeper::revealCell(int y, int x) {
    if (y < 0 || y >= height || x < 0 || x >= width || revealed[y][x] || flagged[y][x]) {
        return;
    }

    revealed[y][x] = true;

    if (countAdjacentMines(x, y) == 0) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                revealCell(y + dy, x + dx);
            }
        }
    }
}

/* Reveal adjacent cells when chord-clicking */
void Minesweeper::revealAdjacentCells(int y, int x) {
    if (!revealed[y][x]) {
        return;
    }

    int mineCount = countAdjacentMines(x, y);
    int flagCount = countAdjacentFlags(y, x);

    if (mineCount == flagCount) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int newY = y + dy;
                int newX = x + dx;
                if (newY >= 0 && newY < height && newX >= 0 && newX < width) {
                    if (!flagged[newY][newX] && !revealed[newY][newX]) {
                        if (minefield[newY][newX]) {
                            gameOver = true;
                            revealAllMines();
                            timer.stop();
                            return;
                        }
                        revealCell(newY, newX);
                    }
                }
            }
        }
    }
}

/* Reveal all mines */
void Minesweeper::revealAllMines() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (minefield[y][x]) {
                revealed[y][x] = true;
            }
        }
    }
}

/* Check if player won */
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

/* Public wrapper: reveal cell by (x, y) */
void Minesweeper::reveal(int x, int y) {
    if (firstMove) {
        firstMove = false;
        timer.start();
        initializeMinefield(y, x, currentSeed);
    }

    if (minefield[y][x]) {
        gameOver = true;
        revealAllMines();
        timer.stop();
    } else {
        revealCell(y, x);
    }

    won = checkWin();
    if (won) {
        timer.stop();
    }
}

/* Public wrapper: toggle flag */
void Minesweeper::toggleFlag(int x, int y) {
    if (!revealed[y][x]) {
        flagged[y][x] = !flagged[y][x];
    }
}

/* Check if score qualifies as high score */
bool Minesweeper::isHighScore(int time) const {
    std::string currentDifficulty;

    switch (difficulty) {
    case Difficulty::EASY:
        currentDifficulty = "Easy";
        break;
    case Difficulty::MEDIUM:
        currentDifficulty = "Medium";
        break;
    case Difficulty::HARD:
        currentDifficulty = "Hard";
        break;
    case Difficulty::CUSTOM:
        return false;  // Don't save custom difficulty scores
    }

    // Count scores for current difficulty
    const auto &scores = highscores.getScores();
    int difficultyScores = 0;
    bool worseThanCurrent = false;

    for (const auto &score : scores) {
        if (score.difficulty == currentDifficulty) {
            difficultyScores++;
            if (score.time > time) {
                worseThanCurrent = true;
            }
        }
    }

    // If we have less than MAX_SCORES for this difficulty, it's a high score
    // Or if we found a worse score in the current list
    return difficultyScores < MAX_SCORES || worseThanCurrent;
}

/* Save high score */
void Minesweeper::saveHighscore() {
    Score score;
    score.name = playerName;
    score.time = timer.getElapsedSeconds();

    switch (difficulty) {
    case Difficulty::EASY:
        score.difficulty = "Easy";
        break;
    case Difficulty::MEDIUM:
        score.difficulty = "Medium";
        break;
    case Difficulty::HARD:
        score.difficulty = "Hard";
        break;
    case Difficulty::CUSTOM:
        return;  // Don't save custom difficulty scores
    }

    highscores.addScore(score);
}

/* Handle input (stub for Allegro - just track menu navigation) */
bool Minesweeper::handleInput(int ch) {
    if (state == GameState::ENTER_NAME) {
        handleNameEntry(ch);
        return true;
    }
    
    if (state == GameState::MENU) {
        if (enteringCustom) {
            handleCustomDifficulty(ch);
            return true;
        } else if (enteringSeed) {
            if (ch == '\n' || ch == '\r' || ch == 10) {
                enteringSeed = false;
                try {
                    currentSeed = std::stoi(seedInput);
                } catch (...) {
                    currentSeed = -1;
                }
                return true;
            } else if (ch == 27) {  // ESC
                enteringSeed = false;
                currentSeed = -1;
                seedInput.clear();
            } else if (ch == 8 || ch == 127) {  // Backspace
                if (!seedInput.empty())
                    seedInput.pop_back();
            } else if (isdigit(ch)) {
                seedInput += ch;
            }
            return true;
        }

        switch (ch) {
        case 's':
        case 'S':
            enteringSeed = true;
            seedInput.clear();
            break;
        case '1':
            setDifficulty(Difficulty::EASY);
            state = GameState::PLAYING;
            break;
        case '2':
            setDifficulty(Difficulty::MEDIUM);
            state = GameState::PLAYING;
            break;
        case '3':
            setDifficulty(Difficulty::HARD);
            state = GameState::PLAYING;
            break;
        case '4':
            enteringCustom = true;
            customStep = 0;
            customWidth.clear();
            customHeight.clear();
            customMines.clear();
            break;
        case 'h':
        case 'H':
            state = GameState::HELP;
            break;
        case 'k':
        case 'K':
            state = GameState::HIGHSCORES;
            break;
        case 'q':
        case 'Q':
            return false;
        }
        return true;
    }

    if (state == GameState::HELP) {
        state = GameState::MENU;
        return true;
    }

    // Playing state
    if (state == GameState::PLAYING) {
        switch (ch) {
        case 'q':
        case 'Q':
            return false;
        case 'r':
        case 'R':
            reset();
            break;
        case 'h':
        case 'H':
            state = GameState::HELP;
            break;
        case 'k':
        case 'K':
            if (isHighScore(timer.getElapsedSeconds())) {
                state = GameState::ENTER_NAME;
                playerName = "";
            } else {
                state = GameState::HIGHSCORES;
            }
            break;
        case 'n':
        case 'N':
            state = GameState::MENU;
            currentSeed = -1;
            break;
        }
    }

    return true;
}

/* Handle name entry */
void Minesweeper::handleNameEntry(int ch) {
    if (ch == '\n' || ch == '\r' || ch == 10) {
        if (!playerName.empty()) {
            saveHighscore();
            state = GameState::HIGHSCORES;
            playerName = "";
        }
    } else if (ch == 27) {
        state = GameState::MENU;
        playerName = "";
    } else if (ch == 8 || ch == 127) {
        if (!playerName.empty()) {
            playerName.pop_back();
        }
    } else if (ch >= 32 && ch < 127 && playerName.length() < 20) {
        playerName += (char)ch;
    }
}

/* Handle custom difficulty input */
void Minesweeper::handleCustomDifficulty(int ch) {
    std::string *currentInput = nullptr;
    switch (customStep) {
    case 0:
        currentInput = &customWidth;
        break;
    case 1:
        currentInput = &customHeight;
        break;
    case 2:
        currentInput = &customMines;
        break;
    }

    if (ch == '\n' || ch == '\r' || ch == 10) {
        customStep++;
        if (customStep > 2) {
            try {
                int w = std::stoi(customWidth);
                int h = std::stoi(customHeight);
                int m = std::stoi(customMines);

                w = std::max(5, std::min(w, 30));
                h = std::max(5, std::min(h, 30));
                m = std::max(1, std::min(m, (w * h) - 9));

                difficulty = Difficulty::CUSTOM;
                width = w;
                height = h;
                mines = m;
                reset();
                state = GameState::PLAYING;
            } catch (...) {
                // Invalid input, reset to menu
                state = GameState::MENU;
            }
            enteringCustom = false;
            customStep = 0;
            customWidth.clear();
            customHeight.clear();
            customMines.clear();
        }
    } else if (ch == 27) {
        enteringCustom = false;
        customStep = 0;
        customWidth.clear();
        customHeight.clear();
        customMines.clear();
    } else if (ch == 8 || ch == 127) {
        if (currentInput && !currentInput->empty()) {
            currentInput->pop_back();
        }
    } else if (isdigit(ch)) {
        if (currentInput && currentInput->length() < 2) {
            *currentInput += ch;
        }
    }
}

/* Stub drawing methods - Allegro GUI handles all drawing */
void Minesweeper::draw() { /* Handled by Allegro GUI */ }
void Minesweeper::drawTitle() { /* Handled by Allegro GUI */ }
void Minesweeper::drawMenu() { /* Handled by Allegro GUI */ }
void Minesweeper::drawHelp() { /* Handled by Allegro GUI */ }
void Minesweeper::drawHighscores() { /* Handled by Allegro GUI */ }
void Minesweeper::drawEnterName() { /* Handled by Allegro GUI */ }
void Minesweeper::setupColors() { /* Handled by Allegro GUI */ }
void Minesweeper::updateTitle() { /* Handled by Allegro GUI */ }
