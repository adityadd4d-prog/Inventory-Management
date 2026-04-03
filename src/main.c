#include "functions.h"

int main(void)
{
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    start_color();
    init_color(COLOR_BLACK, 0, 0, 0);
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    bkgd(COLOR_PAIR(2));
    refresh();
    back:
    switch (MainMenu())
    {
      case 1:
      case 4:
        mvprintw(0, 0,"Inventory Management System V1.0");
        mvprintw(1, 0,"For The Code Visit Github :-");
        attron(COLOR_PAIR(2));
        mvprintw(2, 0,"https://github.com/adityadd4d-prog/Inventory-Management");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(3));
        mvprintw(5, 0,"Press any Key to Return Back.");
        refresh();
        getch();
        clear();
        goto back;
      case 5:
        printw("Exiting Program...");
        refresh();
        goto end;
    }
    end:
    endwin();
    return 0;
}
