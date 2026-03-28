#ifndef FUNCTIONS_H
#define FUNCTIONS_H

//CSV File Format :-Index,Bar code,Name,Price,Stock,TransactionsOfItems,Capacity,Percent Stock,Status 

//---Named Constants---
#define BAR 15
#define BUFFER 256
#define READ 9
#define SEP ","
#define STR 128


//---Structure Definations---
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


//---Function Prototypes---
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

#endif
