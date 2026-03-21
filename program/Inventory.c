// 
//
// File header should be : index,Bar,name,qty,price
// the next line should contain the number of items 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "MyFunctions.h"

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    puts("Usage : ./Inventory <Database Name>");
    return 0;
  }
  FILE *fp = LoadFile(argv[1]);
  Table *tab = FillHashTable(fp);
  while (1)
  {
    char nm[STR];
    printf("Enter Search Item : ");
    fgets(nm , STR , stdin);
    Item *item = Search(nm, tab);
    if (!item)
    {
      puts("Not Found!\n\n");
    }
    else   
    {
      printf("Barcode : %s\n", item->bar);
      printf("Name : %s\n", item->name);
      printf("Quantity : %d\n", item->qty);
      printf("Price : %f\n", item->price);
    } 
  }

  return 0;
}



