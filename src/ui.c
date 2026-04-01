#include "functions.h"

Table *g_tab       = NULL;
char   g_file[STR] = "inventory.csv";

/* ──────────────────────── window helpers ─────────────────────────── */

WINDOW *make_win(int h, int w, int y, int x, const char *title)
{
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);
    if (title && title[0]) {
        int tx = (w - (int)strlen(title) - 2) / 2;
        wattron(win, A_BOLD | COLOR_PAIR(CP_TITLE));
        mvwprintw(win, 0, tx < 1 ? 1 : tx, " %s ", title);
        wattroff(win, A_BOLD | COLOR_PAIR(CP_TITLE));
    }
    keypad(win, TRUE);
    return win;
}

void close_win(WINDOW *win)
{
    delwin(win);
    touchwin(stdscr);
    refresh();
}

void alert(const char *msg, int err)
{
    int R, C;
    getmaxyx(stdscr, R, C);
    int w = (int)strlen(msg) + 12;
    if (w < 30) w = 30;
    if (w > C - 4) w = C - 4;
    WINDOW *win = make_win(7, w, (R - 7) / 2, (C - w) / 2,
                           err ? " Error " : " Info ");
    wattron(win, COLOR_PAIR(err ? CP_ERR : CP_OK) | A_BOLD);
    mvwprintw(win, 2, 4, "%s  %-*.*s",
              err ? "\xe2\x9c\x96" : "\xe2\x9c\x94",
              w - 12, w - 12, msg);
    wattroff(win, COLOR_PAIR(err ? CP_ERR : CP_OK) | A_BOLD);
    int bx = (w - 10) / 2;
    wattron(win, COLOR_PAIR(CP_SEL) | A_BOLD);
    mvwprintw(win, 4, bx, "  [ OK ]  ");
    wattroff(win, COLOR_PAIR(CP_SEL) | A_BOLD);
    wrefresh(win);
    wgetch(win);
    close_win(win);
}

void draw_bg(void)
{
    int R, C;
    getmaxyx(stdscr, R, C);
    clear();

    const char *title = "INVENTORY MANAGEMENT SYSTEM";
    const char *sub   = "Hash Table  |  Data Structures Project";
    int bw = (int)strlen(title) + 8;
    int bx = (C - bw) / 2;
    wattron(stdscr, COLOR_PAIR(CP_TITLE) | A_BOLD);
    mvhline(0, bx, ACS_HLINE, bw);
    mvprintw(1, (C - (int)strlen(title)) / 2, "%s", title);
    mvhline(3, bx, ACS_HLINE, bw);
    wattroff(stdscr, COLOR_PAIR(CP_TITLE) | A_BOLD);
    wattron(stdscr, COLOR_PAIR(CP_HINT));
    mvprintw(2, (C - (int)strlen(sub)) / 2, "%s", sub);
    wattroff(stdscr, COLOR_PAIR(CP_HINT));

    char lft[160], rgt[128];
    snprintf(lft, sizeof(lft), "  File: %s", g_file);
    snprintf(rgt, sizeof(rgt),
             " Items: %-5d  [\xe2\x86\x91\xe2\x86\x93] Nav  "
             "[\xe2\x86\xb5] Select  [q] Back  ",
             g_tab ? g_tab->size : 0);
    wattron(stdscr, COLOR_PAIR(CP_BAR) | A_BOLD);
    mvhline(R - 1, 0, ' ', C);
    mvprintw(R - 1, 0, "%s", lft);
    int rx = C - (int)strlen(rgt);
    if (rx > (int)strlen(lft) + 2) mvprintw(R - 1, rx, "%s", rgt);
    wattroff(stdscr, COLOR_PAIR(CP_BAR) | A_BOLD);
    refresh();
}

/* ──────────────────────── menu runner ────────────────────────────── */

