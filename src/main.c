#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "functions.h"

#define ESC   27
#define ENTER 10

static Table *g_tab       = NULL;
static char   g_file[STR] = "inventory.csv";

/* ──────────────────────── window helpers ─────────────────────────── */

static WINDOW *make_win(int h, int w, int y, int x, const char *title)
{
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);
    if (title && title[0]) {
        int tx = (w - (int)strlen(title) - 2) / 2;
        wattron(win, A_BOLD | COLOR_PAIR(2));
        mvwprintw(win, 0, tx < 1 ? 1 : tx, " %s ", title);
        wattroff(win, A_BOLD | COLOR_PAIR(2));
    }
    keypad(win, TRUE);
    return win;
}

static void close_win(WINDOW *win)
{
    delwin(win);
    touchwin(stdscr);
    refresh();
}

static void alert(const char *msg, int err)
{
    int R, C;
    getmaxyx(stdscr, R, C);
    int w = (int)strlen(msg) + 8, h = 5;
    if (w > C - 4) w = C - 4;
    WINDOW *win = make_win(h, w, (R - h) / 2, (C - w) / 2, err ? "Error" : "Info");
    wattron(win, COLOR_PAIR(err ? 4 : 5) | A_BOLD);
    mvwprintw(win, 2, 4, "%-*.*s", w - 8, w - 8, msg);
    wattroff(win, COLOR_PAIR(err ? 4 : 5) | A_BOLD);
    mvwprintw(win, 3, (w - 18) / 2, "[ Press any key ]");
    wrefresh(win);
    wgetch(win);
    close_win(win);
}

