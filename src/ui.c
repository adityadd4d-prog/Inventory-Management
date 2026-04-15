#include "functions.h"

Item* AddItem(Table tab)
{
  Item *ni = (Item*)malloc(sizeof(Item));
  char buff[BUFFER];
  echo();
  curs_set(1);
  int valid;
  do
  {
    valid = 0;
    clear();
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(0, 0, "---Add Item---");
    attroff(COLOR_PAIR(2) | A_BOLD);
    mvprintw(1, 0, "Barcode (13 digits) : ");
    clrtoeol();
    getnstr(buff, BAR - 1);
    if (strlen(buff) == 13)
    {
      valid = 1;
      for (int i = 0; i < 13; i++)
        if (!isdigit((unsigned char)buff[i]))
        {  
          valid = 0;
          break;
        }
    }
    if (!valid)
    {
      attron(COLOR_PAIR(3));
      mvprintw(2, 0, "Enter exactly 13 numeric digits!");
      attroff(COLOR_PAIR(3));
      attron(COLOR_PAIR(4) | A_UNDERLINE);
      mvprintw(5, 0,"Press Enter Key to Enter Again.");
      attroff(COLOR_PAIR(4) | A_UNDERLINE);
      getch();
    }
    if (valid)
    {
      Item *ch = Search(buff, &tab);
      if (ch)
      {
        attron(COLOR_PAIR(3) | A_BLINK);
        mvprintw(2, 0, "Barcode Already in Use.");
        attroff(COLOR_PAIR(3) | A_BLINK);
        attron(COLOR_PAIR(4));
        mvprintw(5, 0,"Press Enter Key to Enter Again.");
        attroff(COLOR_PAIR(4));
        getch();
        valid = 0;
      }
    }
  } while (!valid);
  strcpy(ni->bar, buff);
  do
  {
    mvprintw(2, 0, "Name                : ");
    clrtoeol();
    getnstr(buff, STR - 1);
  } while (strlen(buff) == 0);
  strcpy(ni->name, buff);
  do 
  {
    mvprintw(3, 0, "Price               : ");
    clrtobot();
    getnstr(buff, BUFFER - 1);
    ni->price = atof(buff);
    if (ni->price < 0)
    {
      attron(COLOR_PAIR(3));
      mvprintw(5, 0, "Entered Price Can't be Negative!");
      attroff(COLOR_PAIR(3));
      attron(COLOR_PAIR(4) | A_UNDERLINE);
      mvprintw(8, 0,"Press Enter Key to Enter Again.");
      attroff(COLOR_PAIR(4) | A_UNDERLINE);
      getch();
      clrtobot();
    }
  } while (ni->price <= 0);
  do 
  {
    mvprintw(4, 0, "Stock               : ");
    clrtobot();
    getnstr(buff, BUFFER - 1);
    if (strlen(buff) == 0)
        continue;
    ni->stock = atoi(buff);
    if (ni->stock < 0)
    {
      attron(COLOR_PAIR(3));
      mvprintw(6, 0, "Entered Stock Can't be Negative!");
      attroff(COLOR_PAIR(3));
      attron(COLOR_PAIR(4) | A_UNDERLINE);
      mvprintw(9, 0,"Press Enter Key to Enter Again.");
      attroff(COLOR_PAIR(4) | A_UNDERLINE);
      getch();
      clrtobot();
    }
  } while (ni->stock < 0);
  do 
  {
    valid = 1;
    mvprintw(5, 0, "Stock Capacity      : ");
    clrtobot();
    getnstr(buff, BUFFER - 1);
    ni->cap = atoi(buff);
    if (ni->cap <= 0 || ni->cap <= ni->stock)
    { 
      valid = 0;
      attron(COLOR_PAIR(3));
      mvprintw(7, 0, "Entered Stock Capacity is less than the current Stock!");
      attroff(COLOR_PAIR(3));
      attron(COLOR_PAIR(4) | A_UNDERLINE);
      mvprintw(10, 0,"Press Enter Key to Enter Again.");
      attroff(COLOR_PAIR(4) | A_UNDERLINE);
      getch();
      clrtobot();
    }

  } while (!valid);
  ni->trans = 0;
  ni->per = ((float)ni->stock / ni->cap) * 100.0;
  ni->status = 1;
  ni->next = NULL;
  noecho();
  curs_set(0);
  return ni;
}

