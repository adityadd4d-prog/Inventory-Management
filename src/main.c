#include "functions.h"

int main(void)
{
  FILE *fp = NULL;
  Table *tab = NULL;
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
  bkgd(COLOR_PAIR(1));
  curs_set(0);
  refresh();
  MainBack:
  clear();
  char fpath[PATH] = "/home/slayer/Projects/Inventory-Management/data/inventory.csv"; // Default Path for files
  const char rpath[] = "/home/slayer/Projects/Inventory-Management/Reports/"; // default Path for Reports
  switch (MainMenu())
  {
    case 1:
      if (!tab)
      {
        attron(COLOR_PAIR(3));
        mvprintw(0, 0, "Hash Table Not Created.");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(4));
        mvprintw(3, 0,"Press Enter Key to Return Back.");
        attroff(COLOR_PAIR(4));
        getch();
        goto MainBack;
      }
      ItemBack:
      clear();
      switch (ItemMenu())
      {
        case 1:
          SearchBack:
          clear();
          char bar[BAR];
          Item *it;
          switch (SearchMenu())
          {
            case 1:
              attron(COLOR_PAIR(2));
              mvprintw(0, 0, "Enter Barcode : ");
              attroff(COLOR_PAIR(2));
              echo();
              curs_set(1);
              getnstr(bar,BAR-1);
              curs_set(0);
              noecho();
              it = Search(bar, tab);
              if (it)
                DisplayItem(it);
              else 
              {
                attron(COLOR_PAIR(3));
                mvprintw(0, 0, "Item Not Found!");
                attroff(COLOR_PAIR(3));
                attron(COLOR_PAIR(4));
                mvprintw(3, 0,"Press Enter Key to Return Back.");
                attroff(COLOR_PAIR(4));
                getch();
              }
              goto SearchBack;
            case 2:
              char path[PATH]; 
              attron(COLOR_PAIR(2));
              mvprintw(0, 0, "OCR Implemented using Tesseract.");
              attroff(COLOR_PAIR(2));
              attron(COLOR_PAIR(2));
              mvprintw(2, 0, "Enter Path To Image : ");
              attroff(COLOR_PAIR(2));
              echo();
              curs_set(1);
              getnstr(path,PATH-1);
              curs_set(0);
              noecho();
              char *res = OCR(path);
              if (!res)
              {
                attron(COLOR_PAIR(3));
                mvprintw(5, 0, "OCR Recognition Failed!");
                attroff(COLOR_PAIR(3));
                attron(COLOR_PAIR(4));
                mvprintw(8, 0,"Press Enter Key to Return Back.");
                attroff(COLOR_PAIR(4));
                getch();
                goto SearchBack;
              } 
              strcpy(bar,res);
              it = Search(bar, tab);
              if (it)
                DisplayItem(it);
              else 
              {
                attron(COLOR_PAIR(3));
                mvprintw(5, 0, "Item Not Found!");
                attroff(COLOR_PAIR(3));
                attron(COLOR_PAIR(4));
                mvprintw(8, 0,"Press Enter Key to Return Back.");
                attroff(COLOR_PAIR(4));
                getch();
              }
              goto SearchBack;
            case 3:
              goto ItemBack;
          }
        case 2:
          attron(COLOR_PAIR(2));
          mvprintw(0, 0, "---Item Updation---");
          attroff(COLOR_PAIR(2));
          UpdateItem(tab);
          goto ItemBack;
        case 3:
          Item *ni = AddItem();
          Add(&tab, ni);
          tab->size++;
          attron(COLOR_PAIR(3));
          mvprintw(7, 0, "Item Added");
          attroff(COLOR_PAIR(3));
          attron(COLOR_PAIR(4));
          mvprintw(10, 0,"Press Enter Key to Return Back.");
          attroff(COLOR_PAIR(4));
          getch();
          goto ItemBack;
        case 4:
          goto MainBack;
      }
    case 2:
      if (!tab)
      {
        attron(COLOR_PAIR(3));
        mvprintw(0, 0, "Hash Table Not Created.");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(4));
        mvprintw(3, 0,"Press Enter Key to Return Back.");
        attroff(COLOR_PAIR(4));
        getch();
        goto MainBack;
      }
      ReportBack:
      clear();
      switch (ReportMenu())
      {
        char buff[PATH];
        case 1:
          snprintf(buff, PATH - 1, "%s%s", rpath,"Low_Stock.csv");
          attron(COLOR_PAIR(2));
          mvprintw(0, 0, "---Low Stock Report---");
          attroff(COLOR_PAIR(2));
          mvprintw(2, 0, "Report Created At : %s",buff);
          attron(COLOR_PAIR(4));
          mvprintw(5, 0,"Press Enter Key to Return Back.");
          attroff(COLOR_PAIR(4));
          LowStock(tab, buff);
          getch();
          goto ReportBack;
        case 2:
          snprintf(buff, PATH - 1, "%s%s", rpath,"Bestsellers.csv");
          attron(COLOR_PAIR(2));
          mvprintw(0, 0, "---Bestseller Report---");
          attroff(COLOR_PAIR(2));
          mvprintw(2, 0, "Report Created At : %s",buff);
          attron(COLOR_PAIR(4));
          mvprintw(5, 0,"Press Enter Key to Return Back.");
          attroff(COLOR_PAIR(4));
          BestSeller(tab, buff);
          getch();
          goto ReportBack;
        case 3:
          snprintf(buff, PATH - 1, "%s%s", rpath,"Slow_Stock.csv");
          attron(COLOR_PAIR(2));
          mvprintw(0, 0, "---Slow Stock Report---");
          attroff(COLOR_PAIR(2));
          mvprintw(2, 0, "Report Created At : %s",buff);
          attron(COLOR_PAIR(4));
          mvprintw(5, 0,"Press Enter Key to Return Back.");
          attroff(COLOR_PAIR(4));
          SlowStock(tab, buff);
          getch();
          goto ReportBack;
        case 4:
          goto MainBack;
      }
    case 3:
      if (AdminVerify())
      {
        char ch;
        FileBack:
        clear();
        switch (FileMenu())
        {
          case 1:
            attron(COLOR_PAIR(2));
            mvprintw(0, 0,"---Load File---");
            attroff(COLOR_PAIR(2));
            mvprintw(1, 0, "Use Default Path [Y/n]: %s", fpath);
            ch = getch();
            if (ch == 'n' || ch =='N')
            {
              move(1,0);
              clrtoeol();
              mvprintw(1, 0,"Path : ");
              echo();
              curs_set(1);
              getnstr(fpath, PATH - 1);
              curs_set(0);
              noecho();
            }
            fp = LoadFile(fpath);
            if (!fp)
              goto FileBack;
            tab = FillHashTable(fp);
            clear();
            attron(COLOR_PAIR(2));
            mvprintw(0, 0,"Hash Table Created");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(4));
            mvprintw(3, 0,"Press Enter Key to Return Back.");
            attroff(COLOR_PAIR(4));
            getch();
            fclose(fp);
            goto FileBack;
          case 2:
          if (!tab)
            {
              attron(COLOR_PAIR(3));
              mvprintw(0, 0, "Hash Table Not Created.");
              attroff(COLOR_PAIR(3));
              attron(COLOR_PAIR(4));
              mvprintw(3, 0,"Press Enter Key to Return Back.");
              attroff(COLOR_PAIR(4));
              getch();
              goto FileBack;
            }
            attron(COLOR_PAIR(2));
            mvprintw(0, 0,"---Save File---");
            attroff(COLOR_PAIR(2));
            mvprintw(1, 0, "Use Default Path [Y/n]: %s", fpath);
            ch = getch();
            if (ch == 'n' || ch =='N')
            {
              move(1,0);
              clrtoeol();
              mvprintw(1, 0,"Path : ");
              echo();
              curs_set(1);
              getnstr(fpath, PATH - 1);
              curs_set(0);
              noecho();
            }
            WriteFile(tab,fpath);
            attron(COLOR_PAIR(2));
            mvprintw(3, 0,"File Saved at %s",fpath);
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(4));
            mvprintw(6, 0,"Press Enter Key to Return Back.");
            attroff(COLOR_PAIR(4));
            refresh();
            getch();
            goto FileBack;
          case 3:
          if (!tab)
            {
              attron(COLOR_PAIR(3));
              mvprintw(0, 0, "Hash Table Not Created.");
              attroff(COLOR_PAIR(3));
              attron(COLOR_PAIR(4));
              mvprintw(3, 0,"Press Enter Key to Return Back.");
              attroff(COLOR_PAIR(4));
              getch();
              goto FileBack;
            }
            attron(COLOR_PAIR(2));
            mvprintw(0, 0,"---Purging Discontinued Items---");
            attroff(COLOR_PAIR(2));
            PurgeTable(tab, fpath);
            mvprintw(2, 0,"Purging Completed");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(4));
            mvprintw(6, 0,"Press Enter Key to Return Back.");
            attroff(COLOR_PAIR(4));
            getch();
            goto FileBack;
          case 4:
            goto MainBack;
        }
      }
      else 
      {
        attron(COLOR_PAIR(3));
        mvprintw(4, 0,"Invalid Credentials!");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(4));
        mvprintw(7, 0,"Press Enter Key to Return Back.");
        attroff(COLOR_PAIR(4));
        refresh();
        getch();
        goto MainBack;
      }
    case 4:
      mvprintw(0, 0,"Inventory Management System V1.0");
      mvprintw(1, 0,"For The Code Visit Github :-");
      attron(COLOR_PAIR(2));
      mvprintw(2, 0,"https://github.com/adityadd4d-prog/Inventory-Management");
      attroff(COLOR_PAIR(2));
      attron(COLOR_PAIR(4));
      mvprintw(5, 0,"Press Enter Key to Return Back.");
      attroff(COLOR_PAIR(4));
      refresh();
      getch();
      goto MainBack;
    case 5:
      printw("Exiting Program...");
      refresh();
      goto end;
  }
  end:
  endwin();
  return 0;
}

