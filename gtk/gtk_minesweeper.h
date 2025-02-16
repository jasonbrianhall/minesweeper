#pragma once
#include <gtk/gtk.h>
#include <memory>
#include <vector>
#include "minesweeper.h"  // Include the base Minesweeper header instead

class GTKMinesweeper {
private:
    std::unique_ptr<Minesweeper> game;
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *timer_label;
    GtkWidget *mines_label;
    std::vector<std::vector<GtkWidget*>> buttons;
    GtkWidget *menubar;
    guint timer_id;
    
    void create_window(GtkApplication *app);
    void create_menu();
    void initialize_grid();
    void update_all_cells();
    void update_cell(int row, int col);
    void update_mine_counter();
    void show_game_over_dialog();
    void show_win_dialog();
    void show_high_scores();

    static void activate(GtkApplication *app, gpointer user_data);
    static void on_button_click(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
    static void on_new_game(GtkWidget *widget, gpointer user_data);
    static void on_high_scores(GtkWidget *widget, gpointer user_data);
    static void on_quit(GtkWidget *widget, gpointer user_data);
    static void on_difficulty(GtkWidget *widget, gpointer difficulty);
    static void on_about(GtkWidget *widget, gpointer user_data);
    static gboolean update_timer(gpointer user_data);

public:
    GTKMinesweeper();
    ~GTKMinesweeper();
    int run(int argc, char *argv[]);
};