int AdminVerify(void)
{
  clear();
  char user[STR], pass[STR];
  attron(COLOR_PAIR(2) | A_BOLD);
  mvprintw(0, 0, "---Admin Verify---");
  attroff(COLOR_PAIR(2) | A_BOLD);
  echo();
  curs_set(1);
  do 
  {
    mvprintw(1, 0, "Username : ");
    clrtoeol();
    getnstr(user, STR - 1);
  } while (strlen(user) == 0);
  curs_set(0);
  noecho();
  do 
  {
    mvprintw(2, 0, "Password : ");
    clrtoeol();
    getnstr(pass, STR - 1);
  } while (strlen(pass) == 0);
  if ((strcmp(user,ADMIN) == 0) && (strcmp(pass,PASS) == 0))
    return 1;
  else
  {
    attron(COLOR_PAIR(3) | A_BLINK);
    mvprintw(4, 0,"Invalid Credentials!");
    attroff(COLOR_PAIR(3) | A_BLINK);
    attron(COLOR_PAIR(4) | A_UNDERLINE);
    mvprintw(7, 0,"Press Enter Key to Return Back.");
    attroff(COLOR_PAIR(4) | A_UNDERLINE);
    refresh();
    getch();
    return 0;
  }
}

int FileMenu(Table *tab)
{
  char *opt[] = {"Load File", "Save File", "Purge Discontinued Items", "Back"};
  attron(COLOR_PAIR(2) | A_BOLD);
  mvprintw(0, 0, "---File Menu---");
  attroff(COLOR_PAIR(2) | A_BOLD);
  attron(COLOR_PAIR(4));
  mvprintw(1, 0,"Total Items : %d | Active Items : %d | Discontinued Items : %d", tab ? tab->size : 0, tab ? tab->act : 0, tab ? tab->dis : 0);
  attroff(COLOR_PAIR(4));
  int ch = MyMenu(opt, 4, 2, 0);
  return ch;
}

int ItemMenu(Table *tab)
{
  char *opt[] = {"Search", "Update", "Add Item", "Back"};
  attron(COLOR_PAIR(2) | A_BOLD);
  mvprintw(0, 0, "---Item Menu---");
  attroff(COLOR_PAIR(2) | A_BOLD);
  attron(COLOR_PAIR(4));
  mvprintw(1, 0,"Total Items : %d | Active Items : %d | Discontinued Items : %d",tab->size, tab->act, tab->dis);
  attroff(COLOR_PAIR(4));
  int ch = MyMenu(opt, 4, 2, 0);
  return ch;
}

