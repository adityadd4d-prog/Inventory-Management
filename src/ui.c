#include "functions.h"

// Define AppState to keep track of UI and Data state
typedef struct {
    Table *tab;
    char default_path[PATH];
    char reports_path[PATH];
    GtkWidget *window;
    GtkWidget *content_area;
    GtkWidget *status_label;
    gboolean is_admin;
} AppState;

// Forward declarations of UI screens
static void show_main_menu(AppState *state);
static void show_items_menu(AppState *state);
static void show_reports_menu(AppState *state);
static void show_file_menu(AppState *state);
static void show_about_screen(AppState *state);

// Helpers
static void clear_content_area(AppState *state) {
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(state->content_area));
    for(iter = children; iter != NULL; iter = g_list_next(iter))
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(children);
}

static void update_status(AppState *state, const char *msg) {
    gtk_label_set_text(GTK_LABEL(state->status_label), msg);
}

static void show_error_dialog(GtkWindow *parent, const char *msg) {
    GtkWidget *dialog = gtk_message_dialog_new(parent, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", msg);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void show_info_dialog(GtkWindow *parent, const char *msg) {
    GtkWidget *dialog = gtk_message_dialog_new(parent, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", msg);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// ---------------------------------------------------------
// Authentication
// ---------------------------------------------------------

static void require_admin_and_show_file_menu(AppState *state) {
    if (state->is_admin) {
        show_file_menu(state);
        return;
    }

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Admin Verify", GTK_WINDOW(state->window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "Cancel", GTK_RESPONSE_CANCEL, "Login", GTK_RESPONSE_ACCEPT, NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);

    GtkWidget *user_label = gtk_label_new("Username:");
    GtkWidget *user_entry = gtk_entry_new();
    GtkWidget *pass_label = gtk_label_new("Password:");
    GtkWidget *pass_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(pass_entry), FALSE);

    gtk_grid_attach(GTK_GRID(grid), user_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), user_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), pass_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), pass_entry, 1, 1, 1, 1);

    gtk_container_add(GTK_CONTAINER(content_area), grid);
    gtk_widget_show_all(dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        const char *user = gtk_entry_get_text(GTK_ENTRY(user_entry));
        const char *pass = gtk_entry_get_text(GTK_ENTRY(pass_entry));
        if (strcmp(user, ADMIN) == 0 && strcmp(pass, PASS) == 0) {
            state->is_admin = TRUE;
            update_status(state, "Admin authenticated.");
            gtk_widget_destroy(dialog);
            show_file_menu(state);
            return;
        } else {
            show_error_dialog(GTK_WINDOW(state->window), "Invalid Credentials!");
        }
    }
    gtk_widget_destroy(dialog);
}

