#include "functions.h"

Item* AddItem(void)
{
  Item *ni = (Item*)malloc(sizeof(Item));
  char buff[BUFFER];
  attron(COLOR_PAIR(2));
  mvprintw(0, 0, "---Add Item---");
  attroff(COLOR_PAIR(2));
  echo();
  curs_set(1);
  mvprintw(1, 0,"Barcode        : ");
  getnstr(buff,BUFFER - 1);
  strcpy(ni->bar,buff);
  mvprintw(2, 0,"Name           : ");
  getnstr(buff,BUFFER - 1);
  strcpy(ni->name,buff);
  mvprintw(3, 0,"Price          : ");
  getnstr(buff,BUFFER - 1);
  ni->price = atof(buff);
  mvprintw(4, 0,"Stock          : ");
  getnstr(buff,BUFFER - 1);
  ni->stock = atoi(buff);
  mvprintw(5, 0,"Stock Capacity : ");
  getnstr(buff,BUFFER - 1);
  ni->cap = atoi(buff);
  ni->trans = 0;
  ni->per = (ni->stock/ni->cap) * 100;
  ni->status = 1;
  ni->next = NULL;
  noecho();
  curs_set(0);
  return ni;
}

int AdminVerify(void)
{
  char user[STR], pass[STR];
  attron(COLOR_PAIR(2));
  mvprintw(0, 0, "---Admin Verify---");
  attroff(COLOR_PAIR(2));
  attron(COLOR_PAIR(1));
  echo();
  curs_set(1);
  mvprintw(1, 0, "Username : ");
  getnstr(user, STR - 1);
  curs_set(0);
  noecho();
  mvprintw(2, 0, "Password : ");
  getnstr(pass, STR - 1);
  attroff(COLOR_PAIR(1));
  if ((strcmp(user,ADMIN) == 0) && (strcmp(pass,PASS) == 0))
    return 1;
  else 
    return 0;
}

int FileMenu(void)
{
  char *opt[] = {"Load File", "Save File", "Purge Discontinued Items", "Back"};
  attron(COLOR_PAIR(2));
  printw("---File Menu---");
  attroff(COLOR_PAIR(2));
  int ch = MyMenu(opt, 4, 1, 0);
  return ch;
}

int ItemMenu(void)
{
  char *opt[] = {"Search", "Update", "Add Item","View All", "Back"};
  attron(COLOR_PAIR(2));
  printw("---Item Menu---");
  attroff(COLOR_PAIR(2));
  int ch = MyMenu(opt, 5, 1, 0);
  return ch;
}

int MainMenu(void)
{
  char *opt[] = {"Items", "Report", "File", "About", "Exit"};
  attron(COLOR_PAIR(2));
  printw("---Main Menu---");
  attroff(COLOR_PAIR(2));
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

int ReportMenu(void)
{
  char *opt[] = {"Low Stock Items", "Bestsellers", "Slow Stock", "Back"};
  attron(COLOR_PAIR(2));
  printw("---Report Menu---");
  attroff(COLOR_PAIR(2));
  int ch = MyMenu(opt, 4, 1, 0);
  return ch;
}

int SearchMenu(void)
{
  char *opt[] = {"Barcode", "OCR", "Back"};
  attron(COLOR_PAIR(2));
  printw("---Search Menu---");
  attroff(COLOR_PAIR(2));
  int ch = MyMenu(opt, 3, 1, 0);
  return ch;
}

void DisplayItem(Item *it)
{
  clear();
  attron(COLOR_PAIR(2));
  printw("---ITEM---\n");
  attroff(COLOR_PAIR(2));
  printw("Barcode        : %s\n",it->bar);
  printw("Name           : %s\n",it->name);
  printw("Price          : %.2f\n Rupees",it->price);
  printw("Stock          : %d\n",it->stock);
  printw("Transactions   : %d\n",it->trans);
  printw("Stock Capacity : %d\n",it->cap);
  printw("Stock Percent  : %.2f%\n",it->per);
  printw("Status         : %s\n",it->status ? "Active" : "Discontinued");
  attron(COLOR_PAIR(4));
  mvprintw(12, 0,"Press any Key to Return Back.");
  attroff(COLOR_PAIR(4));
  getch();
}
