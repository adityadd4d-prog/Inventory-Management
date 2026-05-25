#ifndef FUNCTIONS_H
#define FUNCTIONS_H

//CSV File Format :-Index,Bar code,Name,Price,Stock,NetTransactions,Capacity,Percent Stock,Status 

//------------------
//---Header Files---
//------------------
#include <ctype.h>
#include <math.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//---------------------
//---Named Constants---
//---------------------

//--For Hash Table---
#define BAR 14
#define BUFFER 256
#define PASS "123"
#define READ 9
#define SEP ","
#define STR 128
#define ADMIN "test"

//---For UI---
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
  int dis;
  int act;
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
void BestSeller(Table *tab, char *fileName);
void DeleteTable(Table *tab);
FILE* LoadFile(char *fileName);
void LowStock(Table *tab, char *fileName);
Table* PurgeTable(Table *tab, char *fileName);
void SlowStock(Table *tab, char *fileName);
int LibreOfficeLaunch(char *file);
void WriteFile(Table *tab, char *fileName);

//---For UI---
void create_main_window(Table *tab, int argc, char **argv);

#endif
