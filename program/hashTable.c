#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
        if (is_prime) return n;
    }
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


