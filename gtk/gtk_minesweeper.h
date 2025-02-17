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
    void load_images();
    void cleanup_images();
    GdkPixbuf* load_base64_image(const char* base64_data);
    static void activate(GtkApplication *app, gpointer user_data);
    static void on_button_click(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
    static void on_new_game(GtkWidget *widget, gpointer user_data);
    static void on_reset_game(GtkWidget *widget, gpointer user_data);
    static void on_high_scores(GtkWidget *widget, gpointer user_data);
    static void on_quit(GtkWidget *widget, gpointer user_data);
    static void on_difficulty(GtkWidget *widget, gpointer difficulty);
    static void on_about(GtkWidget *widget, gpointer user_data);
    static void on_how_to_play(GtkWidget *widget, gpointer user_data);
    static void on_set_seed(GtkWidget *widget, gpointer user_data);
    static void on_entry_activate(GtkEntry *entry, gpointer user_data);
    void show_seed_dialog();
    static gboolean update_timer(gpointer user_data);

    GdkPixbuf *flag_pixbuf;
    GdkPixbuf *bomb_pixbuf;
    GdkPixbuf *revealed_pixbuf;
    GdkPixbuf *app_icon;

    // Base64 encoded PNG data
    static const char* FLAG_BASE64;
    static const char* BOMB_BASE64;
    static const char* REVEALED_BASE64;
    static const char* ICON_BASE64;


public:
    GTKMinesweeper();
    ~GTKMinesweeper();
    int run(int argc, char *argv[]);
};