int MainMenu(void)
{
  char *opt[] = {"Items", "Report", "File", "About", "Exit"};
  attron(COLOR_PAIR(2) | A_BOLD);
  printw("---Main Menu---");
  attroff(COLOR_PAIR(2) | A_BOLD);
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
  set_menu_sub(menu, derwin(stdscr, optNum, 30, y, x));
  set_menu_fore(menu, COLOR_PAIR(5) | A_BOLD | A_REVERSE);
  set_menu_back(menu, COLOR_PAIR(1));
  post_menu(menu);
  refresh();
  while ((key = getch()) != F1) 
  {
    switch (key)
    {
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
  free(all);
  return ch;
}

int ReportMenu(void)
{
  char *opt[] = {"Low Stock Items", "Bestsellers", "Slow Stock", "Back"};
  attron(COLOR_PAIR(2) | A_BOLD);
  printw("---Report Menu---");
  attroff(COLOR_PAIR(2) | A_BOLD);
  int ch = MyMenu(opt, 4, 1, 0);
  return ch;
}

int SearchMenu(void)
{
  char *opt[] = {"Barcode", "OCR", "Back"};
  attron(COLOR_PAIR(2) | A_BOLD);
  printw("---Search Menu---");
  attroff(COLOR_PAIR(2) | A_BOLD);
  int ch = MyMenu(opt, 3, 1, 0);
  return ch;
}

void DisplayItem(Item *it)
{
  clear();
  attron(COLOR_PAIR(2) | A_BOLD);
  printw("---ITEM---\n");
  attroff(COLOR_PAIR(2) | A_BOLD);
  printw("Barcode          : %s\n",it->bar);
  printw("Name             : %s\n",it->name);
  printw("Rate             : %.2f Rupees\n",it->price);
  printw("Stock            : %d\n",it->stock);
  printw("Net Transactions : %d\n",it->trans);
  printw("Stock Capacity   : %d\n",it->cap);
  printw("Stock Percent    : %.2f%%\n",it->per);
  printw("Status           : %s\n",it->status ? "Active" : "Discontinued");
  attron(COLOR_PAIR(4) | A_UNDERLINE);
  mvprintw(11, 0,"Press Enter Key to Return Back.");
  attroff(COLOR_PAIR(4) | A_UNDERLINE);
  getch();
}

int UpdateMenu(void)
{
  char *opt[] = {"Price", "Quantity", "Status", "Stock Capacity", "Back"};
  attron(COLOR_PAIR(2) | A_BOLD);
  printw("---Updation Menu---");
  attroff(COLOR_PAIR(2) | A_BOLD);
  int ch = MyMenu(opt, 5, 1, 0);
  return ch;
}

void UpdateItem(Table *tab)
{
  char buff[BUFFER]; 
  echo();
  curs_set(1);
  mvprintw(1, 0,"Barcode : ");
  getnstr(buff,BUFFER - 1);
  curs_set(0);
  Item *it = Search(buff, tab);
  if (!it)
  {
    attron(COLOR_PAIR(3));
    mvprintw(3, 0,"Barcode Not Found!");
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(4) | A_UNDERLINE);
    mvprintw(6, 0,"Press Enter Key to Return Back.");
    attroff(COLOR_PAIR(4) | A_UNDERLINE);
    getch();
    return;
  }
  if (!it->status)
    if (!AdminVerify())
      return;
  Back:
  clear();
  echo();
  switch (UpdateMenu())
  {
    case 1:
      mvprintw(0, 0,"Old Price     : %.2f Rupees",it->price);
      do 
      {
        curs_set(1);
        mvprintw(1, 0,"New Price     : ");
        clrtoeol();
        getnstr(buff, BUFFER - 1);
        curs_set(0);
        if (strlen(buff) == 0)
          continue;
      } while (atof(buff) <= 0);
      it->price = atof(buff);
      mvprintw(2, 0,"Updated Price : %.2f Rupees",it->price);
      attron(COLOR_PAIR(2));
      mvprintw(4, 0,"Price Updated.");
      attroff(COLOR_PAIR(2));
      attron(COLOR_PAIR(4) | A_UNDERLINE);
      mvprintw(7, 0,"Press Enter Key to Return Back.");
      attroff(COLOR_PAIR(4) | A_UNDERLINE);
      getch();
      goto Back;
    case 2:
      mvprintw(0, 0,"Old Stock         : %d",it->stock);
      int trans;
      do 
      {
        curs_set(1);
        mvprintw(1, 0,"Stock Transaction : ");
        clrtobot();
        getnstr(buff, BUFFER - 1);
        curs_set(0);
        if (strlen(buff) == 0)
          continue;
        trans = atoi(buff);
        if (trans < -it->stock)
        {
          attron(COLOR_PAIR(3));
          mvprintw(3, 0, "Entered Stock Transaction Can't be More Than the Available Stock.");
          attroff(COLOR_PAIR(3));
          attron(COLOR_PAIR(4) | A_UNDERLINE);
          mvprintw(6, 0,"Press Enter Key to Enter Again.");
          attroff(COLOR_PAIR(4) | A_UNDERLINE);
          getch();
          clrtobot();
        }
        if ((trans + it->stock) > it->cap)
        {
          attron(COLOR_PAIR(3));
          mvprintw(3, 0, "Entered Stock Intake Can't be More Than The Stock Capacity.");
          attroff(COLOR_PAIR(3));
          attron(COLOR_PAIR(4) | A_UNDERLINE);
          mvprintw(6, 0,"Press Enter Key to Enter Again.");
          attroff(COLOR_PAIR(4) | A_UNDERLINE);
          getch();
          clrtobot();
        }
      } while (trans == 0 || trans < -it->stock || (trans + it->stock) > it->cap);
      it->trans += trans;
      it->stock += trans;
      it->per = ((float)it->stock/it->cap) * 100;
      mvprintw(2, 0,"Updated Stock     : %d",it->stock);
      attron(COLOR_PAIR(2));
      mvprintw(4, 0,"Stock Updated.");
      attroff(COLOR_PAIR(2));
      attron(COLOR_PAIR(4) | A_UNDERLINE);
      mvprintw(7, 0,"Press Enter Key to Return Back.");
      attroff(COLOR_PAIR(4) | A_UNDERLINE);
      getch();
      goto Back;
    case 3:
      mvprintw(0, 0,"Current Status   : %s",it->status ? "Active" : "Discontinued");
      mvprintw(1, 0,"New Status [A/D] : ");
      char ch = getch();
      if ((ch == 'D' || ch == 'd') && it->status == 1)
      {
        if (AdminVerify())
        {
          clear();
          mvprintw(0, 0,"Current Status   : %s",it->status ? "Active" : "Discontinued");
          mvprintw(1, 0,"New Status [A/D] : %c",ch);
          it->status = 0;
          tab->dis++;
          tab->act--;
        }
      }
      if ((ch == 'A' || ch == 'a') && it->status == 0)
      {
        it->status = 1;
        tab->dis--;
        tab->act++;
      }
      mvprintw(2, 0,"Updated Status   : %s",it->status ? "Active" : "Discontinued");
      attron(COLOR_PAIR(2));
      mvprintw(4, 0,"Status Updated.");
      attroff(COLOR_PAIR(2));
      attron(COLOR_PAIR(4) | A_UNDERLINE);
      mvprintw(7, 0,"Press Enter Key to Return Back.");
      attroff(COLOR_PAIR(4) | A_UNDERLINE);
      getch();
      goto Back;
    case 4:
      mvprintw(0, 0,"Current Stock Capacity : %d",it->cap);
      do 
      {
        curs_set(1);
        mvprintw(1, 0,"Change                 : ");
        clrtobot();
        getnstr(buff, BUFFER - 1);
        curs_set(0);
        if (strlen(buff) == 0)
          continue;
        trans = atoi(buff);
        if ((trans + it->cap) < it->stock)
        {
          attron(COLOR_PAIR(3));
          mvprintw(3, 0, "Entered Value Can't be Accepted as it results in Stock Capacity Being Less than Current Stock.");
          attroff(COLOR_PAIR(3));
          attron(COLOR_PAIR(4) | A_UNDERLINE);
          mvprintw(6, 0,"Press Enter Key to Enter Again.");
          attroff(COLOR_PAIR(4) | A_UNDERLINE);
          getch();
          clrtobot();

        }
      } while (trans == 0 || (trans + it->cap) < it->stock);
      it->cap += trans;
      it->per = ((float)it->stock/it->cap) * 100;
      mvprintw(2, 0,"Updated Stock Capacity : %d",it->cap);
      attron(COLOR_PAIR(2));
      mvprintw(4, 0,"Stock Capacity Updated.");
      attroff(COLOR_PAIR(2));
      attron(COLOR_PAIR(4) | A_UNDERLINE);
      mvprintw(7, 0,"Press Enter Key to Return Back.");
      attroff(COLOR_PAIR(4) | A_UNDERLINE);
      getch();
      goto Back;
    case 5:
      noecho();
      curs_set(0);
      return;
  }
}
