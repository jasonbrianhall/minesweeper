#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "gtk_minesweeper.h"
#include <cstring>
#include <sstream>      // for stringstream
#include <iomanip>      // for setw, setfill
#include <random>       // for mt19937, random_device

/*GTKMinesweeper::GTKMinesweeper() 
    : game(std::make_unique<Minesweeper>())
    , window(nullptr)
    , grid(nullptr)
    , timer_label(nullptr)
    , mines_label(nullptr)
    , menubar(nullptr)
    , timer_id(0) {
    game->setDifficulty(Difficulty::EASY);
}*/

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
Minesweeper::Minesweeper() 
    : height(9)
    , width(9)
    , mines(10)
    , firstMove(true)
    , gameOver(false)
    , won(false) {
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

bool Minesweeper::isHighScore(int time, const std::string& difficulty) {
    return highscores.isHighScore(time, difficulty);
}

void Minesweeper::saveHighscore() {
    std::string difficultyStr;
    switch (width) {
        case 9: difficultyStr = "Easy"; break;
        case 16: difficultyStr = "Medium"; break;
        case 30: difficultyStr = "Hard"; break;
        default: difficultyStr = "Custom"; break;
    }
    
    Score score;
    score.name = playerName;
    score.time = timer.getElapsedSeconds();
    score.difficulty = difficultyStr;
    highscores.addScore(score);
}

// Add these to gtk_minesweeper.cpp:

void GTKMinesweeper::show_game_over_dialog() {
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(window),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Game Over!");
    
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void GTKMinesweeper::show_win_dialog() {
    int time = game->timer.getElapsedSeconds();
    std::string difficulty;
    switch(game->width) {
        case 9: difficulty = "Easy"; break;
        case 16: difficulty = "Medium"; break;
        case 30: difficulty = "Hard"; break;
        default: difficulty = "Custom"; break;
    }
    
    if(game->isHighScore(time, difficulty)) {
        GtkWidget *dialog = gtk_dialog_new_with_buttons(
            "New High Score!",
            GTK_WINDOW(window),
            GTK_DIALOG_MODAL,
            "_OK",
            GTK_RESPONSE_ACCEPT,
            NULL);
            
        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        
        GtkWidget *name_entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Enter your name");
        gtk_container_add(GTK_CONTAINER(content_area), name_entry);
        
        gtk_widget_show_all(dialog);
        
        if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            const gchar *name = gtk_entry_get_text(GTK_ENTRY(name_entry));
            if(name && strlen(name) > 0) {
                game->playerName = name;
                game->saveHighscore();
            }
        }
        
        gtk_widget_destroy(dialog);
        show_high_scores();
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Congratulations! You won!");
            
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

void GTKMinesweeper::update_all_cells() {
    for(int i = 0; i < game->height; i++) {
        for(int j = 0; j < game->width; j++) {
            update_cell(i, j);
        }
    }
}

void GTKMinesweeper::update_cell(int row, int col) {
    GtkWidget *button = buttons[row][col];
    GtkWidget *label = gtk_bin_get_child(GTK_BIN(button));
    if(label != NULL) {
        gtk_widget_destroy(label);
    }
    
    if(game->revealed[row][col]) {
        if(game->minefield[row][col]) {
            GtkWidget *image = gtk_image_new_from_icon_name(
                "dialog-error", GTK_ICON_SIZE_BUTTON);
            gtk_button_set_image(GTK_BUTTON(button), image);
        } else {
            int count = game->countAdjacentMines(row, col);
            if(count > 0) {
                GtkWidget *label = gtk_label_new(
                    std::to_string(count).c_str());
                gtk_container_add(GTK_CONTAINER(button), label);
            }
        }
    } else if(game->flagged[row][col]) {
        GtkWidget *image = gtk_image_new_from_icon_name(
            "emblem-important", GTK_ICON_SIZE_BUTTON);
        gtk_button_set_image(GTK_BUTTON(button), image);
    } else {
        gtk_button_set_image(GTK_BUTTON(button), NULL);
    }
    
    gtk_widget_show_all(button);
}
