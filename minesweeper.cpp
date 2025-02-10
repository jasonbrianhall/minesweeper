#include <ncurses.h>
#include <random>
#include <ctime>
#include <vector>
#include <set>
#include <chrono>
#include <string>
#include <thread>
#include <atomic>

enum class GameState {
    MENU,
    PLAYING,
    HELP,
    GAME_OVER
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

class Timer {
private:
    std::chrono::steady_clock::time_point startTime;
    bool running;
    int elapsedSeconds;

public:
    Timer() : running(false), elapsedSeconds(0) {}

    void start() {
        startTime = std::chrono::steady_clock::now();
        running = true;
    }

    void stop() {
        if (running) {
            update();
            running = false;
        }
    }

    void update() {
        if (running) {
            auto now = std::chrono::steady_clock::now();
            elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(
                now - startTime).count();
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

class Minesweeper {
private:
    std::thread titleThread;
    std::atomic<bool> running{true};
    int height, width, mines;
    std::vector<std::vector<bool>> minefield;
    std::vector<std::vector<bool>> revealed;
    std::vector<std::vector<bool>> flagged;
    int cursorY = 0, cursorX = 0;
    bool gameOver = false;
    bool won = false;
    bool firstMove = true;
    GameState state = GameState::MENU;
    Difficulty difficulty = Difficulty::EASY;
    Timer timer;
    int currentSeed=-1;

    
    void animateTitle() {
        const std::string title = "MINESWEEPER";
        int direction = 1;
        int pos = 0;
        int maxPos = width - title.length();
    
        while (running) {
            if (state == GameState::PLAYING) {
                move(0, 0);
                clrtoeol();
            
                attron(COLOR_PAIR(10) | A_BOLD);
                mvprintw(0, 0, "%s", title.c_str());
                attroff(COLOR_PAIR(10) | A_BOLD);
            
                if (!firstMove && !checkWin()) {
                    timer.update();
                    mvprintw(0, width * 2 + 5, "Time: %s", timer.getTimeString().c_str());
                } else if (firstMove) {
                    mvprintw(0, width * 2 + 5, "Time: %s", timer.getTimeString().c_str());
                } else {
                    mvprintw(0, width * 2 + 5, "Time: %s - You win", timer.getTimeString().c_str());
                }
            
                refresh();
            
                pos += direction;
                if (pos >= maxPos || pos <= 0) {
                    direction *= -1;
                }
            }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
        
    void setupColors() {
        start_color();
        init_pair(1, COLOR_BLUE, COLOR_BLACK);      // 1
        init_pair(2, COLOR_GREEN, COLOR_BLACK);     // 2
        init_pair(3, COLOR_RED, COLOR_BLACK);       // 3
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);    // 4
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);   // 5
        init_pair(6, COLOR_CYAN, COLOR_BLACK);      // 6
        init_pair(7, COLOR_WHITE, COLOR_BLACK);     // 7,8
        init_pair(8, COLOR_RED, COLOR_BLACK);       // mine
        init_pair(9, COLOR_YELLOW, COLOR_BLACK);    // flag
        init_pair(10, COLOR_WHITE, COLOR_BLUE);     // title
    }

    void initializeMinefield(int firstY, int firstX, int seed = -1) {
        minefield = std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));
        
        if (seed == -1) {
            std::random_device rd;
            currentSeed = rd();
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
            if (!minefield[y][x] && 
                (abs(y - firstY) > 1 || abs(x - firstX) > 1)) {
                minefield[y][x] = true;
                placedMines++;
            }
        }
    }
    
    int countAdjacentMines(int y, int x) {
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

    int countAdjacentFlags(int y, int x) {
        int count = 0;
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int newY = y + dy;
                int newX = x + dx;
                if (newY >= 0 && newY < height && newX >= 0 && newX < width) {
                    if (flagged[newY][newX]) count++;
                }
            }
        }
        return count;
    }

    void revealCell(int y, int x) {
        if (y < 0 || y >= height || x < 0 || x >= width || revealed[y][x] || flagged[y][x]) 
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

    void revealAdjacentCells(int y, int x) {
        if (!revealed[y][x]) return;

        int mineCount = countAdjacentMines(y, x);
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

    void revealAllMines() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (minefield[y][x]) revealed[y][x] = true;
            }
        }
    }

    bool checkWin() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (!minefield[y][x] && !revealed[y][x]) {
                    return false;
                }
            }
        }
        return true;
    }
    
    void drawTitle() {
        attron(COLOR_PAIR(10) | A_BOLD);
        mvprintw(0, 0, "MINESWEEPER");
        attroff(COLOR_PAIR(10) | A_BOLD);
        if (!firstMove) {
            mvprintw(0, width * 2 + 5, "Time: %s", timer.getTimeString().c_str());
        }
    }

    void drawMenu() {
        clear();
        mvprintw(height/2 - 2, width, "MINESWEEPER");
        mvprintw(height/2, width, "Select Difficulty:");
        mvprintw(height/2 + 1, width, "1. Easy (9x9, 10 mines)");
        mvprintw(height/2 + 2, width, "2. Medium (16x16, 40 mines)");
        mvprintw(height/2 + 3, width, "3. Hard (16x30, 99 mines)");
        mvprintw(height/2 + 5, width, "Press 1-3 to start");
        mvprintw(height/2 + 6, width, "Press H for help");
    }

    void drawHelp() {
        clear();
        mvprintw(2, 2, "MINESWEEPER HELP");
        mvprintw(4, 2, "Controls:");
        mvprintw(5, 4, "Arrow Keys: Move cursor");
        mvprintw(6, 4, "Space: Reveal cell");
        mvprintw(7, 4, "F: Flag/unflag cell");
        mvprintw(8, 4, "H: Show/hide help");
        mvprintw(9, 4, "C: Clear Screen and get rid of any artifacts");
        mvprintw(10, 4, "N: New Game");
        mvprintw(11, 4, "Q: Quit game");
        mvprintw(13, 2, "Tips:");
        mvprintw(14, 4, "- First click is always safe");
        mvprintw(15, 4, "- Numbers show adjacent mines");
        mvprintw(16, 4, "- Flag suspected mines with F");
        mvprintw(17, 4, "- Press space on revealed numbers to clear adjacent cells");
        mvprintw(18, 2, "Press any key to return");
    }

