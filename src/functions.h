#ifndef FUNCTIONS_H
#define FUNCTIONS_H

//CSV File Format :-Index,Bar code,Name,Price,Stock,TransactionsOfItems,Capacity,Percent Stock,Status 

//------------------
//---Header Files---
//------------------

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//---------------------
//---Named Constants---
//---------------------

//--For Hash Table---
#define BAR 15
#define BUFFER 256
#define READ 9
#define SEP ","
#define STR 128

//---For UI---
#define ESC    27
#define ENTER  10
#define MENU_W 48
#define SY      2
#define CP_DEF   1
#define CP_TITLE 2
#define CP_SEL   3
#define CP_ERR   4
#define CP_OK    5
#define CP_BAR   6
#define CP_HINT  7


//---------------------------
//---Structure Definations---
//---------------------------

typedef struct item
{
  char bar[BAR];
  char name[STR];
  float price;
  int stock;
  int trans;
  int cap;
  double per;
  int status;
  struct item *next;
} Item;

typedef struct
{
  Item **buckets;
  int cap;
  int size;
} Table;


//-------------------------
//---Function Prototypes---
//-------------------------

//---For Hash Table---
char* OCR(char *image);
int BucketSize(int size);
int Count(FILE *fp);
int Hash(char *key, int cap);
Item* ReadItem(char *str);
Item* Search(char *key, Table *tab);
Table* CreateHashTable(FILE *fp);
Table* FillHashTable(FILE *fp);
void Add(Table **tab, Item *ni);
void LoadFile(char *fileName);
void WriteFile(Table *tab, char *fileName);

//---For UI---
int     nav_menu(WINDOW *win, const char **items, const char **descs, int n);
void    alert(const char *msg, int err);
void    close_win(WINDOW *win);
void    draw_bg(void);
void    menu_file(void);
void    menu_items(void);
void    menu_reports(void);
void    redraw_win(WINDOW *win, const char *title);
void    render_item(WINDOW *win, Item *it, int y);
void    render_menu(WINDOW *win, const char **items, const char **descs, int n, int sel);
void    rstr(WINDOW *win, int y, int x, char *buf, int max);
void    scr_add(void);
void    scr_edit(void);
void    scr_list(const char *title, int (*flt)(Item *));
void    scr_load(void);
void    scr_remove(void);
void    scr_save(void);
void    scr_search(void);
WINDOW *make_win(int h, int w, int y, int x, const char *title);


extern Table *g_tab;
extern char   g_file[STR];


#endif
