#include "gtk_minesweeper.h"
#include <sstream>
#include <iomanip>
#include <random>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cstring>

const char* GTKMinesweeper::FLAG_BASE64 = R"(iVBORw0KGgoAAAANSUhEUgAAACAAAAAgBAMAAACBVGfHAAAABGdBTUEAALGPC/xhBQAAAAFzUkdCAdn
JLH8AAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAAOpgAABdwnLpRPAAAACFQTFRFAAAA/2Z
mAAAA////mTMzzMzMzGZmZjMzmWYzMwAA/8xmaoi8KgAAAAF0Uk5TAEDm2GYAAABdSURBVCjPY2DABpQ
U0AS6GggJKCkooepSVkkUTJnEwMAEE1VWFBQUFHOf7oIiAAIDLaDmXu6GIqAEdPaUREERuACYgeRBZfQ
AGqwCSmixwhQaGhqEIgBUocSAEwAAjwQWTza+izoAAAAASUVORK5CYII=)";

const char* GTKMinesweeper::BOMB_BASE64 = R"(iVBORw0KGgoAAAANSUhEUgAAACAAAAAgBAMAAACBVGfHAAAABGdBTUEAALGPC/xhBQAAAAFzUkdCAd
nJLH8AAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAAOpgAABdwnLpRPAAAACFQTFRFAAAA
gwMTcwoRqBIlgTE0yyk14kFE21NWqnN1z5ye////XulGdQAAAAF0Uk5TAEDm2GYAAADbSURBVCjPbdK9
DoIwEAfwQ/lYKeEBsOEFTF10qm9AQqpxc7GzizAbN1cmeALCU0rLtUVClza/XPMvdwCsrNocfK63UBYI
JNdb5YBmukA+Ec40/4cdpRMI2DK2B49ogDq8RKJkjHgEY6AQGjg3+dEExL4sQLAVL4TYwAMhNSAQkiW4
iu/QsOPQO+hGOA29u3JrS3Zo3+k8tpvHavjMQKUMzQLasSIDJ+odqftYUJD6WFFVoghGiEmy4VMLrxCM
LYx9Yppc2K6rnkZS3u1c8sXk6BRi54Lg1mbtV/gBxfI7i3nTTAoAAAAASUVORK5CYII=)";

const char* GTKMinesweeper::REVEALED_BASE64 = R"(iVBORw0KGgoAAAANSUhEUgAAACAAAAAgAQMAAABJtOi3AAAABGdBTUEAALGPC/xhBQAAAAFzUkdCAdn
JLH8AAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAAOpgAABdwnLpRPAAAAAZQTFRFAAAAIx8
gaVhvIAAAAAF0Uk5TAEDm2GYAAABWSURBVAjXY2DADRpghEITkDBoAxICLDBJ/g9AwoIPSCQcBin72MD
AyPDDgYGJ44cCAwv/DwMGNhDBzP8jgYGx/cMBBobjD4FmFbQDFcvZAAn2BwxEAADI2BObaHDmJQAAAAB
JRU5ErkJggg==)";

