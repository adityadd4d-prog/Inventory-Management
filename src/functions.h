#ifndef FUNCTIONS_H
#define FUNCTIONS_H

//CSV File Format :-Index,Bar code,Name,Price,Stock,TransactionsOfItems,Capacity,Percent Stock,Status 

//------------------
//---Header Files---
//------------------
#include <math.h>
#include <menu.h>
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
#define PASS "123"
#define READ 9
#define SEP ","
#define STR 128
#define ADMIN "test"

//---For UI---
#define ENTER 10
#define F1 KEY_F(1)
#define PATH 512


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
FILE* LoadFile(char *fileName);
void WriteFile(Table *tab, char *fileName);

//---For UI---
Item* AddItem(void);
int AdminVerify(void);
int FileMenu(void);
int ItemMenu(void);
int MainMenu(void);
int MyMenu(char **opt, int optNum, int y, int x);
int ReportMenu(void);
int SearchMenu(void);
void DisplayItem(Item *it);

#endif
