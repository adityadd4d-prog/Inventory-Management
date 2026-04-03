#include "functions.h"

int MainMenu(void)
{
  char *opt[] = {"Items", "Report", "File", "About", "Exit"};
  curs_set(0);
  attron(COLOR_PAIR(2));
  printw("---Main Menu---");
  attroff(COLOR_PAIR(1));
  int ch = MyMenu(opt, 5, 1, 0);
  return ch;

}

int MyMenu(char **opt, int optNum, int y, int x)
{
    int i, key, ch = optNum - 1;
    MENU *menu;
    ITEM *cur, **all;
    all = (ITEM**)calloc(optNum + 1, sizeof(ITEM*));
    for (i = 0; i < optNum; i++)
      all[i] = new_item(opt[i],"");
    all[i] = (ITEM*)NULL;
    menu = new_menu(all);
    set_menu_win(menu, stdscr);
    set_menu_sub(menu, derwin(stdscr, optNum, 20, y, x));
    post_menu(menu);
    refresh();
    while ((key = getch()) != F1) 
    {
        switch (key) {
        case KEY_DOWN:
            menu_driver(menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(menu, REQ_UP_ITEM);
            break;
        case ENTER:
            cur = current_item(menu);
            ch = item_index(cur) + 1;
            goto end;
        }
    }
end:
    unpost_menu(menu);
    refresh();
    for (i = 0; i < optNum; i++)
      free_item(all[i]);
    free_menu(menu);
    return ch;
}