const char* GTKMinesweeper::ICON_BASE64 = R"(AAABAAEAICAQAAEABADoAgAAFgAAACgAAAAgAAAAQAAAAAEABAAAAAAAAAIAAAAAAAAAAAAAEAAAABAA
AABKNaUAhl1FAE5gdQDkljwAjXu8AHVy4AAfgN8AsaKMAGmwzgDt6NwAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAmZmZmZmZmZmZmZmZmZmZmZmZd3d3
d3d3d3d3d3d3eZmZcREREQAAACAAACEREReZmREUERBQAABmZmZhMzMzeZdxBgEQUAAAhmZmYXd3EXmXeI
ZmEAVVAIaGZmF3d3F5l3KIghBEBVCGKIZhmZlxeZdxdxEQWZQAhmJmZ3d3EXmXd3dxFFBQUIZmZmcXdxF5
l3d3cRd0d3R5d3InF5cReZd3mXd4SIgABFSEhxFxEXmXl3d3eIiCEzMUiIcXdxF5l3mXmXKIEzMzMUiHF3
cReZczNzM4gjMxEzMVh3FxcXmXF3d3coF3d3czNIAAAAB5l3d3d3gnd3d3czKAUAAAeZeWImB4h3d3d3M4
hQF3EHmXl2ZgcoGZl3mTGIVHd3B5l3hmJ3iCmZmXMyiFCZlweZeYYodygilxETKIhVAUAHmXlyd3eIICeX
GIiIURVVB5l5mZmXdIIndBiISXdERAeZdWZmZRERERIhF3EAAAAHmXZgZmYXERESIgd3VQAAB5l4BoZmd3
cRF3Zmd1AFAAeZeAiYBnd3cRcImAdVSUUHmXiIiIZ3d3EXdoh3UERAB5mYiAiGd5l3F3cnd1VQVQmZl4iI
iHmZmXeZmZdEREVJmZl3d3d3d3d3d3d3d3d3mZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZkA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA==)";


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
    
    // Remove any existing image first
    gtk_button_set_image(GTK_BUTTON(button), nullptr);
    
    // Get and remove any existing label
    GtkWidget *existing_child = gtk_bin_get_child(GTK_BIN(button));
    if (existing_child) {
        gtk_container_remove(GTK_CONTAINER(button), existing_child);
    }
    
    // Create style provider for the button
    GtkCssProvider *button_provider = gtk_css_provider_new();
    GtkStyleContext *button_context = gtk_widget_get_style_context(button);
    
    if(game->revealed[row][col]) {
        // Set the revealed cell background color to match Windows SystemColors::Control
        const char* revealed_bg_css = "button { background: #F0F0F0; border: 1px solid #A0A0A0; }";
        gtk_css_provider_load_from_data(button_provider, revealed_bg_css, -1, NULL);
        gtk_style_context_add_provider(button_context,
                                     GTK_STYLE_PROVIDER(button_provider),
                                     GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        
        if(game->minefield[row][col]) {
            // Show mine
            GtkWidget *image = gtk_image_new_from_pixbuf(bomb_pixbuf);
            gtk_widget_show(image);
            gtk_button_set_image(GTK_BUTTON(button), image);
        } else {
            int count = game->countAdjacentMines(row, col);
            if(count > 0) {
                GtkWidget *label = gtk_label_new(std::to_string(count).c_str());
                
                // Create a CSS provider for the label
                GtkCssProvider *provider = gtk_css_provider_new();
                GtkStyleContext *context = gtk_widget_get_style_context(label);
                
                // Add color based on number like in Windows version
                const char* color;
                switch(count) {
                    case 1: color = "blue"; break;
                    case 2: color = "green"; break;
                    case 3: color = "red"; break;
                    case 4: color = "darkblue"; break;
                    case 5: color = "darkred"; break;
                    default: color = "darkgray"; break;
                }
                
                // Create CSS string and load it
                gchar *css = g_strdup_printf("label { color: %s; }", color);
                gtk_css_provider_load_from_data(provider, css, -1, NULL);
                g_free(css);
                
                // Apply the CSS to the label
                gtk_style_context_add_provider(context,
                                            GTK_STYLE_PROVIDER(provider),
                                            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
                g_object_unref(provider);
                
                gtk_widget_show(label);
                gtk_container_add(GTK_CONTAINER(button), label);
            } else {
                // Show revealed background for empty cells
                GtkWidget *image = gtk_image_new_from_pixbuf(revealed_pixbuf);
                gtk_widget_show(image);
                gtk_button_set_image(GTK_BUTTON(button), image);
            }
        }
    } else {
        // Unrevealed cell style with raised border effect
        const char* unrevealed_bg_css = "button { background: #E0E0E0; border: 2px outset #D4D4D4; }";
        gtk_css_provider_load_from_data(button_provider, unrevealed_bg_css, -1, NULL);
        gtk_style_context_add_provider(button_context,
                                     GTK_STYLE_PROVIDER(button_provider),
                                     GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        
        if(game->flagged[row][col]) {
            // Show flag
            GtkWidget *image = gtk_image_new_from_pixbuf(flag_pixbuf);
            gtk_widget_show(image);
            gtk_button_set_image(GTK_BUTTON(button), image);
        }
    }
    
    g_object_unref(button_provider);
    gtk_widget_show(button);
}

    GdkPixbuf* GTKMinesweeper::load_base64_image(const char* base64_data) {
        std::vector<guchar> decoded_data;
        gsize decoded_len;
        guchar* raw_data = g_base64_decode(base64_data, &decoded_len);
        
        GInputStream* stream = g_memory_input_stream_new_from_data(raw_data, decoded_len, nullptr);
        GError* error = nullptr;
        GdkPixbuf* pixbuf = gdk_pixbuf_new_from_stream(stream, nullptr, &error);
        
        g_object_unref(stream);
        g_free(raw_data);
        
        if (error) {
            g_error_free(error);
            return nullptr;
        }
        
        return pixbuf;
    }
    
    void GTKMinesweeper::load_images() {
        flag_pixbuf = load_base64_image(FLAG_BASE64);
        bomb_pixbuf = load_base64_image(BOMB_BASE64);
        revealed_pixbuf = load_base64_image(REVEALED_BASE64);
        app_icon = load_base64_image(ICON_BASE64);
        
        if (app_icon) {
            gtk_window_set_icon(GTK_WINDOW(window), app_icon);
        }
    }
    
    void GTKMinesweeper::cleanup_images() {
        if (flag_pixbuf) g_object_unref(flag_pixbuf);
        if (bomb_pixbuf) g_object_unref(bomb_pixbuf);
        if (revealed_pixbuf) g_object_unref(revealed_pixbuf);
        if (app_icon) g_object_unref(app_icon);
    }

GTKMinesweeper::GTKMinesweeper() 
    : game(std::make_unique<Minesweeper>())
    , window(nullptr)
    , grid(nullptr)
    , timer_label(nullptr)
    , mines_label(nullptr)
    , menubar(nullptr)
    , timer_id(0)
    , flag_pixbuf(nullptr)
    , bomb_pixbuf(nullptr)
    , revealed_pixbuf(nullptr)
    , app_icon(nullptr) {
    
    game->setDifficulty(Difficulty::EASY);
    load_images();
}

GTKMinesweeper::~GTKMinesweeper() {
    if (timer_id > 0) {
        g_source_remove(timer_id);
        timer_id = 0;
    }
    cleanup_images();
}

void GTKMinesweeper::create_window(GtkApplication *app) {
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Minesweeper");
    
    // Set minimum window size
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 500);
    
    // Create geometry hints struct properly
    GdkGeometry geometry;
    geometry.min_width = 300;
    geometry.min_height = 400;
    gtk_window_set_geometry_hints(GTK_WINDOW(window), NULL,
                                &geometry,
                                GDK_HINT_MIN_SIZE);
    
    // Main vbox for entire window
    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);
    gtk_widget_set_vexpand(main_vbox, TRUE);
    gtk_widget_set_hexpand(main_vbox, TRUE);
    
    // Menu bar (flush with window edges)
    create_menu();
    gtk_box_pack_start(GTK_BOX(main_vbox), menubar, FALSE, FALSE, 0);
    
    // Content box with margins
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox), content_box, TRUE, TRUE, 0);
    gtk_widget_set_margin_start(content_box, 20);   // Left margin
    gtk_widget_set_margin_end(content_box, 20);     // Right margin
    gtk_widget_set_margin_top(content_box, 20);     // Top margin
    gtk_widget_set_margin_bottom(content_box, 20);  // Bottom margin
    
    // Info box (timer and mines counter)
    GtkWidget *info_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(content_box), info_box, FALSE, FALSE, 5);
    
    timer_label = gtk_label_new("Time: 00:00");
    mines_label = gtk_label_new("Mines: 10");
    gtk_box_pack_start(GTK_BOX(info_box), timer_label, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(info_box), mines_label, TRUE, TRUE, 5);
    
    // Game grid
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 1);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 1);
    gtk_box_pack_start(GTK_BOX(content_box), grid, TRUE, TRUE, 5);
    
    initialize_grid();
    
    gtk_widget_show_all(window);
    
    timer_id = g_timeout_add(100, G_SOURCE_FUNC(update_timer), this);
}