public:
    Minesweeper() {
        setupColors();
        setDifficulty(Difficulty::EASY);
        titleThread = std::thread(&Minesweeper::animateTitle, this);
    }

    ~Minesweeper() {
        running = false;
        if (titleThread.joinable()) {
            titleThread.join();
        }
    }

    void setDifficulty(Difficulty diff) {
        difficulty = diff;
        const auto& settings = DIFFICULTY_SETTINGS[static_cast<int>(diff)];
        height = settings.height;
        width = settings.width;
        mines = settings.mines;
        reset();
    }

    void reset() {
        revealed = std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));
        flagged = std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));
        firstMove = true;
        gameOver = false;
        won = false;
        cursorY = 0;
        cursorX = 0;
        timer = Timer();
        clear();
    }

    void draw() {
        if (state == GameState::MENU) {
            drawMenu();
            return;
        }
        if (state == GameState::HELP) {
            drawHelp();
            return;
        }

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                move(y + 2, x * 2);  // +2 for title space
                if (y == cursorY && x == cursorX) {
                    attron(A_REVERSE);
                }

                if (flagged[y][x]) {
                    attron(COLOR_PAIR(9));
                    addch('F');
                    addch(' ');
                    attroff(COLOR_PAIR(9));
                } else if (!revealed[y][x]) {
                    addch('#');
                    addch(' ');
                } else if (minefield[y][x]) {
                    attron(COLOR_PAIR(8) | A_BOLD);
                    addch('*');
                    addch(' ');
                    attroff(COLOR_PAIR(8) | A_BOLD);
                } else {
                    int count = countAdjacentMines(y, x);
                    if (count == 0) {
                        addch(' ');
                        addch(' ');
                    } else {
                        attron(COLOR_PAIR(count) | A_BOLD);
                        addch('0' + count);
                        addch(' ');
                        attroff(COLOR_PAIR(count) | A_BOLD);
                    }
                }

                if (y == cursorY && x == cursorX) {
                    attroff(A_REVERSE);
                }
            }
        }

        // Status and instructions
        mvprintw(height + 3, 0, "Press 'h' for help | Space=Reveal | F=Flag | Q=Quit");

        if (gameOver) {
            mvprintw(height + 4, 0, "Game Over! Press 'q' to quit or 'r' to restart.");
        } else if (won) {
            mvprintw(height + 4, 0, "You Won! Time: %s | Press 'q' to quit or 'r' to restart.", 
                    timer.getTimeString().c_str());
        }

        refresh();
    }

    bool handleInput(int ch) {
        if (state == GameState::MENU) {
            switch (ch) {
                case '1':
                    setDifficulty(Difficulty::EASY);
                    state = GameState::PLAYING;
                    clear();
                    break;
                case '2':
                    setDifficulty(Difficulty::MEDIUM);
                    state = GameState::PLAYING;
                    clear();
                    break;
                case '3':
                    setDifficulty(Difficulty::HARD);
                    state = GameState::PLAYING;
                    clear();    
                    break;
                case 'h':
                case 'H':
                    state = GameState::HELP;
                    break;    
                case 'q':
                case 'Q':
                    return false;
            }
            return true;
        }

        if (state == GameState::HELP) {
            state = GameState::PLAYING;
            clear();
            return true;
        }
                
        switch (ch) {
            case KEY_UP:
                if (cursorY > 0) cursorY--;
                break;
            case KEY_DOWN:
                if (cursorY < height - 1) cursorY++;
                break;
            case KEY_LEFT:
                if (cursorX > 0) cursorX--;
                break;
            case KEY_RIGHT:
                if (cursorX < width - 1) cursorX++;
                break;
            case ' ':
                if (firstMove) {
                    initializeMinefield(cursorY, cursorX);
                    firstMove = false;
                    timer.start();
                }
                if (revealed[cursorY][cursorX]) {
                    revealAdjacentCells(cursorY, cursorX);
                } else if (!flagged[cursorY][cursorX]) {
                    if (minefield[cursorY][cursorX]) {
                        gameOver = true;
                        revealAllMines();
                        timer.stop();
                    } else {
                        revealCell(cursorY, cursorX);
                        if (!won) {
                            won = checkWin();
                            if (won) {
                               timer.stop();
                            }
                        }
                    }
                }
                break;
            case 'f':
            case 'F':
                if (!revealed[cursorY][cursorX]) {
                    flagged[cursorY][cursorX] = !flagged[cursorY][cursorX];
                }
                break;
            case 'h':
            case 'H':
                state = GameState::HELP;
                break;
            case 'n':
            case 'N':
                reset();
                break;
            case 'c':
            case 'C':
                clear();
                break;
            case 'q':
            case 'Q':
                return false;
        }
        return true;
    }
    bool isGameOver() const { return gameOver || won; }
};

int main() {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);  // Hide cursor

    Minesweeper game;
    bool running = true;

    while (running) {
        game.draw();
        int ch = getch();
        running = game.handleInput(ch);
    }

    endwin();
    return 0;
}