// ---------------------------------------------------------
// File Menu Actions
// ---------------------------------------------------------
static void on_load_file_clicked(GtkWidget *widget, AppState *state) {
    if (state->tab) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(state->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Hash Table Already in Memory. Delete and load a new one?");
        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        if (response == GTK_RESPONSE_YES) {
            DeleteTable(state->tab);
            state->tab = NULL;
        } else {
            return;
        }
    }

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Load File", GTK_WINDOW(state->window), GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), "/home/slayer/Projects/Inventory-Management/data/");

    gint res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        FILE *fp = LoadFile(filename);
        if (fp) {
            state->tab = FillHashTable(fp);
            fclose(fp);
            char msg[256];
            snprintf(msg, sizeof(msg), "Hash Table Created. Items Loaded: %d", state->tab->size);
            show_info_dialog(GTK_WINDOW(state->window), msg);
        } else {
            show_error_dialog(GTK_WINDOW(state->window), "Failed to load file.");
        }
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

static void on_save_file_clicked(GtkWidget *widget, AppState *state) {
    if (!state->tab) {
        show_error_dialog(GTK_WINDOW(state->window), "Hash Table Not Created.");
        return;
    }
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Save File", GTK_WINDOW(state->window), GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT, NULL);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), "/home/slayer/Projects/Inventory-Management/data/");
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "inventory.csv");

    gint res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        WriteFile(state->tab, filename);
        char msg[256];
        snprintf(msg, sizeof(msg), "File Saved at %s. Items Saved: %d", filename, state->tab->size);
        show_info_dialog(GTK_WINDOW(state->window), msg);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

static void on_purge_clicked(GtkWidget *widget, AppState *state) {
    if (!state->tab) {
        show_error_dialog(GTK_WINDOW(state->window), "Hash Table Not Created.");
        return;
    }
    if (!state->tab->dis) {
        show_info_dialog(GTK_WINDOW(state->window), "No Discontinued Items To Purge.");
        return;
    }
    int pur = state->tab->dis;
    state->tab = PurgeTable(state->tab, state->default_path); // requires valid default path
    if (!state->tab) {
        show_error_dialog(GTK_WINDOW(state->window), "Purge/Auto Reload Failed.");
    } else {
        char msg[256];
        snprintf(msg, sizeof(msg), "Purging Completed. Items Purged: %d", pur);
        state->tab->size = state->tab->act;
        state->tab->dis = 0;
        show_info_dialog(GTK_WINDOW(state->window), msg);
    }
}

static void show_file_menu(AppState *state) {
    clear_content_area(state);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(state->content_area), vbox);

    char stats[256];
    snprintf(stats, sizeof(stats), "Total Items: %d | Active: %d | Discontinued: %d", state->tab ? state->tab->size : 0, state->tab ? state->tab->act : 0, state->tab ? state->tab->dis : 0);
    GtkWidget *label = gtk_label_new(stats);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

    GtkWidget *btn_load = gtk_button_new_with_label("Load File");
    GtkWidget *btn_save = gtk_button_new_with_label("Save File");
    GtkWidget *btn_purge = gtk_button_new_with_label("Purge Discontinued Items");
    GtkWidget *btn_back = gtk_button_new_with_label("Back");

    g_signal_connect(btn_load, "clicked", G_CALLBACK(on_load_file_clicked), state);
    g_signal_connect(btn_save, "clicked", G_CALLBACK(on_save_file_clicked), state);
    g_signal_connect(btn_purge, "clicked", G_CALLBACK(on_purge_clicked), state);
    g_signal_connect_swapped(btn_back, "clicked", G_CALLBACK(show_main_menu), state);

    gtk_box_pack_start(GTK_BOX(vbox), btn_load, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_save, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_purge, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_back, FALSE, FALSE, 0);

    gtk_widget_show_all(state->content_area);
}

// ---------------------------------------------------------
// Item Search/Display
// ---------------------------------------------------------
static void show_item_details(AppState *state, Item *it) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Item Details", GTK_WINDOW(state->window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "OK", GTK_RESPONSE_ACCEPT, NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    char details[1024];
    snprintf(details, sizeof(details),
        "Barcode: %s\nName: %s\nRate: %.2f\nStock: %d\nNet Trans: %d\nCapacity: %d\nPercent: %.2f%%\nStatus: %s",
        it->bar, it->name, it->price, it->stock, it->trans, it->cap, it->per, it->status ? "Active" : "Discontinued"
    );
    GtkWidget *label = gtk_label_new(details);
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void on_search_barcode_clicked(GtkWidget *widget, AppState *state) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Search Barcode", GTK_WINDOW(state->window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "Cancel", GTK_RESPONSE_CANCEL, "Search", GTK_RESPONSE_ACCEPT, NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *entry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content_area), entry);
    gtk_widget_show_all(dialog);

    gint res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        const char *bar = gtk_entry_get_text(GTK_ENTRY(entry));
        Item *it = Search((char*)bar, state->tab);
        if (it) {
            show_item_details(state, it);
        } else {
            show_error_dialog(GTK_WINDOW(dialog), "Item Not Found!");
        }
    }
    gtk_widget_destroy(dialog);
}

static void on_search_ocr_clicked(GtkWidget *widget, AppState *state) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select Barcode Image for OCR", GTK_WINDOW(state->window), GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
    gint res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        char *bar = OCR(filename);
        if (bar) {
            Item *it = Search(bar, state->tab);
            if (it) {
                show_item_details(state, it);
            } else {
                show_error_dialog(GTK_WINDOW(dialog), "Item Not Found!");
            }
            free(bar);
        } else {
            show_error_dialog(GTK_WINDOW(dialog), "OCR Recognition Failed!");
        }
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