void render_menu(WINDOW *win, const char **items, const char **descs,
                 int n, int sel)
{
    int i, inner = MENU_W - 4;
    for (i = 0; i < n; i++) {
        char line[56];
        snprintf(line, sizeof(line), " %d  %s", i + 1, items[i]);
        if (i == sel) {
            wattron(win, A_BOLD | COLOR_PAIR(CP_SEL));
            mvwprintw(win, SY + i, 2, "\xe2\x96\xb6%-*s", inner - 1, line);
            wattroff(win, A_BOLD | COLOR_PAIR(CP_SEL));
        } else {
            wattron(win, COLOR_PAIR(CP_DEF));
            mvwprintw(win, SY + i, 2, " %-*s", inner - 1, line);
            wattroff(win, COLOR_PAIR(CP_DEF));
        }
    }
    mvwhline(win, SY + n, 1, ACS_HLINE, MENU_W - 2);
    wattron(win, COLOR_PAIR(CP_HINT));
    mvwprintw(win, SY + n + 1, 2, "%-*.*s", inner, inner,
              descs && descs[sel] ? descs[sel] : "");
    wattroff(win, COLOR_PAIR(CP_HINT));
    wattron(win, COLOR_PAIR(CP_DEF));
    mvwprintw(win, SY + n + 2, 2, "%-*s", inner,
              "[\xe2\x86\x91\xe2\x86\x93] Nav  [\xe2\x86\xb5] Select  [q] Back");
    wattroff(win, COLOR_PAIR(CP_DEF));
    wrefresh(win);
}

int nav_menu(WINDOW *win, const char **items, const char **descs, int n)
{
    int sel = 0, ch;
    render_menu(win, items, descs, n, sel);
    while (1) {
        ch = wgetch(win);
        if (ch >= '1' && ch <= '0' + n) return ch - '1';
        switch (ch) {
            case KEY_UP:   sel = (sel - 1 + n) % n; break;
            case KEY_DOWN: sel = (sel + 1) % n;      break;
            case ENTER: case KEY_ENTER: return sel;
            case ESC: case 'q': case 'Q': return -1;
        }
        render_menu(win, items, descs, n, sel);
    }
}

void redraw_win(WINDOW *win, const char *title)
{
    wclear(win);
    box(win, 0, 0);
    wattron(win, A_BOLD | COLOR_PAIR(CP_TITLE));
    mvwprintw(win, 0, (MENU_W - (int)strlen(title) - 2) / 2, " %s ", title);
    wattroff(win, A_BOLD | COLOR_PAIR(CP_TITLE));
    draw_bg();
    touchwin(win);
    wrefresh(win);
}

/* ──────────────────────── input helpers ──────────────────────────── */

void rstr(WINDOW *win, int y, int x, char *buf, int max)
{
    echo();
    curs_set(1);
    wattron(win, COLOR_PAIR(CP_OK));
    mvwgetnstr(win, y, x, buf, max - 1);
    wattroff(win, COLOR_PAIR(CP_OK));
    noecho();
    curs_set(0);
}

/* ──────────────────────── item display ───────────────────────────── */

void render_item(WINDOW *win, Item *it, int y)
{
    int pair = it->status ? CP_OK : CP_ERR;
    wattron(win, COLOR_PAIR(pair));
    mvwprintw(win, y + 0, 3, "Barcode  : %-14s", it->bar);
    mvwprintw(win, y + 1, 3, "Name     : %-36s", it->name);
    mvwprintw(win, y + 2, 3, "Price    : Rs. %.2f", it->price);
    mvwprintw(win, y + 3, 3, "Stock    : %d / %d  (%.1f%%)", it->stock, it->cap, it->per);
    mvwprintw(win, y + 4, 3, "Trans    : %d", it->trans);
    mvwprintw(win, y + 5, 3, "Status   : %s", it->status ? "Active" : "Discontinued");
    wattroff(win, COLOR_PAIR(pair));
}

/* ──────────────────────── item screens ───────────────────────────── */

void scr_search(void)
{
    if (!g_tab) { alert("Load a file first.", 1); return; }
    int R, C;
    getmaxyx(stdscr, R, C);
    WINDOW *win = make_win(14, 58, (R - 14) / 2, (C - 58) / 2, "Search Item");
    mvwprintw(win, 2, 3, "Barcode : ");
    wrefresh(win);
    char key[BAR] = {0};
    rstr(win, 2, 13, key, BAR);
    Item *it = Search(key, g_tab);
    if (!it) {
        close_win(win);
        draw_bg();
        alert("Barcode not found.", 1);
    } else {
        render_item(win, it, 4);
        mvwprintw(win, 12, (58 - 18) / 2, "[ Press any key ]");
        wrefresh(win);
        wgetch(win);
        close_win(win);
        draw_bg();
    }
}

