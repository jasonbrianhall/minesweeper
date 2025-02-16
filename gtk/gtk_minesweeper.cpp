#include "gtk_minesweeper.h"
#include <sstream>
#include <iomanip>
#include <random>

GTKMinesweeper::GTKMinesweeper() 
    : game(std::make_unique<Minesweeper>())
    , window(nullptr)
    , grid(nullptr)
    , timer_label(nullptr)
    , mines_label(nullptr)
    , menubar(nullptr)
    , timer_id(0) {
    game->setDifficulty(Difficulty::EASY);
}

GTKMinesweeper::~GTKMinesweeper() {
    if (timer_id > 0) {
        g_source_remove(timer_id);
        timer_id = 0;
    }
}

void GTKMinesweeper::create_window(GtkApplication *app) {
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Minesweeper");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 500);
    
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    create_menu();
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
    
    GtkWidget *info_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), info_box, FALSE, FALSE, 5);
    
    timer_label = gtk_label_new("Time: 00:00");
    mines_label = gtk_label_new("Mines: 10");
    gtk_box_pack_start(GTK_BOX(info_box), timer_label, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(info_box), mines_label, TRUE, TRUE, 5);
    
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 1);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 1);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 5);
    
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
    
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), game_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), diff_item);
}

void GTKMinesweeper::initialize_grid() {
    GList *children = gtk_container_get_children(GTK_CONTAINER(grid));
    for(GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    
    buttons.resize(game->height);
    for(int i = 0; i < game->height; i++) {
        buttons[i].resize(game->width);
        for(int j = 0; j < game->width; j++) {
            GtkWidget *button = gtk_button_new();
            gtk_widget_set_size_request(button, 30, 30);
            
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
        
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    
    GtkWidget *list = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(content_area), list);
    
    const auto& scores = game->highscores.getScores();
    for(const auto& score : scores) {
        std::string text = score.name + " - " + 
                          std::to_string(score.time) + "s - " + 
                          score.difficulty;
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *label = gtk_label_new(text.c_str());
        gtk_container_add(GTK_CONTAINER(row), label);
        gtk_list_box_insert(GTK_LIST_BOX(list), row, -1);
    }
    
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

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
        
        if(!minesweeper->game->flagged[row][col]) {
            if(minesweeper->game->minefield[row][col]) {
                minesweeper->game->gameOver = true;
                minesweeper->game->revealAllMines();
                minesweeper->game->timer.stop();
                minesweeper->show_game_over_dialog();
            } else {
                minesweeper->game->revealCell(row, col);
                if(minesweeper->game->checkWin()) {
                    minesweeper->game->won = true;
                    minesweeper->game->timer.stop();
                    minesweeper->show_win_dialog();
                }
            }
            minesweeper->update_all_cells();
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
        "Written by [Your Name] (2025)");
        
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