void GTKMinesweeper::create_menu() {
    menubar = gtk_menu_bar_new();
    
    // Game menu
    GtkWidget *game_menu = gtk_menu_new();
    GtkWidget *game_item = gtk_menu_item_new_with_label("Game");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(game_item), game_menu);
    
    GtkWidget *new_game = gtk_menu_item_new_with_label("New Game");
    g_signal_connect(G_OBJECT(new_game), "activate", G_CALLBACK(on_new_game), this);
    gtk_menu_shell_append(GTK_MENU_SHELL(game_menu), new_game);
    
    GtkWidget *high_scores = gtk_menu_item_new_with_label("High Scores");
    g_signal_connect(G_OBJECT(high_scores), "activate", G_CALLBACK(on_high_scores), this);
    gtk_menu_shell_append(GTK_MENU_SHELL(game_menu), high_scores);
    
    GtkWidget *quit = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(G_OBJECT(quit), "activate", G_CALLBACK(on_quit), this);
    gtk_menu_shell_append(GTK_MENU_SHELL(game_menu), quit);
    
    // Difficulty menu
    GtkWidget *diff_menu = gtk_menu_new();
    GtkWidget *diff_item = gtk_menu_item_new_with_label("Difficulty");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(diff_item), diff_menu);
    
    GtkWidget *easy = gtk_menu_item_new_with_label("Easy");
    GtkWidget *medium = gtk_menu_item_new_with_label("Medium");
    GtkWidget *hard = gtk_menu_item_new_with_label("Hard");
    
    g_object_set_data(G_OBJECT(easy), "minesweeper", this);
    g_object_set_data(G_OBJECT(medium), "minesweeper", this);
    g_object_set_data(G_OBJECT(hard), "minesweeper", this);
    
    g_signal_connect(G_OBJECT(easy), "activate", G_CALLBACK(on_difficulty), GINT_TO_POINTER(0));
    g_signal_connect(G_OBJECT(medium), "activate", G_CALLBACK(on_difficulty), GINT_TO_POINTER(1));
    g_signal_connect(G_OBJECT(hard), "activate", G_CALLBACK(on_difficulty), GINT_TO_POINTER(2));
    
    gtk_menu_shell_append(GTK_MENU_SHELL(diff_menu), easy);
    gtk_menu_shell_append(GTK_MENU_SHELL(diff_menu), medium);
    gtk_menu_shell_append(GTK_MENU_SHELL(diff_menu), hard);
    
    // Help menu
    GtkWidget *help_menu = gtk_menu_new();
    GtkWidget *help_item = gtk_menu_item_new_with_label("Help");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_item), help_menu);
    
    GtkWidget *how_to_play = gtk_menu_item_new_with_label("How To Play");
    g_signal_connect(G_OBJECT(how_to_play), "activate", G_CALLBACK(on_how_to_play), this);
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), how_to_play);
    
    GtkWidget *about = gtk_menu_item_new_with_label("About");
    g_signal_connect(G_OBJECT(about), "activate", G_CALLBACK(on_about), this);
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about);
    
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), game_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), diff_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help_item);
}