void scr_add(void)
{
    if (!g_tab) { alert("Load a file first.", 1); return; }
    int R, C;
    getmaxyx(stdscr, R, C);
    WINDOW *win = make_win(18, 58, (R - 18) / 2, (C - 58) / 2, "Add New Item");

    char bar[BAR] = {0};
    mvwprintw(win, 2, 3, "Barcode  : ");
    wrefresh(win);
    rstr(win, 2, 14, bar, BAR);
    if (!bar[0]) { close_win(win); draw_bg(); return; }
    if (Search(bar, g_tab)) {
        close_win(win);
        draw_bg();
        alert("Barcode already exists.", 1);
        return;
    }

    char name[STR]={0}, ptmp[16]={0}, stmp[16]={0}, ctmp[16]={0};
    mvwprintw(win, 3, 3, "Name     : "); wrefresh(win); rstr(win, 3, 14, name, STR);
    mvwprintw(win, 4, 3, "Price    : "); wrefresh(win); rstr(win, 4, 14, ptmp, 16);
    mvwprintw(win, 5, 3, "Stock    : "); wrefresh(win); rstr(win, 5, 14, stmp, 16);
    mvwprintw(win, 6, 3, "Capacity : "); wrefresh(win); rstr(win, 6, 14, ctmp, 16);

    Item *ni = (Item *)malloc(sizeof(Item));
    if (!ni) { close_win(win); draw_bg(); alert("Memory error.", 1); return; }
    strcpy(ni->bar,  bar);
    strcpy(ni->name, name[0] ? name : "Unnamed");
    ni->price  = (float)atof(ptmp);
    ni->stock  = atoi(stmp);
    ni->cap    = atoi(ctmp);
    ni->trans  = 0;
    ni->per    = ni->cap > 0 ? (ni->stock * 100.0) / ni->cap : 0.0;
    ni->status = 1;
    ni->next   = NULL;
    Add(&g_tab, ni);
    g_tab->size++;

    wattron(win, COLOR_PAIR(CP_OK) | A_BOLD);
    mvwprintw(win, 9, 3, "Item added successfully.");
    wattroff(win, COLOR_PAIR(CP_OK) | A_BOLD);
    mvwprintw(win, 11, (58 - 18) / 2, "[ Press any key ]");
    wrefresh(win);
    wgetch(win);
    close_win(win);
    draw_bg();
}

void scr_edit(void)
{
    if (!g_tab) { alert("Load a file first.", 1); return; }
    int R, C;
    getmaxyx(stdscr, R, C);
    WINDOW *win = make_win(18, 70, (R - 18) / 2, (C - 70) / 2, "Edit Item");

    char key[BAR] = {0};
    mvwprintw(win, 2, 3, "Barcode  : ");
    wrefresh(win);
    rstr(win, 2, 14, key, BAR);
    Item *it = Search(key, g_tab);
    if (!it) {
        close_win(win);
        draw_bg();
        alert("Item not found.", 1);
        return;
    }

    wclear(win); box(win, 0, 0);
    wattron(win, A_BOLD | COLOR_PAIR(CP_TITLE));
    mvwprintw(win, 0, (70 - 11) / 2, " Edit Item ");
    wattroff(win, A_BOLD | COLOR_PAIR(CP_TITLE));
    wattron(win, COLOR_PAIR(CP_DEF));
    mvwprintw(win, 2, 3, "(Leave blank to keep current)");
    wattroff(win, COLOR_PAIR(CP_DEF));
    wattron(win, COLOR_PAIR(CP_SEL) | A_BOLD);
    mvwprintw(win, 3, 3, "  %-16s %-26s New Value", "Field", "Current");
    wattroff(win, COLOR_PAIR(CP_SEL) | A_BOLD);
    mvwhline(win, 4, 2, ACS_HLINE, 66);
    wrefresh(win);

    int row = 5;
    char nname[STR]={0}, nprice[16]={0}, nstock[16]={0}, ncap[16]={0};
    mvwprintw(win, row, 3, "  %-16s %-26.26s", "Name",     it->name);
    wrefresh(win); rstr(win, row, 47, nname,  STR); row++;
    mvwprintw(win, row, 3, "  %-16s %-26.2f",  "Price",    it->price);
    wrefresh(win); rstr(win, row, 47, nprice, 16);  row++;
    mvwprintw(win, row, 3, "  %-16s %-26d",    "Stock",    it->stock);
    wrefresh(win); rstr(win, row, 47, nstock, 16);  row++;
    mvwprintw(win, row, 3, "  %-16s %-26d",    "Capacity", it->cap);
    wrefresh(win); rstr(win, row, 47, ncap,   16);  row++;

    if (nname[0])  strncpy(it->name, nname, STR - 1);
    if (nprice[0]) it->price = (float)atof(nprice);
    if (nstock[0]) it->stock = atoi(nstock);
    if (ncap[0])   it->cap   = atoi(ncap);
    it->per = it->cap > 0 ? (it->stock * 100.0) / it->cap : 0.0;

    wattron(win, COLOR_PAIR(CP_OK) | A_BOLD);
    mvwprintw(win, row + 1, 3, "Item updated successfully.");
    wattroff(win, COLOR_PAIR(CP_OK) | A_BOLD);
    mvwprintw(win, row + 3, (70 - 18) / 2, "[ Press any key ]");
    wrefresh(win);
    wgetch(win);
    close_win(win);
    draw_bg();
}