static void draw_bg(void)
{
    int R, C;
    getmaxyx(stdscr, R, C);
    clear();

    /* ── outer border ── */
    wattron(stdscr, COLOR_PAIR(7));
    border(ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
           ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    wattroff(stdscr, COLOR_PAIR(7));

    /* ── title block ── */
    const char *hdr = "INVENTORY MANAGEMENT SYSTEM";
    const char *sub = "Hash Table  \xe2\x80\xa2  Separate Chaining";
    wattron(stdscr, A_BOLD | COLOR_PAIR(2));
    mvprintw(1, (C - (int)strlen(hdr)) / 2, "%s", hdr);
    wattroff(stdscr, A_BOLD | COLOR_PAIR(2));
    wattron(stdscr, COLOR_PAIR(1));
    mvprintw(2, (C - (int)strlen(sub)) / 2, "%s", sub);
    wattroff(stdscr, COLOR_PAIR(1));

    /* ── title separator, connected to outer border ── */
    wattron(stdscr, COLOR_PAIR(7));
    mvhline(3, 1, ACS_HLINE, C - 2);
    mvaddch(3, 0,     ACS_LTEE);
    mvaddch(3, C - 1, ACS_RTEE);
    wattroff(stdscr, COLOR_PAIR(7));

    /* ── status bar ── */
    char lft[160], rgt[128];
    snprintf(lft, sizeof(lft), "  File: %s", g_file);
    snprintf(rgt, sizeof(rgt),
             "Items: %d   [\xe2\x86\x91\xe2\x86\x93] Nav   [\xe2\x86\xb5] Select   [q] Back  ",
             g_tab ? g_tab->size : 0);
    wattron(stdscr, COLOR_PAIR(6) | A_BOLD);
    mvhline(R - 1, 0, ' ', C);
    mvprintw(R - 1, 0, "%s", lft);
    int rx = C - (int)strlen(rgt) - 1;
    if (rx > (int)strlen(lft) + 2) mvprintw(R - 1, rx, "%s", rgt);
    wattroff(stdscr, COLOR_PAIR(6) | A_BOLD);

    refresh();
}

/* ──────────────────────── menu runner ────────────────────────────── */

/*
 * hints: optional array of description strings, one per item.
 *        When non-NULL, a separator + hint line is drawn below the items.
 *        Menu window must be make_win(n + 6, ...) to accommodate it:
 *          row 0        : border + title
 *          row 1        : padding  (sy = 2)
 *          rows 2..n+1  : items
 *          row n+2      : hline separator
 *          row n+3      : hint text
 *          row n+4      : padding
 *          row n+5      : bottom border
 */
static void render_menu(WINDOW *win, const char **items, int n, int sel,
                        int sy, const char **hints)
{
    int i;
    for (i = 0; i < n; i++) {
        if (i == sel) {
            wattron(win, A_BOLD | COLOR_PAIR(3));
            mvwprintw(win, sy + i, 2, " [%d] %-20s", i + 1, items[i]);
            wattroff(win, A_BOLD | COLOR_PAIR(3));
        } else {
            wattron(win, COLOR_PAIR(1));
            mvwprintw(win, sy + i, 2, "  %d  %-20s", i + 1, items[i]);
            wattroff(win, COLOR_PAIR(1));
        }
    }
    if (hints) {
        int ww, hh;
        getmaxyx(win, hh, ww);
        (void)hh;
        wattron(win, COLOR_PAIR(7));
        mvwhline(win, sy + n, 1, ACS_HLINE, ww - 2);
        mvwprintw(win, sy + n + 1, 2, " %-*s", ww - 4, hints[sel]);
        wattroff(win, COLOR_PAIR(7));
    }
    wrefresh(win);
}

static int nav_menu(WINDOW *win, const char **items, int n, int sy,
                    const char **hints)
{
    int sel = 0, ch;
    render_menu(win, items, n, sel, sy, hints);
    while (1) {
        ch = wgetch(win);
        /* number hotkeys */
        if (ch >= '1' && ch <= '9') {
            int idx = ch - '1';
            if (idx < n) return idx;
        }
        switch (ch) {
            case KEY_UP:
            case 'k': case 'K':
                sel = (sel - 1 + n) % n;  break;
            case KEY_DOWN:
            case 'j': case 'J':
                sel = (sel + 1) % n;       break;
            case KEY_HOME:  sel = 0;       break;
            case KEY_END:   sel = n - 1;   break;
            case ENTER: case KEY_ENTER: return sel;
            case ESC: case 'q': case 'Q': return -1;
        }
        render_menu(win, items, n, sel, sy, hints);
    }
}

static void redraw_win(WINDOW *win, int w, const char *title)
{
    wclear(win);
    box(win, 0, 0);
    wattron(win, A_BOLD | COLOR_PAIR(2));
    mvwprintw(win, 0, (w - (int)strlen(title) - 2) / 2, " %s ", title);
    wattroff(win, A_BOLD | COLOR_PAIR(2));
    draw_bg();
    touchwin(win);
    wrefresh(win);
}

/* ──────────────────────── input helpers ──────────────────────────── */

static void rstr(WINDOW *win, int y, int x, char *buf, int max)
{
    echo();
    curs_set(1);
    wattron(win, COLOR_PAIR(5));
    mvwgetnstr(win, y, x, buf, max - 1);
    wattroff(win, COLOR_PAIR(5));
    noecho();
    curs_set(0);
}

/* ──────────────────────── item display ───────────────────────────── */

static void render_item(WINDOW *win, Item *it, int y)
{
    int pair = it->status ? 5 : 4;
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

static void scr_search(void)
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

static void scr_add(void)
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

    char name[STR] = {0}, ptmp[16] = {0}, stmp[16] = {0}, ctmp[16] = {0};
    mvwprintw(win, 3,  3, "Name     : "); wrefresh(win); rstr(win, 3,  14, name,  STR);
    mvwprintw(win, 4,  3, "Price    : "); wrefresh(win); rstr(win, 4,  14, ptmp,  16);
    mvwprintw(win, 5,  3, "Stock    : "); wrefresh(win); rstr(win, 5,  14, stmp,  16);
    mvwprintw(win, 6,  3, "Capacity : "); wrefresh(win); rstr(win, 6,  14, ctmp,  16);

    Item *ni = (Item *)malloc(sizeof(Item));
    if (!ni) { close_win(win); draw_bg(); alert("Memory error.", 1); return; }
    strcpy(ni->bar,  bar);
    strcpy(ni->name, name[0] ? name : "Unnamed");
    ni->price  = (float)atof(ptmp);
    ni->stock  = atoi(stmp);
    ni->cap    = atoi(ctmp);
    ni->trans  = 0;                                           /* BUG FIX: was missing ';' */
    ni->per    = ni->cap > 0 ? (ni->stock * 100.0) / ni->cap : 0.0;
    ni->status = 1;
    ni->next   = NULL;
    Add(&g_tab, ni);
    g_tab->size++;

    wattron(win, COLOR_PAIR(5) | A_BOLD);
    mvwprintw(win, 9, 3, "Item added successfully.");
    wattroff(win, COLOR_PAIR(5) | A_BOLD);
    mvwprintw(win, 11, (58 - 18) / 2, "[ Press any key ]");
    wrefresh(win);
    wgetch(win);
    close_win(win);
    draw_bg();
}

static void scr_edit(void)
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
    wattron(win, A_BOLD | COLOR_PAIR(2));
    mvwprintw(win, 0, (70 - 11) / 2, " Edit Item ");
    wattroff(win, A_BOLD | COLOR_PAIR(2));
    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, 2, 3, "(Leave blank to keep current)");
    wattroff(win, COLOR_PAIR(1));
    wattron(win, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(win, 3, 3, "  %-16s %-26s New Value", "Field", "Current");
    wattroff(win, COLOR_PAIR(3) | A_BOLD);
    mvwhline(win, 4, 2, ACS_HLINE, 66);
    wrefresh(win);

    int row = 5;
    char nname[STR] = {0}, nprice[16] = {0}, nstock[16] = {0}, ncap[16] = {0};

    mvwprintw(win, row, 3, "  %-16s %-26.26s", "Name", it->name);
    wrefresh(win); rstr(win, row, 47, nname, STR); row++;

    mvwprintw(win, row, 3, "  %-16s %-26.2f", "Price", it->price);
    wrefresh(win); rstr(win, row, 47, nprice, 16); row++;

    mvwprintw(win, row, 3, "  %-16s %-26d", "Stock", it->stock);
    wrefresh(win); rstr(win, row, 47, nstock, 16); row++;

    mvwprintw(win, row, 3, "  %-16s %-26d", "Capacity", it->cap);
    wrefresh(win); rstr(win, row, 47, ncap, 16); row++;

    if (nname[0])  strcpy(it->name, nname);
    if (nprice[0]) it->price = (float)atof(nprice);
    if (nstock[0]) it->stock = atoi(nstock);
    if (ncap[0])   it->cap   = atoi(ncap);
    it->per = it->cap > 0 ? (it->stock * 100.0) / it->cap : 0.0;

    wattron(win, COLOR_PAIR(5) | A_BOLD);
    mvwprintw(win, row + 1, 3, "Item updated.");
    wattroff(win, COLOR_PAIR(5) | A_BOLD);
    mvwprintw(win, row + 3, (70 - 18) / 2, "[ Press any key ]");
    wrefresh(win);
    wgetch(win);
    close_win(win);
    draw_bg();
}