static void on_add_item_clicked(GtkWidget *widget, AppState *state) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Add Item", GTK_WINDOW(state->window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "Cancel", GTK_RESPONSE_CANCEL, "Add", GTK_RESPONSE_ACCEPT, NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    GtkWidget *e_bar = gtk_entry_new();
    GtkWidget *e_name = gtk_entry_new();
    GtkWidget *e_price = gtk_entry_new();
    GtkWidget *e_stock = gtk_entry_new();
    GtkWidget *e_cap = gtk_entry_new();

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Barcode:"), 0, 0, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_bar, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Name:"), 0, 1, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_name, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Price:"), 0, 2, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_price, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Stock:"), 0, 3, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_stock, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Capacity:"), 0, 4, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_cap, 1, 4, 1, 1);

    gtk_container_add(GTK_CONTAINER(content_area), grid);
    gtk_widget_show_all(dialog);

    while (TRUE) {
        gint res = gtk_dialog_run(GTK_DIALOG(dialog));
        if (res == GTK_RESPONSE_ACCEPT) {
            const char *bar = gtk_entry_get_text(GTK_ENTRY(e_bar));
            const char *name = gtk_entry_get_text(GTK_ENTRY(e_name));
            const char *price_s = gtk_entry_get_text(GTK_ENTRY(e_price));
            const char *stock_s = gtk_entry_get_text(GTK_ENTRY(e_stock));
            const char *cap_s = gtk_entry_get_text(GTK_ENTRY(e_cap));

            if (strlen(bar) != 13) {
                show_error_dialog(GTK_WINDOW(dialog), "Barcode must be 13 digits.");
                continue;
            }
            if (Search((char*)bar, state->tab)) {
                show_error_dialog(GTK_WINDOW(dialog), "Barcode Already in Use.");
                continue;
            }
            if (strlen(name) == 0) {
                show_error_dialog(GTK_WINDOW(dialog), "Name cannot be empty.");
                continue;
            }
            float price = atof(price_s);
            if (price <= 0) {
                show_error_dialog(GTK_WINDOW(dialog), "Price must be positive.");
                continue;
            }
            int stock = atoi(stock_s);
            if (stock < 0) {
                show_error_dialog(GTK_WINDOW(dialog), "Stock cannot be negative.");
                continue;
            }
            int cap = atoi(cap_s);
            if (cap <= 0 || cap <= stock) {
                show_error_dialog(GTK_WINDOW(dialog), "Capacity must be greater than 0 and greater than stock.");
                continue;
            }

            Item *ni = (Item*)malloc(sizeof(Item));
            strcpy(ni->bar, bar);
            strcpy(ni->name, name);
            ni->price = price;
            ni->stock = stock;
            ni->cap = cap;
            ni->trans = 0;
            ni->per = ((float)ni->stock / ni->cap) * 100.0;
            ni->status = 1;
            ni->next = NULL;

            Add(&(state->tab), ni);
            state->tab->size++;
            state->tab->act++;
            show_info_dialog(GTK_WINDOW(state->window), "Item Added Successfully.");
            break;
        } else {
            break;
        }
    }
    gtk_widget_destroy(dialog);
}

