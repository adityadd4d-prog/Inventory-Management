#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "MyFunctions.h"


int BucketSize(int size)
{
    int i, n;
    double cap = size / 5.0;
    cap += cap * 0.25;
    n = (int)ceil(cap) - 1;
    while (1)
    {
        int is_prime = 1;
        n++;
        if (n % 2 == 0 || n % 3 == 0) 
          continue;
        for (i = 5; i * i <= n; i += 6)
            if (n % i == 0 || n % (i + 2) == 0)
            {
              is_prime = 0;
              break;
            }
        if (is_prime)
          return n;
    }
}

Item* CreateItem(char *str)
{
  int fl = 0;
  Item *ni = NULL;
  do 
  {
    ni = (Item*)malloc(sizeof(Item));
    fl++;
  } while (!ni && (fl < 3));
  ni->bar = strtok(str, SEP);
  ni->name = strtok(NULL, SEP);
  ni->qty = atoi(strtok(NULL, SEP));
  ni->price = atof(strtok(NULL, SEP));
  ni->next = NULL;
  return ni;
}

Table* CreateHashTable(int size)
{
  int i, cap;
  cap = BucketSize(size);
  Table *tab = (Table*)malloc(sizeof(table));
  if (!tab)
  {
    puts("Hash Table Creation Failed!\nSuggested Rerun.");
    exit(0);
  }
  tab->capacity = cap;
  tab->size = size;
  Item **buc = (Item**)malloc(sizeof(Item*) * cap);
  if (!buc)
  {
    puts("Hash Table Creation Failed!\nSuggested Rerun.");
    exit(0);
  }
  for (i = 0; i < cap; i++)
  {
    buc[i] = NULL;
  }
  tab->buckets = buc;
  return tab;
}

unsigned int Count(FILE *fp) 
{
    fseek(fp, 0, SEEK_END);
    long pos = ftell(fp);

    // skip trailing newlines
    while (pos > 0)
    {
        fseek(fp, --pos, SEEK_SET);
        char c = fgetc(fp);
        if (c != '\n' && c != '\r') 
          break;
    }

    // walk back to start of last line
    while (pos > 0) 
    {
        fseek(fp, --pos, SEEK_SET);
        char c = fgetc(fp);
        if (c == '\n') 
          break;
    }

    char buff[BUFFER];
    fgets(buff, BUFFER, fp);
    char *tok = strtok(buff, SEP);
    if (!tok) 
      return 0;

    return (unsigned int)atoi(tok);
}

unsigned int Hash(char *key, int cap)
{
  unsigned int hash = 5381;
  int i = 0;
  while (key[i] != '\0')
  {
    hash = (hash * 33) + key[i];
  }
  return hash % cap;
}



