#ifndef GUI_HANDLERS_H
#define GUI_HANDLERS_H

#include <gtk/gtk.h>
#include "functions.h"

/* Shared app state passed to every signal handler */
typedef struct {
    GtkBuilder    *builder;
    GtkNotebook   *notebook;
    GtkTreeView   *stock_treeview;
    GtkListStore  *stock_liststore;
    GtkStatusbar  *statusbar;
    guint          statusbar_ctx;
    Table         *table;
    char           csv_path[256];
} AppWidgets;

/* Page index constants — MUST match notebook page order */
#define PAGE_STOCK   0
#define PAGE_ADD     1
#define PAGE_SEARCH  2

/* ── Helper ── */
void refresh_stock_view(AppWidgets *app);
void status_msg(AppWidgets *app, const char *msg);

/* ── Window ── */
void on_main_window_destroy(GtkWidget *w, gpointer d);

/* ── Menu: Inventory ── */
void on_menu_item_view_activate(GtkMenuItem *item, gpointer d);
void on_menu_item_add_activate(GtkMenuItem *item, gpointer d);
void on_menu_item_search_activate(GtkMenuItem *item, gpointer d);
void on_menu_item_delete_activate(GtkMenuItem *item, gpointer d);

/* ── Menu: File ── */
void on_menu_item_load_activate(GtkMenuItem *item, gpointer d);
void on_menu_item_save_activate(GtkMenuItem *item, gpointer d);
void on_menu_item_quit_activate(GtkMenuItem *item, gpointer d);

/* ── Menu: Help ── */
void on_menu_item_about_activate(GtkMenuItem *item, gpointer d);

/* ── Toolbar ── */
void on_tb_add_clicked(GtkToolButton *btn, gpointer d);
void on_tb_search_clicked(GtkToolButton *btn, gpointer d);
void on_tb_save_clicked(GtkToolButton *btn, gpointer d);
void on_tb_refresh_clicked(GtkToolButton *btn, gpointer d);

/* ── Add Item Form ── */
void on_btn_add_item_clicked(GtkButton *btn, gpointer d);
void on_btn_clear_form_clicked(GtkButton *btn, gpointer d);

/* ── Search ── */
void on_btn_search_clicked(GtkButton *btn, gpointer d);
void on_btn_ocr_clicked(GtkButton *btn, gpointer d);

#endif
