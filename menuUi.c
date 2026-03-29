#include <ncurses.h>
#include <string.h>
#include "functions.h"


void draw_bg(void)
{
    int R, C;
    getmaxyx(stdscr, R, C);
    clear();
    wattron(stdscr, COLOR_PAIR(2) | A_BOLD);
    const char *hdr = "INVENTORY MANAGEMENT SYSTEM";
    mvprintw(1, (C - (int)strlen(hdr)) / 2, "%s", hdr);
    wattroff(stdscr, COLOR_PAIR(2) | A_BOLD);
    mvhline(2, 1, ACS_HLINE, C - 2);

    char lft[160], rgt[128];
    snprintf(lft, sizeof(lft), " File: %s", g_file);
    snprintf(rgt, sizeof(rgt), "Items: %d   [" "\xe2\x86\x91" "\xe2\x86\x93" "] Nav   [" "\xe2\x86\xb5" "] Select   [q] Back ",
             g_tab ? g_tab->size : 0);
    wattron(stdscr, COLOR_PAIR(6) | A_BOLD);
    mvhline(R - 1, 0, ' ', C);
    mvprintw(R - 1, 0, "%s", lft);
    int rx = C - (int)strlen(rgt) - 1;
    if (rx > (int)strlen(lft) + 2) mvprintw(R - 1, rx, "%s", rgt);
    wattroff(stdscr, COLOR_PAIR(6) | A_BOLD);
    refresh();
}
