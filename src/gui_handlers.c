#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include "gui_handlers.h"
#include "functions.h"

/* ─────────────────────────────────────────
   HELPERS
───────────────────────────────────────── */

void status_msg(AppWidgets *app, const char *msg) {
    gtk_statusbar_pop(app->statusbar, app->statusbar_ctx);
    gtk_statusbar_push(app->statusbar, app->statusbar_ctx, msg);
}

/* Populate GtkListStore from your hash table */
void refresh_stock_view(AppWidgets *app) {
    gtk_list_store_clear(app->stock_liststore);
    if (!app->table) return;

    GtkTreeIter iter;
    int i;
    for (i = 0; i < app->table->cap; i++) {
        Item *temp = app->table->buckets[i];
        while (temp) {
            char price_str[32], per_str[32];
            snprintf(price_str, sizeof(price_str), "%.2f", temp->price);
            snprintf(per_str,   sizeof(per_str),   "%.1f%%", temp->per);

            gtk_list_store_append(app->stock_liststore, &iter);
            gtk_list_store_set(app->stock_liststore, &iter,
                0, temp->bar,
                1, temp->name,
                2, price_str,
                3, temp->stock,
                4, temp->trans,
                5, temp->cap,
                6, per_str,
                7, temp->status ? "Active" : "Discontinued",
                -1);
            temp = temp->next;
        }
    }

    /* Update status label */
    char msg[64];
    snprintf(msg, sizeof(msg), "Loaded %d items", app->table->size);
    GtkLabel *lbl = GTK_LABEL(gtk_builder_get_object(app->builder, "stock_status_label"));
    gtk_label_set_text(lbl, msg);
    status_msg(app, msg);
}

/* ─────────────────────────────────────────
   WINDOW
───────────────────────────────────────── */

void on_main_window_destroy(GtkWidget *w, gpointer d) {
    gtk_main_quit();
}

/* ─────────────────────────────────────────
   MENU: INVENTORY
───────────────────────────────────────── */

void on_menu_item_view_activate(GtkMenuItem *item, gpointer d) {
    AppWidgets *app = (AppWidgets *)d;
    gtk_notebook_set_current_page(app->notebook, PAGE_STOCK);
    refresh_stock_view(app);
}

void on_menu_item_add_activate(GtkMenuItem *item, gpointer d) {
    AppWidgets *app = (AppWidgets *)d;
    gtk_notebook_set_current_page(app->notebook, PAGE_ADD);
    status_msg(app, "Fill in the form to add a new item.");
}

void on_menu_item_search_activate(GtkMenuItem *item, gpointer d) {
    AppWidgets *app = (AppWidgets *)d;
    gtk_notebook_set_current_page(app->notebook, PAGE_SEARCH);
    status_msg(app, "Enter a barcode to search.");
}

void on_menu_item_delete_activate(GtkMenuItem *item, gpointer d) {
    AppWidgets *app = (AppWidgets *)d;

    /* Get selected row from treeview */
    GtkTreeSelection *sel = gtk_tree_view_get_selection(app->stock_treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    if (!gtk_tree_selection_get_selected(sel, &model, &iter)) {
        status_msg(app, "Select an item first.");
        return;
    }

    gchar *barcode;
    gtk_tree_model_get(model, &iter, 0, &barcode, -1);

    /* Confirm dialog */
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(gtk_builder_get_object(app->builder, "main_window")),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "Discontinue item with barcode: %s?", barcode
    );
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if (response == GTK_RESPONSE_YES) {
        /* Mark as discontinued — status = 0 */
        Item *found = Search(barcode, app->table);
        if (found) {
            found->status = 0;
            refresh_stock_view(app);
            status_msg(app, "Item discontinued.");
        }
    }
    g_free(barcode);
}

/* ─────────────────────────────────────────
   MENU: FILE
───────────────────────────────────────── */

void on_menu_item_load_activate(GtkMenuItem *item, gpointer d) {
    AppWidgets *app = (AppWidgets *)d;

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Open CSV File",
        GTK_WINDOW(gtk_builder_get_object(app->builder, "main_window")),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open",   GTK_RESPONSE_ACCEPT,
        NULL
    );

    /* Filter to CSV only */
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "CSV files");
    gtk_file_filter_add_pattern(filter, "*.csv");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        strncpy(app->csv_path, path, sizeof(app->csv_path) - 1);
        g_free(path);

        FILE *fp = fopen(app->csv_path, "r");
        if (fp) {
            app->table = FillHashTable(fp);
            fclose(fp);
            refresh_stock_view(app);
            status_msg(app, "File loaded successfully.");
        } else {
            status_msg(app, "ERROR: Could not open file.");
        }
    }
    gtk_widget_destroy(dialog);
}