static void on_update_item_clicked(GtkWidget *widget, AppState *state) {
    GtkWidget *b_dialog = gtk_dialog_new_with_buttons("Update Item (Barcode)", GTK_WINDOW(state->window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "Cancel", GTK_RESPONSE_CANCEL, "Next", GTK_RESPONSE_ACCEPT, NULL);
    GtkWidget *b_ca = gtk_dialog_get_content_area(GTK_DIALOG(b_dialog));
    GtkWidget *b_entry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(b_ca), gtk_label_new("Enter Barcode:"));
    gtk_container_add(GTK_CONTAINER(b_ca), b_entry);
    gtk_widget_show_all(b_dialog);

    gint b_res = gtk_dialog_run(GTK_DIALOG(b_dialog));
    if (b_res != GTK_RESPONSE_ACCEPT) {
        gtk_widget_destroy(b_dialog);
        return;
    }
    const char *bar = gtk_entry_get_text(GTK_ENTRY(b_entry));
    Item *it = Search((char*)bar, state->tab);
    gtk_widget_destroy(b_dialog);

    if (!it) {
        show_error_dialog(GTK_WINDOW(state->window), "Barcode Not Found!");
        return;
    }
    if (!it->status && !state->is_admin) {
        // Need admin verify
        GtkWidget *dialog = gtk_dialog_new_with_buttons("Admin Verify Required", GTK_WINDOW(state->window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "Cancel", GTK_RESPONSE_CANCEL, "Login", GTK_RESPONSE_ACCEPT, NULL);
        GtkWidget *ca = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        GtkWidget *ue = gtk_entry_new();
        GtkWidget *pe = gtk_entry_new();
        gtk_entry_set_visibility(GTK_ENTRY(pe), FALSE);
        gtk_container_add(GTK_CONTAINER(ca), gtk_label_new("Username:")); gtk_container_add(GTK_CONTAINER(ca), ue);
        gtk_container_add(GTK_CONTAINER(ca), gtk_label_new("Password:")); gtk_container_add(GTK_CONTAINER(ca), pe);
        gtk_widget_show_all(dialog);
        gint r = gtk_dialog_run(GTK_DIALOG(dialog));
        if (r == GTK_RESPONSE_ACCEPT) {
            const char *u = gtk_entry_get_text(GTK_ENTRY(ue));
            const char *p = gtk_entry_get_text(GTK_ENTRY(pe));
            if (strcmp(u, ADMIN) == 0 && strcmp(p, PASS) == 0) {
                state->is_admin = TRUE;
            } else {
                show_error_dialog(GTK_WINDOW(dialog), "Invalid Credentials");
                gtk_widget_destroy(dialog);
                return;
            }
        } else {
            gtk_widget_destroy(dialog);
            return;
        }
        gtk_widget_destroy(dialog);
    }

    // Now update dialog
    GtkWidget *u_dialog = gtk_dialog_new_with_buttons("Update Item", GTK_WINDOW(state->window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "Cancel", GTK_RESPONSE_CANCEL, "Update", GTK_RESPONSE_ACCEPT, NULL);
    GtkWidget *u_ca = gtk_dialog_get_content_area(GTK_DIALOG(u_dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    char sp[32], ss[32], sc[32], sst[32];
    snprintf(sp, sizeof(sp), "%.2f", it->price);
    snprintf(ss, sizeof(ss), "0"); // stock transaction
    snprintf(sc, sizeof(sc), "%d", it->cap);
    snprintf(sst, sizeof(sst), "%d", it->status);

    GtkWidget *e_price = gtk_entry_new(); gtk_entry_set_text(GTK_ENTRY(e_price), sp);
    GtkWidget *e_stock_trans = gtk_entry_new(); gtk_entry_set_text(GTK_ENTRY(e_stock_trans), ss);
    GtkWidget *e_cap = gtk_entry_new(); gtk_entry_set_text(GTK_ENTRY(e_cap), sc);
    GtkWidget *e_status = gtk_entry_new(); gtk_entry_set_text(GTK_ENTRY(e_status), sst);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Price:"), 0, 0, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_price, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new(it->status ? "Stock Transaction (Enter change):" : "Stock Trans:"), 0, 1, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_stock_trans, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Capacity:"), 0, 2, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_cap, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Status (1=Active, 0=Disc):"), 0, 3, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_status, 1, 3, 1, 1);

    gtk_container_add(GTK_CONTAINER(u_ca), grid);
    gtk_widget_show_all(u_dialog);

    while(TRUE) {
        gint r = gtk_dialog_run(GTK_DIALOG(u_dialog));
        if (r == GTK_RESPONSE_ACCEPT) {
            float p = atof(gtk_entry_get_text(GTK_ENTRY(e_price)));
            int trans = atoi(gtk_entry_get_text(GTK_ENTRY(e_stock_trans)));
            int cap = atoi(gtk_entry_get_text(GTK_ENTRY(e_cap)));
            int stat = atoi(gtk_entry_get_text(GTK_ENTRY(e_status)));

            if (p <= 0) { show_error_dialog(GTK_WINDOW(u_dialog), "Price must be positive"); continue; }
            if (trans < -it->stock) { show_error_dialog(GTK_WINDOW(u_dialog), "Cannot withdraw more than stock"); continue; }
            if ((trans + it->stock) > cap) { show_error_dialog(GTK_WINDOW(u_dialog), "Stock exceeds capacity"); continue; }
            if (cap < (it->stock + trans)) { show_error_dialog(GTK_WINDOW(u_dialog), "Capacity too small"); continue; }
            if (stat != 0 && stat != 1) { show_error_dialog(GTK_WINDOW(u_dialog), "Status must be 0 or 1"); continue; }

            if (stat == 0 && it->status == 1) {
                if (!state->is_admin) { show_error_dialog(GTK_WINDOW(u_dialog), "Admin needed to discontinue"); continue; }
                state->tab->dis++; state->tab->act--;
            } else if (stat == 1 && it->status == 0) {
                state->tab->dis--; state->tab->act++;
            }

            it->price = p;
            it->trans += trans;
            it->stock += trans;
            it->cap = cap;
            it->status = stat;
            it->per = ((float)it->stock/it->cap) * 100;

            show_info_dialog(GTK_WINDOW(state->window), "Item Updated Successfully");
            break;
        } else {
            break;
        }
    }
    gtk_widget_destroy(u_dialog);
}

static void show_items_menu(AppState *state) {
    if (!state->tab) {
        show_error_dialog(GTK_WINDOW(state->window), "Hash Table Not Created. Load a file first.");
        return;
    }
    clear_content_area(state);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(state->content_area), vbox);

    GtkWidget *btn_search = gtk_button_new_with_label("Search Barcode");
    GtkWidget *btn_search_ocr = gtk_button_new_with_label("Search OCR");
    GtkWidget *btn_add = gtk_button_new_with_label("Add Item");
    GtkWidget *btn_update = gtk_button_new_with_label("Update Item");
    GtkWidget *btn_back = gtk_button_new_with_label("Back");

    g_signal_connect(btn_search, "clicked", G_CALLBACK(on_search_barcode_clicked), state);
    g_signal_connect(btn_search_ocr, "clicked", G_CALLBACK(on_search_ocr_clicked), state);
    g_signal_connect(btn_add, "clicked", G_CALLBACK(on_add_item_clicked), state);
    g_signal_connect(btn_update, "clicked", G_CALLBACK(on_update_item_clicked), state);
    g_signal_connect_swapped(btn_back, "clicked", G_CALLBACK(show_main_menu), state);

    gtk_box_pack_start(GTK_BOX(vbox), btn_search, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_search_ocr, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_add, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_update, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_back, FALSE, FALSE, 0);

    gtk_widget_show_all(state->content_area);
}

// ---------------------------------------------------------
// Reports Menu
// ---------------------------------------------------------
static void on_report_clicked(GtkWidget *widget, AppState *state, int type) {
    char path[PATH];
    const char *name = "";
    if (type == 1) {
        snprintf(path, PATH, "%sLow_Stock.csv", state->reports_path);
        LowStock(state->tab, path);
        name = "Low Stock";
    } else if (type == 2) {
        snprintf(path, PATH, "%sBestsellers.csv", state->reports_path);
        BestSeller(state->tab, path);
        name = "Bestsellers";
    } else if (type == 3) {
        snprintf(path, PATH, "%sSlow_Stock.csv", state->reports_path);
        SlowStock(state->tab, path);
        name = "Slow Stock";
    }

    char msg[256];
    snprintf(msg, sizeof(msg), "%s Report Created At:\n%s\nOpen in LibreOffice?", name, path);
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(state->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", msg);
    gint res = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    if (res == GTK_RESPONSE_YES) {
        if (LibreOfficeLaunch(path)) {
            show_info_dialog(GTK_WINDOW(state->window), "Launched LibreOffice Successfully");
        } else {
            show_error_dialog(GTK_WINDOW(state->window), "Failed to launch LibreOffice");
        }
    }
}

static void on_low_stock_clicked(GtkWidget *w, AppState *s) { on_report_clicked(w, s, 1); }
static void on_best_seller_clicked(GtkWidget *w, AppState *s) { on_report_clicked(w, s, 2); }
static void on_slow_stock_clicked(GtkWidget *w, AppState *s) { on_report_clicked(w, s, 3); }

static void show_reports_menu_proper(AppState *state) {
    if (!state->tab) {
        show_error_dialog(GTK_WINDOW(state->window), "Hash Table Not Created.");
        return;
    }
    clear_content_area(state);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(state->content_area), vbox);

    GtkWidget *btn_low = gtk_button_new_with_label("Low Stock Items");
    GtkWidget *btn_best = gtk_button_new_with_label("Bestsellers");
    GtkWidget *btn_slow = gtk_button_new_with_label("Slow Stock");
    GtkWidget *btn_back = gtk_button_new_with_label("Back");

    g_signal_connect(btn_low, "clicked", G_CALLBACK(on_low_stock_clicked), state);
    g_signal_connect(btn_best, "clicked", G_CALLBACK(on_best_seller_clicked), state);
    g_signal_connect(btn_slow, "clicked", G_CALLBACK(on_slow_stock_clicked), state);
    g_signal_connect_swapped(btn_back, "clicked", G_CALLBACK(show_main_menu), state);

    gtk_box_pack_start(GTK_BOX(vbox), btn_low, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_best, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_slow, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_back, FALSE, FALSE, 0);

    gtk_widget_show_all(state->content_area);
}


// ---------------------------------------------------------
// Main Menu
// ---------------------------------------------------------
static void show_about_screen(AppState *state) {
    GtkWidget *dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Inventory Management System");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "1.0");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "Developed By: Aditya Patel\nGTK Ported");
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "https://github.com/adityadd4d-prog/Inventory-Management");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void on_exit_clicked(GtkWidget *widget, AppState *state) {
    if (state->tab) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(state->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Hash Table in Memory. Save before exit?");
        gint res = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        if (res == GTK_RESPONSE_YES) {
            // Assume default path
            WriteFile(state->tab, state->default_path);
        }
    }
    gtk_main_quit();
}

static void show_main_menu(AppState *state) {
    clear_content_area(state);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(state->content_area), vbox);

    GtkWidget *label = gtk_label_new("--- Main Menu ---");
    GtkWidget *btn_items = gtk_button_new_with_label("Items");
    GtkWidget *btn_report = gtk_button_new_with_label("Report");
    GtkWidget *btn_file = gtk_button_new_with_label("File");
    GtkWidget *btn_about = gtk_button_new_with_label("About");
    GtkWidget *btn_exit = gtk_button_new_with_label("Exit");

    g_signal_connect_swapped(btn_items, "clicked", G_CALLBACK(show_items_menu), state);
    g_signal_connect_swapped(btn_report, "clicked", G_CALLBACK(show_reports_menu_proper), state);
    g_signal_connect_swapped(btn_file, "clicked", G_CALLBACK(require_admin_and_show_file_menu), state);
    g_signal_connect_swapped(btn_about, "clicked", G_CALLBACK(show_about_screen), state);
    g_signal_connect(btn_exit, "clicked", G_CALLBACK(on_exit_clicked), state);

    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_items, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_report, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_file, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_about, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_exit, FALSE, FALSE, 0);

    gtk_widget_show_all(state->content_area);
}

// ---------------------------------------------------------
// Entry point
// ---------------------------------------------------------
void create_main_window(Table *tab, int argc, char **argv) {
    gtk_init(&argc, &argv);

    AppState *state = g_malloc(sizeof(AppState));
    state->tab = tab;
    state->is_admin = FALSE;
    strcpy(state->default_path, "/home/slayer/Projects/Inventory-Management/data/inventory.csv");
    strcpy(state->reports_path, "/home/slayer/Projects/Inventory-Management/Reports/");

    state->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(state->window), "Inventory Management System");
    gtk_window_set_default_size(GTK_WINDOW(state->window), 600, 400);
    gtk_container_set_border_width(GTK_CONTAINER(state->window), 10);
    g_signal_connect(state->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(state->window), main_vbox);

    state->content_area = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(main_vbox), state->content_area, TRUE, TRUE, 0);

    state->status_label = gtk_label_new("Ready");
    gtk_box_pack_start(GTK_BOX(main_vbox), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox), state->status_label, FALSE, FALSE, 0);

    show_main_menu(state);

    gtk_widget_show_all(state->window);
    gtk_main();

    g_free(state);
}
