#ifndef MYFUNCTIONS_H
#define MYFUNCTIONS_H

//---Named Constants---
#define BAR 12
#define STR 128

//---Structures---
typedef struct item
{
  char bar[BAR];
  char name[STR];
  int qty;
  float price;
  struct item *nxt;
} Item;

typedef struct
{
  Item **buckets;
  int capacity;
  int size;
} Table;

//---Functions Prototypes---

int BucketSize(int size);
Table* CreateHashTable(int size);
unsigned int Hash(char *key, int cap);
void MsgLoading();

//---Colours---
#define STOP    "\033[0m"
#define BLACK   "\033[38;5;232m"
#define RED     "\033[38;5;196m"
#define GREEN   "\033[38;5;46m"
#define YELLOW  "\033[38;5;226m"
#define BLUE    "\033[38;5;27m"
#define MAGENTA "\033[38;5;207m"
#define CYAN    "\033[38;5;51m"
#define WHITE   "\033[38;5;231m"

#endif // !MYFUNCTIONS_H