void on_menu_item_save_activate(GtkMenuItem *item, gpointer d) {
    AppWidgets *app = (AppWidgets *)d;
    if (!app->table) { status_msg(app, "No data to save."); return; }

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Save CSV File",
        GTK_WINDOW(gtk_builder_get_object(app->builder, "main_window")),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Save",   GTK_RESPONSE_ACCEPT,
        NULL
    );
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        WriteFile(app->table, path);
        g_free(path);
        status_msg(app, "File saved successfully.");
    }
    gtk_widget_destroy(dialog);
}

void on_menu_item_quit_activate(GtkMenuItem *item, gpointer d) {
    gtk_main_quit();
}

void on_menu_item_about_activate(GtkMenuItem *item, gpointer d) {
    AppWidgets *app = (AppWidgets *)d;
    GtkWidget *dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Inventory Management System");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "1.0");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "Hash table-based inventory manager.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/* ─────────────────────────────────────────
   TOOLBAR
───────────────────────────────────────── */

void on_tb_add_clicked(GtkToolButton *btn, gpointer d) {
    on_menu_item_add_activate(NULL, d);
}
void on_tb_search_clicked(GtkToolButton *btn, gpointer d) {
    on_menu_item_search_activate(NULL, d);
}
void on_tb_save_clicked(GtkToolButton *btn, gpointer d) {
    on_menu_item_save_activate(NULL, d);
}
void on_tb_refresh_clicked(GtkToolButton *btn, gpointer d) {
    AppWidgets *app = (AppWidgets *)d;
    refresh_stock_view(app);
    status_msg(app, "View refreshed.");
}

/* ─────────────────────────────────────────
   ADD ITEM FORM
───────────────────────────────────────── */

void on_btn_add_item_clicked(GtkButton *btn, gpointer d) {
    AppWidgets *app = (AppWidgets *)d;

    /* Read from form widgets */
    GtkEntry  *e_bar   = GTK_ENTRY(gtk_builder_get_object(app->builder, "entry_bar"));
    GtkEntry  *e_name  = GTK_ENTRY(gtk_builder_get_object(app->builder, "entry_name"));
    GtkEntry  *e_price = GTK_ENTRY(gtk_builder_get_object(app->builder, "entry_price"));
    GtkSpinButton *s_stock = GTK_SPIN_BUTTON(gtk_builder_get_object(app->builder, "spin_stock"));
    GtkSpinButton *s_cap   = GTK_SPIN_BUTTON(gtk_builder_get_object(app->builder, "spin_cap"));
    GtkSpinButton *s_trans = GTK_SPIN_BUTTON(gtk_builder_get_object(app->builder, "spin_trans"));
    GtkComboBoxText *c_status = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(app->builder, "combo_status"));

    const char *bar   = gtk_entry_get_text(e_bar);
    const char *name  = gtk_entry_get_text(e_name);
    const char *price = gtk_entry_get_text(e_price);
    int stock = (int)gtk_spin_button_get_value(s_stock);
    int cap   = (int)gtk_spin_button_get_value(s_cap);
    int trans = (int)gtk_spin_button_get_value(s_trans);
    int status = (gtk_combo_box_get_active(GTK_COMBO_BOX(c_status)) == 0) ? 1 : 0;

    /* Basic validation */
    if (strlen(bar) == 0 || strlen(name) == 0) {
        status_msg(app, "ERROR: Barcode and Name are required.");
        return;
    }
    if (!app->table) {
        status_msg(app, "ERROR: Load a CSV file first.");
        return;
    }

    /* Build Item manually */
    Item *ni = (Item *)malloc(sizeof(Item));
    if (!ni) { status_msg(app, "ERROR: Memory allocation failed."); return; }
    strncpy(ni->bar,  bar,  BAR - 1);  ni->bar[BAR - 1]   = '\0';
    strncpy(ni->name, name, STR - 1);  ni->name[STR - 1]  = '\0';
    ni->price  = (float)atof(price);
    ni->stock  = stock;
    ni->cap    = cap;
    ni->trans  = trans;
    ni->per    = (cap > 0) ? ((double)stock / cap) * 100.0 : 0.0;
    ni->status = status;
    ni->next   = NULL;

    Add(&app->table, ni);
    app->table->size++;

    refresh_stock_view(app);
    status_msg(app, "Item added successfully.");

    /* Switch to stock view */
    gtk_notebook_set_current_page(app->notebook, PAGE_STOCK);
}