void GTKMinesweeper::initialize_grid() {
    GList *children = gtk_container_get_children(GTK_CONTAINER(grid));
    for(GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    
    // Set grid to expand
    gtk_widget_set_vexpand(grid, TRUE);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    
    buttons.resize(game->height);
    for(int i = 0; i < game->height; i++) {
        buttons[i].resize(game->width);
        for(int j = 0; j < game->width; j++) {
            GtkWidget *button = gtk_button_new();
            
            // Make buttons expand
            gtk_widget_set_vexpand(button, TRUE);
            gtk_widget_set_hexpand(button, TRUE);
            
            // Set minimum size to prevent buttons from becoming too small
            gtk_widget_set_size_request(button, 25, 25);
            
            g_object_set_data(G_OBJECT(button), "row", GINT_TO_POINTER(i));
            g_object_set_data(G_OBJECT(button), "col", GINT_TO_POINTER(j));
            
            g_signal_connect(G_OBJECT(button), "button-press-event",
                           G_CALLBACK(on_button_click), this);
            
            gtk_grid_attach(GTK_GRID(grid), button, j, i, 1, 1);
            buttons[i][j] = button;
        }
    }
    
    gtk_widget_show_all(grid);
}

void GTKMinesweeper::update_mine_counter() {
    int flagged_count = 0;
    for(int i = 0; i < game->height; i++) {
        for(int j = 0; j < game->width; j++) {
            if(game->flagged[i][j]) flagged_count++;
        }
    }
    gtk_label_set_text(GTK_LABEL(mines_label), 
                      ("Mines: " + std::to_string(game->mines - flagged_count)).c_str());
}

void GTKMinesweeper::show_high_scores() {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "High Scores",
        GTK_WINDOW(window),
        GTK_DIALOG_MODAL,
        "_OK",
        GTK_RESPONSE_ACCEPT,
        NULL);

    // Set dialog size
    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 300);
        
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content_area), 10);

    // Create a grid for the table
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    // Create and style header labels
    const char* header_css = "label { font-weight: bold; }";
    GtkCssProvider *header_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(header_provider, header_css, -1, NULL);

    // Add headers
    GtkWidget *name_header = gtk_label_new("Player");
    GtkWidget *time_header = gtk_label_new("Time");
    GtkWidget *diff_header = gtk_label_new("Difficulty");

    // Apply bold style to headers
    for (GtkWidget* header : {name_header, time_header, diff_header}) {
        GtkStyleContext *context = gtk_widget_get_style_context(header);
        gtk_style_context_add_provider(context,
                                     GTK_STYLE_PROVIDER(header_provider),
                                     GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        gtk_widget_set_halign(header, GTK_ALIGN_START);
    }

    // Attach headers
    gtk_grid_attach(GTK_GRID(grid), name_header, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), time_header, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), diff_header, 2, 0, 1, 1);

    // Add separator after headers
    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_attach(GTK_GRID(grid), separator, 0, 1, 3, 1);

    // Add scores
    const auto& scores = game->highscores.getScores();
    int row = 2;  // Start after header and separator
    for (const auto& score : scores) {
        // Create labels for each column
        GtkWidget *name_label = gtk_label_new(score.name.c_str());
        GtkWidget *time_label = gtk_label_new((std::to_string(score.time) + "s").c_str());
        GtkWidget *diff_label = gtk_label_new(score.difficulty.c_str());

        // Align labels to the left
        for (GtkWidget* label : {name_label, time_label, diff_label}) {
            gtk_widget_set_halign(label, GTK_ALIGN_START);
        }

        // Attach labels to grid
        gtk_grid_attach(GTK_GRID(grid), name_label, 0, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), time_label, 1, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), diff_label, 2, row, 1, 1);

        row++;
    }

    g_object_unref(header_provider);

    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Add these helper methods to the Minesweeper class:
int Minesweeper::countAdjacentFlags(int y, int x) {
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

void Minesweeper::revealAdjacent(int y, int x) {
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
        if (checkWin()) {
            won = true;
            timer.stop();
        }
    }
}

// Modify the on_button_click handler:
void GTKMinesweeper::on_button_click(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    GTKMinesweeper *minesweeper = static_cast<GTKMinesweeper*>(user_data);
    int row = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "row"));
    int col = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "col"));
    
    if(event->button == 1) { // Left click
        if(minesweeper->game->firstMove) {
            minesweeper->game->initializeMinefield(row, col, -1);
            minesweeper->game->firstMove = false;
            minesweeper->game->timer.start();
        }
        
        if(minesweeper->game->revealed[row][col]) {
            // If clicking on a revealed number, try to reveal adjacent
            minesweeper->game->revealAdjacent(row, col);
            minesweeper->update_all_cells();
            if(minesweeper->game->gameOver) {
                minesweeper->show_game_over_dialog();
            } else if(minesweeper->game->won) {
                minesweeper->show_win_dialog();
            }
        } else if(!minesweeper->game->flagged[row][col]) {
            if(minesweeper->game->minefield[row][col]) {
                minesweeper->game->gameOver = true;
                minesweeper->game->revealAllMines();
                minesweeper->game->timer.stop();
                minesweeper->update_all_cells();
                while (gtk_events_pending()) {
                    gtk_main_iteration();
                }
                minesweeper->show_game_over_dialog();
            } else {
                minesweeper->game->revealCell(row, col);
                if(minesweeper->game->checkWin()) {
                    minesweeper->game->won = true;
                    minesweeper->game->timer.stop();
                    minesweeper->update_all_cells();
                    while (gtk_events_pending()) {
                        gtk_main_iteration();
                    }
                    minesweeper->show_win_dialog();
                } else {
                    minesweeper->update_all_cells();
                }
            }
        }
    } else if(event->button == 3) { // Right click
        if(!minesweeper->game->revealed[row][col]) {
            minesweeper->game->flagged[row][col] = !minesweeper->game->flagged[row][col];
            minesweeper->update_cell(row, col);
            minesweeper->update_mine_counter();
        }
    }
}