void scr_remove(void)
{
    if (!g_tab) { alert("Load a file first.", 1); return; }
    int R, C;
    getmaxyx(stdscr, R, C);
    WINDOW *win = make_win(16, 58, (R - 16) / 2, (C - 58) / 2, "Remove Item");

    char key[BAR] = {0};
    mvwprintw(win, 2, 3, "Barcode : ");
    wrefresh(win);
    rstr(win, 2, 13, key, BAR);
    Item *it = Search(key, g_tab);
    if (!it) {
        close_win(win);
        draw_bg();
        alert("Item not found.", 1);
        return;
    }
    render_item(win, it, 4);
    wattron(win, COLOR_PAIR(CP_ERR) | A_BOLD);
    mvwprintw(win, 12, 3, "Mark as discontinued? [Y/N] : ");
    wattroff(win, COLOR_PAIR(CP_ERR) | A_BOLD);
    wrefresh(win);
    int ch = wgetch(win);
    close_win(win);
    draw_bg();
    if (ch == 'y' || ch == 'Y') {
        it->status = 0;
        alert("Item marked as discontinued.", 0);
    }
}

/* ──────────────────────── scrollable list ────────────────────────── */

static int flt_all(Item *it)  { (void)it; return 1; }
static int flt_low(Item *it)  { return it->per < 25.0 && it->status; }
static int flt_zero(Item *it) { return it->stock == 0 && it->status; }

void scr_list(const char *title, int (*flt)(Item *))
{
    if (!g_tab) { alert("Load a file first.", 1); return; }
    int R, C;
    getmaxyx(stdscr, R, C);
    int h = R - 6, w = C - 8;
    int list_h = h - 5;
    WINDOW *win = make_win(h, w, 3, 4, title);

    wattron(win, COLOR_PAIR(CP_SEL) | A_BOLD);
    mvwprintw(win, 1, 2, " %-14s %-26s %9s %7s %7s  %s",
              "Barcode", "Name", "Price", "Stock", "%Stock", "Status");
    wattroff(win, COLOR_PAIR(CP_SEL) | A_BOLD);
    mvwhline(win, 2, 1, ACS_HLINE, w - 2);
    mvwprintw(win, h - 2, 2, "[\xe2\x86\x91\xe2\x86\x93] Scroll   [q] Back");

    int offset = 0, ch;
    while (1) {
        int row = 3, shown = 0, total = 0;
        for (int i = 0; i < g_tab->cap; i++) {
            Item *cur = g_tab->buckets[i];
            while (cur) {
                if (flt(cur)) {
                    if (total >= offset && shown < list_h) {
                        int pair = (!cur->status || cur->per < 25.0) ? CP_ERR : CP_DEF;
                        wattron(win, COLOR_PAIR(pair));
                        mvwprintw(win, row, 2, " %-14s %-26.26s %9.2f %7d %6.1f%%  %s",
                                  cur->bar, cur->name, cur->price,
                                  cur->stock, cur->per,
                                  cur->status ? "Active" : "Disc.");
                        wattroff(win, COLOR_PAIR(pair));
                        row++;
                        shown++;
                    }
                    total++;
                }
                cur = cur->next;
            }
        }
        for (; shown < list_h; shown++)
            mvwprintw(win, row++, 2, "%*s", w - 4, "");

        wattron(win, COLOR_PAIR(CP_HINT) | A_BOLD);
        mvwprintw(win, h - 3, w - 20, " %d-%d of %d ",
                  total ? offset + 1 : 0,
                  offset + shown < total ? offset + shown : total, total);
        wattroff(win, COLOR_PAIR(CP_HINT) | A_BOLD);
        wrefresh(win);

        ch = wgetch(win);
        if      (ch == KEY_UP   && offset > 0)              offset--;
        else if (ch == KEY_DOWN && offset + list_h < total) offset++;
        else if (ch == ESC || ch == 'q' || ch == 'Q')       break;
    }
    close_win(win);
    draw_bg();
}

/* ──────────────────────── file screens ──────────────────────────── */