void on_btn_clear_form_clicked(GtkButton *btn, gpointer d) {
    AppWidgets *app = (AppWidgets *)d;
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(app->builder, "entry_bar")),   "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(app->builder, "entry_name")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(app->builder, "entry_price")), "");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gtk_builder_get_object(app->builder, "spin_stock")), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gtk_builder_get_object(app->builder, "spin_cap")),   0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gtk_builder_get_object(app->builder, "spin_trans")), 0);
    status_msg(app, "Form cleared.");
}

/* ─────────────────────────────────────────
   SEARCH — calls your Search() from hashTable.c
───────────────────────────────────────── */

static void display_search_result(AppWidgets *app, Item *found) {
    char buf[64];
    #define SET_LBL(id, txt) \
        gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(app->builder, id)), txt)

    if (!found) {
        SET_LBL("lbl_res_bar",    "—");
        SET_LBL("lbl_res_name",   "Not found");
        SET_LBL("lbl_res_price",  "—");
        SET_LBL("lbl_res_stock",  "—");
        SET_LBL("lbl_res_trans",  "—");
        SET_LBL("lbl_res_cap",    "—");
        SET_LBL("lbl_res_per",    "—");
        SET_LBL("lbl_res_status", "—");
        status_msg(app, "Item not found.");
        return;
    }

    SET_LBL("lbl_res_bar",  found->bar);
    SET_LBL("lbl_res_name", found->name);
    snprintf(buf, sizeof(buf), "%.2f", found->price);
    SET_LBL("lbl_res_price", buf);
    snprintf(buf, sizeof(buf), "%d", found->stock);
    SET_LBL("lbl_res_stock", buf);
    snprintf(buf, sizeof(buf), "%d", found->trans);
    SET_LBL("lbl_res_trans", buf);
    snprintf(buf, sizeof(buf), "%d", found->cap);
    SET_LBL("lbl_res_cap", buf);
    snprintf(buf, sizeof(buf), "%.1f%%", found->per);
    SET_LBL("lbl_res_per", buf);
    SET_LBL("lbl_res_status", found->status ? "Active" : "Discontinued");
    status_msg(app, "Item found.");
    #undef SET_LBL
}

void on_btn_search_clicked(GtkButton *btn, gpointer d) {
    AppWidgets *app = (AppWidgets *)d;
    if (!app->table) { status_msg(app, "ERROR: No data loaded."); return; }

    GtkEntry *entry = GTK_ENTRY(gtk_builder_get_object(app->builder, "entry_search_bar"));
    const char *barcode = gtk_entry_get_text(entry);
    if (strlen(barcode) == 0) { status_msg(app, "Enter a barcode."); return; }

    Item *found = Search((char *)barcode, app->table);
    display_search_result(app, found);
}

void on_btn_ocr_clicked(GtkButton *btn, gpointer d) {
    AppWidgets *app = (AppWidgets *)d;

    /* Open file chooser for image */
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Select Barcode Image",
        GTK_WINDOW(gtk_builder_get_object(app->builder, "main_window")),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Scan",   GTK_RESPONSE_ACCEPT,
        NULL
    );
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Images");
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_filter_add_pattern(filter, "*.jpeg");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        char *barcode = OCR(path);  /* calls your OCR() from hashTable.c */
        g_free(path);

        if (barcode) {
            /* Strip newline */
            barcode[strcspn(barcode, "\n")] = '\0';
            GtkEntry *e = GTK_ENTRY(gtk_builder_get_object(app->builder, "entry_search_bar"));
            gtk_entry_set_text(e, barcode);
            if (app->table) {
                Item *found = Search(barcode, app->table);
                display_search_result(app, found);
            }
            free(barcode);
        } else {
            status_msg(app, "OCR scan failed.");
        }
    }
    gtk_widget_destroy(dialog);
}