static void scr_remove(void)
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
    wattron(win, COLOR_PAIR(4) | A_BOLD);
    mvwprintw(win, 12, 3, "Mark as discontinued? [Y/N] : ");
    wattroff(win, COLOR_PAIR(4) | A_BOLD);
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

typedef int (*ItemFilter)(Item *it);
static int flt_all(Item *it)  { (void)it; return 1; }
static int flt_low(Item *it)  { return it->per < 25.0 && it->status; }
static int flt_zero(Item *it) { return it->stock == 0 && it->status; }

static void scr_list(const char *title, ItemFilter flt)
{
    if (!g_tab) { alert("Load a file first.", 1); return; }
    int R, C;
    getmaxyx(stdscr, R, C);
    int h = R - 6, w = C - 8;
    int list_h = h - 5;
    WINDOW *win = make_win(h, w, 3, 4, title);

    wattron(win, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(win, 1, 2, " %-14s %-26s %9s %7s %7s  %s",
              "Barcode", "Name", "Price", "Stock", "%Stock", "Status");
    wattroff(win, COLOR_PAIR(3) | A_BOLD);
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
                        int pair = !cur->status ? 4 : (cur->per < 25.0 ? 4 : 1);
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

        wattron(win, COLOR_PAIR(6));
        mvwprintw(win, h - 3, w - 20, " %d-%d of %d ",
                  total ? offset + 1 : 0,
                  offset + shown < total ? offset + shown : total, total);
        wattroff(win, COLOR_PAIR(6));
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

static void scr_load(void)
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

static void scr_save(void)
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

static void menu_items(void)
{
    const char *items[] = {
        "Search Item", "Add Item", "Edit Item",
        "Remove Item", "View All", "Back"
    };
    const char *hints[] = {
        "Find an item by barcode",
        "Add a new item to the inventory",
        "Modify an existing item's fields",
        "Mark an item as discontinued",
        "Browse all inventory items",
        "Return to main menu"
    };
    int n = 6, w = 34;
    int R, C; getmaxyx(stdscr, R, C);
    WINDOW *win = make_win(n + 6, w, (R - n - 6) / 2, (C - w) / 2, "Items");
    int ch;
    while (1) {
        ch = nav_menu(win, items, n, 2, hints);
        switch (ch) {
            case 0: scr_search();                    break;
            case 1: scr_add();                       break;
            case 2: scr_edit();                      break;
            case 3: scr_remove();                    break;
            case 4: scr_list("View All", flt_all);   break;
            case 5: case -1: goto done;
        }
        redraw_win(win, w, "Items");
    }
done:
    close_win(win);
    draw_bg();
}

static void menu_file(void)
{
    const char *items[] = { "Load File", "Save File", "Back" };
    const char *hints[] = {
        "Load inventory data from a CSV file",
        "Write current inventory to a CSV file",
        "Return to main menu"
    };
    int n = 3, w = 34;
    int R, C; getmaxyx(stdscr, R, C);
    WINDOW *win = make_win(n + 6, w, (R - n - 6) / 2, (C - w) / 2, "File");
    int ch;
    while (1) {
        ch = nav_menu(win, items, n, 2, hints);
        switch (ch) {
            case 0: scr_load(); break;
            case 1: scr_save(); break;
            case 2: case -1: goto done;
        }
        redraw_win(win, w, "File");
    }
done:
    close_win(win);
    draw_bg();
}

static void menu_reports(void)
{
    const char *items[] = {
        "Low Stock  (<25%)", "Out of Stock",
        "All Items", "Back"
    };
    const char *hints[] = {
        "Active items below 25% capacity",
        "Active items with zero remaining stock",
        "Browse the full inventory",
        "Return to main menu"
    };
    int n = 4, w = 34;
    int R, C; getmaxyx(stdscr, R, C);
    WINDOW *win = make_win(n + 6, w, (R - n - 6) / 2, (C - w) / 2, "Reports");
    int ch;
    while (1) {
        ch = nav_menu(win, items, n, 2, hints);
        switch (ch) {
            case 0: scr_list("Low Stock Items", flt_low);  break;
            case 1: scr_list("Out of Stock",    flt_zero); break;
            case 2: scr_list("All Items",       flt_all);  break;
            case 3: case -1: goto done;
        }
        redraw_win(win, w, "Reports");
    }
done:
    close_win(win);
    draw_bg();
}

/* ──────────────────────── main ──────────────────────────────────── */

int main(void)
{
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);

    start_color();
    use_default_colors();
    init_pair(1, -1,            -1);           /* default text          */
    init_pair(2, COLOR_CYAN,    -1);           /* titles / headers      */
    init_pair(3, COLOR_BLACK,   COLOR_YELLOW); /* selected item         */
    init_pair(4, COLOR_RED,     -1);           /* error / discontinued  */
    init_pair(5, COLOR_GREEN,   -1);           /* success / active      */
    init_pair(6, COLOR_BLACK,   COLOR_WHITE);  /* status bar            */
    init_pair(7, COLOR_YELLOW,  -1);           /* border / hints        */

    draw_bg();

    const char *main_items[] = { "Items", "File", "Reports", "Exit" };
    const char *main_hints[] = {
        "Search, add, edit, and remove items",
        "Load or save the CSV data file",
        "View filtered inventory reports",
        "Exit the application"
    };
    int n = 4, w = 34;
    int R, C;
    getmaxyx(stdscr, R, C);
    WINDOW *mwin = make_win(n + 6, w, (R - n - 6) / 2, (C - w) / 2, "Main Menu");

    int ch;
    while (1) {
        ch = nav_menu(mwin, main_items, n, 2, main_hints);
        switch (ch) {
            case 0: menu_items();   break;
            case 1: menu_file();    break;
            case 2: menu_reports(); break;
            case 3: case -1: goto quit;
        }
        redraw_win(mwin, w, "Main Menu");
    }
quit:
    if (g_tab) {
        for (int i = 0; i < g_tab->cap; i++) {
            Item *cur = g_tab->buckets[i];
            while (cur) { Item *nx = cur->next; free(cur); cur = nx; }
        }
        free(g_tab->buckets);
        free(g_tab);
    }
    delwin(mwin);
    endwin();
    return 0;
}
