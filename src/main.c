#include "functions.h"

int main(void)
{
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);

    start_color();
    use_default_colors();
    init_pair(CP_DEF,   -1,           -1);
    init_pair(CP_TITLE, COLOR_CYAN,   -1);
    init_pair(CP_SEL,   COLOR_BLACK,  COLOR_YELLOW);
    init_pair(CP_ERR,   COLOR_RED,    -1);
    init_pair(CP_OK,    COLOR_GREEN,  -1);
    init_pair(CP_BAR,   COLOR_BLACK,  COLOR_WHITE);
    init_pair(CP_HINT,  COLOR_YELLOW, -1);

    draw_bg();

    const char *main_items[] = { "Items", "File", "Reports", "Exit" };
    const char *main_descs[] = {
        "Search, add, edit, or remove inventory items",
        "Load or save the inventory CSV file",
        "View filtered reports and stock alerts",
        "Exit the program"
    };
    int n = 4;
    int R, C;
    getmaxyx(stdscr, R, C);
    WINDOW *mwin = make_win(n + 6, MENU_W, (R - n - 6) / 2, (C - MENU_W) / 2,
                            "Main Menu");
    int ch;
    while (1) {
        ch = nav_menu(mwin, main_items, main_descs, n);
        switch (ch) {
            case 0: menu_items();   break;
            case 1: menu_file();    break;
            case 2: menu_reports(); break;
            case 3: case -1: goto quit;
        }
        redraw_win(mwin, "Main Menu");
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
