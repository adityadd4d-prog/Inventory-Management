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
  switch (MainMenu())
  {
    case 1:
      ItemBack:
      clear();
      switch (ItemMenu())
      {
        case 1:
          if (!tab)
          {
            attron(COLOR_PAIR(3));
            mvprintw(0, 0, "Hash Table Not Created.");
            attroff(COLOR_PAIR(3));
            attron(COLOR_PAIR(4));
            mvprintw(3, 0,"Press any Key to Return Back.");
            attroff(COLOR_PAIR(4));
            getch();
            goto ItemBack;
          }
          SearchBack:
          clear();
          char bar[BAR];
          Item *it = Search(bar, tab);
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
              if (it)
                DisplayItem(it);
              else 
              {
                attron(COLOR_PAIR(3));
                mvprintw(0, 0, "Item Not Found!");
                attroff(COLOR_PAIR(3));
                attron(COLOR_PAIR(4));
                mvprintw(3, 0,"Press any Key to Return Back.");
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
              strcpy(bar,OCR(path));
              if (!bar)
              {
                attron(COLOR_PAIR(3));
                mvprintw(5, 0, "OCR Recognition Failed!");
                attroff(COLOR_PAIR(3));
                attron(COLOR_PAIR(4));
                mvprintw(8, 0,"Press any Key to Return Back.");
                attroff(COLOR_PAIR(4));
                getch();
                goto SearchBack;
              } 
              if (it)
                DisplayItem(it);
              else 
              {
                attron(COLOR_PAIR(3));
                mvprintw(0, 0, "Item Not Found!");
                attroff(COLOR_PAIR(3));
                attron(COLOR_PAIR(4));
                mvprintw(3, 0,"Press any Key to Return Back.");
                attroff(COLOR_PAIR(4));
                getch();
              }
              goto SearchBack;
            case 3:
              goto ItemBack;
          }
        case 2:
          //update
        case 3:
          Item *ni = AddItem();
          Add((Table**)tab, ni);
          attron(COLOR_PAIR(3));
          mvprintw(0, 0, "Item Added");
          attroff(COLOR_PAIR(3));
          attron(COLOR_PAIR(4));
          mvprintw(3, 0,"Press any Key to Return Back.");
          attroff(COLOR_PAIR(4));
          getch();
          goto ItemBack;
        case 4:
          // view all
        case 5:
          goto MainBack;
      }
    case 2:
      switch (ReportMenu())
      {
        case 1:
          // Low stock items
        case 2:
          // Bestsellers
        case 3:
          // slow stock
        case 4:
          goto MainBack;

      }

    case 3:
      if (AdminVerify())
      {
        FileBack:
        clear();
        switch (FileMenu())
        {
          case 1:
            char path[STR];
            attron(COLOR_PAIR(2));
            mvprintw(0, 0,"---Load File---");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
            mvprintw(1, 0,"Path : ");
            echo();
            curs_set(1);
            getnstr(path, STR - 1);
            curs_set(0);
            noecho();
            attroff(COLOR_PAIR(1));
            fp = LoadFile(path);
            if (!fp)
              goto FileBack;
            tab = FillHashTable(fp);
            clear();
            attron(COLOR_PAIR(2));
            mvprintw(0, 0,"Hash Table Created");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(4));
            mvprintw(3, 0,"Press any Key to Return Back.");
            attroff(COLOR_PAIR(4));
            getch();
            goto FileBack;
          case 2:
            // save file
          case 3:
            // purge discountinued Item 
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
        mvprintw(7, 0,"Press any Key to Return Back.");
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
      mvprintw(5, 0,"Press any Key to Return Back.");
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

