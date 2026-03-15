// 
//
// File header should be : sku,name,qty,price,stk

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define NAME 128
#define SIZE 10000

typedef struct item
{
  unsigned int sku;
  char name[NAME];
  char cat[NAME];
  int qty;
  float price;
  struct item *nxt;
} Item;

typedef struct hashTable
{
  Item **buckets;
  int capacity;
  int size;
}

