#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "functions.h"

char* OCR(char *image)
{
  char command[BUFFER], bar[BAR];
  snprintf(command, BUFFER, "tesseract \"%s\" stdout 2>/dev/null", image);
  FILE *pipe = popen(command, "r");
  if (!pipe)
  {
    puts("Scan Failed.");
    return NULL;
  }
  fgets(bar, BAR, pipe);
  return bar;
}

int BucketSize(int size)
{
    if (size <=0)
      return 7;
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

int Count(FILE *fp) 
{
    fseek(fp, 0, SEEK_END);
    long pos = ftell(fp);
    while (pos > 0)
    {
        fseek(fp, --pos, SEEK_SET);
        char c = fgetc(fp);
        if (c != '\n') 
          break;
    }
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
    return atoi(tok);
}

int Hash(char *key, int cap)
{
  unsigned int hash = 5381;
  int i = 0;
  while (key[i] != '\0')
  {
    hash = (hash * 33) + key[i++];
  }
  return hash % cap;
}

Item* ReadItem(char *str)
{
  int fl = 0;
  Item *ni = NULL;
  do 
  {
    ni = (Item*)malloc(sizeof(Item));
    fl++;
  } while (!ni && (fl < 3));
  if (!ni)
  {
    return NULL;
  }
  char *tok[READ];
  int i = 0;
  tok[i++] = strtok(str, SEP);
  while (i < READ)
  {
    tok[i] = strtok(NULL, SEP);
    if (tok[i] == NULL)
    {
      free(ni);
      return NULL;
    }
    i++;
  }
  strcpy(ni->bar, tok[1]);
  strcpy(ni->name, tok[2]);
  ni->price = atof(tok[3]);
  ni->stock = atoi(tok[4]);
  ni->trans = atoi(tok[5]);
  ni->cap = atoi(tok[6]);
  ni->per = atof(tok[7]);
  ni->status = atoi(tok[8]);
  ni->next = NULL;
  return ni;
}

Item* Search(char *key, Table *tab)
{
  int hash = Hash( key, tab->capacity);
  Item *temp = tab->buckets[hash]; 
  while (temp)
  {
    if (strcasecmp(temp->bar, key) == 0)
      if (temp->status)
        return temp;
      else 
        {
          printf("The Searched Barcode Belongs to a Discontinued Item.\nWould You Still Like To Fetch Data [Y/N] : ");
          int ch;
          scanf("%c", &ch);
          if (ch == 'n' || ch == 'N')
            return NULL;
          else 
            return temp;
        }
    temp = temp->nxt;
  }
  return NULL;
}

Table* CreateHashTable(FILE *fp)
{
  int i, cap, size;
  size = Count(fp);
  cap = BucketSize(size);
  Table *tab = (Table*)malloc(sizeof(Table));
  if (!tab)
  {
    puts("Hash Table Creation Failed!\nSuggested Rerun.");
    exit(0);
  }
  tab->cap = cap;
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

Table* FillHashTable(FILE *fp)
{
  Table *tab = CreateHashTable(fp);
  char buff[BUFFER];
  rewind(fp);
  fgets(buff, BUFFER, fp);
  while (fgets(buff, BUFFER, fp) != NULL)
  {
    Item *ni = ReadItem(buff);
    if (!ni) 
      continue;
    Add(&tab, ni);
  }
  return tab;
}

void Add(Table **tab, Item *ni)
{
  int hash = Hash(ni->bar, (*tab)->cap);
  if ((*tab)->buckets[hash] == NULL)
  {
    (*tab)->buckets[hash] = ni;
  }
  else 
  {
    Item *temp = (*tab)->buckets[hash];
    while (temp->next != NULL)
          temp = temp->next;
    temp->next = ni;
  }
  return;
}

Void LoadFile(char *fileName)
{
  FILE *fp = fopen(fileName, "r");
  if (!fp)
  {
    puts("Data Loading Failed!\nRerun Program");
    exit(0);
  }
  int size = Count(fp);
  if (!size)
  {
    puts("Empty Database!\n Would You Like To Continue [Y/N] : ");
    char ch;
    scanf("%c", &ch);
    if (ch == 'N' || ch == 'n')
    {
      puts("Exiting Program...\n");
      exit(0);
    }
    else 
    {
      fclose(fp);
      return;
    }
  }
  fclose(fp);
  return;
}

void WriteFile(Table *tab, char *fileName)
{
  int i;
  FILE *fp = fopen(fileName, "w");
  if (!fp)
  {
    puts("File Creation Failed.\nTry Again.");
    return NULL;
  }
  fprintf(fp, "Index,Barcode,Name,Price,Stock,Transactions,Capacity,Percent,Status\n");
  for (i = 1; i <= tab->cap; i++)
  {
    Item *temp = tab->buckets[i];
    while (temp)
    {  
      fprintf(fp,"%d,%s,%s,%.2f,%d,%d,%d,%.2f,%d\n",i,temp->bar,temp->name,temp->price,temp->stock,temp->trans,temp->cap,temp->per,temp->status);
      temp = temp->next;
    }
  }
  fclose(fp);
  return;
}
