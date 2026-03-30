#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"


Table *g_tab = NULL;
char g_file[STR] = "inventory.csv";

int main(void)
{
  Table *g_tab = NULL;
  char g_file[STR] = "inventory.csv";
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();
    use_default_colors();
    init_pair(1, -1,           -1);
    init_pair(2, COLOR_CYAN,   -1);
    init_pair(3, COLOR_BLACK,  COLOR_CYAN);
    init_pair(4, COLOR_RED,    -1);
    init_pair(5, COLOR_GREEN,  -1);
    init_pair(6, COLOR_BLACK,  COLOR_WHITE);
    draw_bg();
    endwin();
}
