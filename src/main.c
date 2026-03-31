#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "gui_handlers.h"
#include "functions.h"

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget  *window;
    GError     *error = NULL;

    gtk_init(&argc, &argv);

    /* 1. Load Glade UI */
    builder = gtk_builder_new();
    if (!gtk_builder_add_from_file(builder, "ui/inventory.ui", &error)) {
        g_printerr("Failed to load UI: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    /* 2. Set up shared app state */
    AppWidgets *app = g_new0(AppWidgets, 1);
    app->builder          = builder;
    app->notebook         = GTK_NOTEBOOK(gtk_builder_get_object(builder, "main_notebook"));
    app->stock_treeview   = GTK_TREE_VIEW(gtk_builder_get_object(builder, "stock_treeview"));
    app->stock_liststore  = GTK_LIST_STORE(gtk_builder_get_object(builder, "stock_liststore"));
    app->statusbar        = GTK_STATUSBAR(gtk_builder_get_object(builder, "statusbar"));
    app->statusbar_ctx    = gtk_statusbar_get_context_id(app->statusbar, "main");
    app->table            = NULL;
    strncpy(app->csv_path, "data/inventory.csv", sizeof(app->csv_path) - 1);

    /* 3. Auto-connect all signals from Glade */
    gtk_builder_connect_signals(builder, app);

    /* 4. Show window */
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_widget_show_all(window);

    /* 5. Auto-load CSV if it exists */
    FILE *fp = fopen(app->csv_path, "r");
    if (fp) {
        app->table = FillHashTable(fp);
        fclose(fp);
        refresh_stock_view(app);
    } else {
        status_msg(app, "No CSV loaded. Use File → Load CSV to get started.");
    }

    gtk_main();

    g_free(app);
    g_object_unref(builder);
    return 0;
}