void GTKMinesweeper::on_new_game(GtkWidget *widget, gpointer user_data) {
    (void)widget;  // Unused parameter
    GTKMinesweeper *minesweeper = static_cast<GTKMinesweeper*>(user_data);
    minesweeper->game->reset();
    minesweeper->initialize_grid();
    minesweeper->update_mine_counter();
}

void GTKMinesweeper::on_high_scores(GtkWidget *widget, gpointer user_data) {
    (void)widget;  // Unused parameter
    GTKMinesweeper *minesweeper = static_cast<GTKMinesweeper*>(user_data);
    minesweeper->show_high_scores();
}

void GTKMinesweeper::on_quit(GtkWidget *widget, gpointer user_data) {
    (void)widget;  // Unused parameter
    (void)user_data;  // Unused parameter
    g_application_quit(G_APPLICATION(gtk_window_get_application(GTK_WINDOW(widget))));
}

void GTKMinesweeper::on_difficulty(GtkWidget *widget, gpointer difficulty) {
    GTKMinesweeper *minesweeper = static_cast<GTKMinesweeper*>(
        g_object_get_data(G_OBJECT(widget), "minesweeper"));
    int diff = GPOINTER_TO_INT(difficulty);
    minesweeper->game->setDifficulty(static_cast<Difficulty>(diff));
    minesweeper->initialize_grid();
    minesweeper->update_mine_counter();
}

void GTKMinesweeper::on_how_to_play(GtkWidget *widget, gpointer user_data) {
    (void)widget;  // Unused parameter
    GTKMinesweeper *minesweeper = static_cast<GTKMinesweeper*>(user_data);
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(minesweeper->window),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "How to Play Minesweeper:\n\n"
        "- Left click to reveal a cell\n"
        "- Right click to flag/unflag a cell\n"
        "- Click on revealed number to reveal adjacent cells\n"
        "  if correct number of flags are placed\n"
        "- The goal is to reveal all non-mine cells\n"
        "  without triggering any mines!");
        
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void GTKMinesweeper::on_about(GtkWidget *widget, gpointer user_data) {
    (void)widget;  // Unused parameter
    GTKMinesweeper *minesweeper = static_cast<GTKMinesweeper*>(user_data);
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(minesweeper->window),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Minesweeper\n\n"
        "A classic game where you must avoid the mines and clear the field.\n\n"
        "Left click: Reveal cell\n"
        "Right click: Flag/unflag cell\n"
        "Click on revealed number: Reveal adjacent cells if correct number of flags\n\n"
        "Written by Jason Hall (2025)");
        
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
gboolean GTKMinesweeper::update_timer(gpointer user_data) {
    GTKMinesweeper *minesweeper = static_cast<GTKMinesweeper*>(user_data);
    if(!minesweeper->game->gameOver && !minesweeper->game->won && 
       !minesweeper->game->firstMove) {
        gtk_label_set_text(GTK_LABEL(minesweeper->timer_label), 
                        ("Time: " + minesweeper->game->timer.getTimeString()).c_str());
    }
    return G_SOURCE_CONTINUE;
}

void GTKMinesweeper::activate(GtkApplication *app, gpointer user_data) {
    GTKMinesweeper *minesweeper = static_cast<GTKMinesweeper*>(user_data);
    minesweeper->create_window(app);
}

int GTKMinesweeper::run(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new(
        "org.gtk.minesweeper",
        G_APPLICATION_DEFAULT_FLAGS);
        
    g_signal_connect(app, "activate", G_CALLBACK(activate), this);
    
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}

int main(int argc, char *argv[]) {
    GTKMinesweeper minesweeper;
    return minesweeper.run(argc, argv);
}