void scr_load(void)
{
    int R, C;
    getmaxyx(stdscr, R, C);
    WINDOW *win = make_win(7, 58, (R - 7) / 2, (C - 58) / 2, "Load File");
    mvwprintw(win, 2, 3, "Filename [%s]: ", g_file);
    wrefresh(win);
    char fname[STR] = {0};
    rstr(win, 2, 15 + (int)strlen(g_file), fname, STR);
    if (!fname[0]) strcpy(fname, g_file);
    FILE *fp = fopen(fname, "r");
    close_win(win);
    draw_bg();
    if (!fp) { alert("Cannot open file.", 1); return; }
    if (g_tab) {
        for (int i = 0; i < g_tab->cap; i++) {
            Item *cur = g_tab->buckets[i];
            while (cur) { Item *nx = cur->next; free(cur); cur = nx; }
        }
        free(g_tab->buckets);
        free(g_tab);
        g_tab = NULL;
    }
    g_tab = FillHashTable(fp);
    fclose(fp);
    strcpy(g_file, fname);
    draw_bg();
    alert("File loaded successfully.", 0);
}

void scr_save(void)
{
    if (!g_tab) { alert("No data to save.", 1); return; }
    int R, C;
    getmaxyx(stdscr, R, C);
    WINDOW *win = make_win(7, 58, (R - 7) / 2, (C - 58) / 2, "Save File");
    mvwprintw(win, 2, 3, "Filename [%s]: ", g_file);
    wrefresh(win);
    char fname[STR] = {0};
    rstr(win, 2, 15 + (int)strlen(g_file), fname, STR);
    if (!fname[0]) strcpy(fname, g_file);
    close_win(win);
    draw_bg();
    WriteFile(g_tab, fname);
    strcpy(g_file, fname);
    alert("File saved successfully.", 0);
}

/* ──────────────────────── submenus ──────────────────────────────── */

void menu_items(void)
{
    const char *items[] = { "Search Item", "Add Item", "Edit Item",
                             "Remove Item", "View All", "Back" };
    const char *descs[] = {
        "Find an item by barcode",
        "Add a new item to the inventory",
        "Modify an existing item's details",
        "Mark an item as discontinued",
        "Browse the full inventory list",
        "Return to the main menu"
    };
    int n = 6;
    int R, C; getmaxyx(stdscr, R, C);
    WINDOW *win = make_win(n + 6, MENU_W, (R - n - 6) / 2, (C - MENU_W) / 2, "Items");
    int ch;
    while (1) {
        ch = nav_menu(win, items, descs, n);
        switch (ch) {
            case 0: scr_search();                   break;
            case 1: scr_add();                      break;
            case 2: scr_edit();                     break;
            case 3: scr_remove();                   break;
            case 4: scr_list("View All", flt_all);  break;
            case 5: case -1: goto done;
        }
        redraw_win(win, "Items");
    }
done:
    close_win(win);
    draw_bg();
}

void menu_file(void)
{
    const char *items[] = { "Load File", "Save File", "Back" };
    const char *descs[] = {
        "Load inventory data from a CSV file",
        "Save the current inventory to a CSV file",
        "Return to the main menu"
    };
    int n = 3;
    int R, C; getmaxyx(stdscr, R, C);
    WINDOW *win = make_win(n + 6, MENU_W, (R - n - 6) / 2, (C - MENU_W) / 2, "File");
    int ch;
    while (1) {
        ch = nav_menu(win, items, descs, n);
        switch (ch) {
            case 0: scr_load(); break;
            case 1: scr_save(); break;
            case 2: case -1: goto done;
        }
        redraw_win(win, "File");
    }
done:
    close_win(win);
    draw_bg();
}

void menu_reports(void)
{
    const char *items[] = { "Low Stock  (<25%)", "Out of Stock",
                             "All Items", "Back" };
    const char *descs[] = {
        "Items with stock below 25% of capacity",
        "Items that are completely out of stock",
        "Browse the entire inventory list",
        "Return to the main menu"
    };
    int n = 4;
    int R, C; getmaxyx(stdscr, R, C);
    WINDOW *win = make_win(n + 6, MENU_W, (R - n - 6) / 2, (C - MENU_W) / 2, "Reports");
    int ch;
    while (1) {
        ch = nav_menu(win, items, descs, n);
        switch (ch) {
            case 0: scr_list("Low Stock Items", flt_low);  break;
            case 1: scr_list("Out of Stock",    flt_zero); break;
            case 2: scr_list("All Items",       flt_all);  break;
            case 3: case -1: goto done;
        }
        redraw_win(win, "Reports");
    }
done:
    close_win(win);
    draw_bg();
}
