#include "functions.h"

char* OCR(char *image)
{
  char *bar = (char*)malloc(sizeof(char) * BAR);
  char command[BUFFER];
  snprintf(command, BUFFER, "tesseract \"%s\" stdout 2>/dev/null", image);
  FILE *pipe = popen(command, "r");
  if (!pipe)
  {
    puts("Scan Failed.");
    return NULL;
  }
  fgets(bar, BAR, pipe);
  pclose(pipe);
  bar[strcspn(bar, "\n\r")] = '\0';
  return bar;
}

int BucketSize(int size)
{
    if (size <= 0)
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
  rewind(fp);
  int count = 0;
  char buff[BUFFER];
  fgets(buff, BUFFER, fp);
  while (fgets(buff, BUFFER, fp) != NULL)
    count++;
  return count;
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
  int hash = Hash(key, tab->cap);
  Item *temp = tab->buckets[hash]; 
  while (temp)
  {
    if (strcasecmp(temp->bar, key) == 0)
      return temp;
    temp = temp->next;
  }
  return NULL;
}

Table* CreateHashTable(FILE *fp)
{
  int i, cap, size;
  size = Count(fp);
  cap = BucketSize(size);
  Table *tab = (Table*)malloc(sizeof(Table));
  tab->cap = cap;
  tab->size = size;
  Item **buc = (Item**)malloc(sizeof(Item*) * cap);
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
  (*tab)->size++;
  return;
}

FILE* LoadFile(char *fileName)
{
  FILE *fp = fopen(fileName, "r");
  if (!fp)
  {
    mvprintw(3, 0, "File Opening Failed!");
    mvprintw(6, 0, "Press Any Key To Return Back.");
    getch();
    return NULL;
  }
  int size = Count(fp);
  if (!size)
  {
    attron(COLOR_PAIR(3));
    mvprintw(3, 0, "Empty File!");
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(4));
    mvprintw(6, 0, "Would You Like To Continue [Y/n] : ");
    attroff(COLOR_PAIR(4));
    echo();
    char ch = getch();
    noecho();
    if (ch == 'N' || ch == 'n')
    {
      return NULL;
    }
    else 
    {
      attron(COLOR_PAIR(2));
      mvprintw(3, 0, "File Opening Sucessfull.");
      attroff(COLOR_PAIR(2));
      attron(COLOR_PAIR(4));
      mvprintw(6, 0, "Press Any Key To Proceed To Create Hash Table");
      attroff(COLOR_PAIR(4));
      getch();
      return fp;
    }
  }
  attron(COLOR_PAIR(2));
  mvprintw(3, 0, "File Opening Sucessfull.");
  attroff(COLOR_PAIR(2));
  attron(COLOR_PAIR(4));
  mvprintw(6, 0, "Press Any Key To Proceed To Create Hash Table.");
  attroff(COLOR_PAIR(4));
  getch();
  return fp;
}

void BestSeller(Table *tab, char *fileName)
{
  int i;
  FILE *fp = fopen(fileName, "w");
  if (!fp)
  {
    return;
  }
  fprintf(fp, "Index,Barcode,Name,Price,Stock,Transactions,Capacity,Percent,Status\n");
  for (i = 0; i < tab->cap; i++)
  {
    Item *temp = tab->buckets[i];
    while (temp)
    { if (temp->trans < 0 && temp->per < 34) 
        fprintf(fp,"%d,%s,%s,%.2f,%d,%d,%d,%.2f,%d\n",i,temp->bar,temp->name,temp->price,temp->stock,temp->trans,temp->cap,temp->per,temp->status);
      temp = temp->next;
    }
  }
  fclose(fp);
  return;
}

void LowStock(Table *tab, char *fileName)
{
  int i;
  FILE *fp = fopen(fileName, "w");
  if (!fp)
  {
    return;
  }
  fprintf(fp, "Index,Barcode,Name,Price,Stock,Transactions,Capacity,Percent,Status\n");
  for (i = 0; i < tab->cap; i++)
  {
    Item *temp = tab->buckets[i];
    while (temp)
    { if (temp->per < 34 && temp->status) 
        fprintf(fp,"%d,%s,%s,%.2f,%d,%d,%d,%.2f,%d\n",i,temp->bar,temp->name,temp->price,temp->stock,temp->trans,temp->cap,temp->per,temp->status);
      temp = temp->next;
    }
  }
  fclose(fp);
  return;
}

void PurgeTable(Table *tab, char *fileName)
{
  int i;
  FILE *fp = fopen(fileName, "w");
  if (!fp)
  {
    return;
  }
  fprintf(fp, "Index,Barcode,Name,Price,Stock,Transactions,Capacity,Percent,Status\n");
  for (i = 0; i < tab->cap; i++)
  {
    Item *temp = tab->buckets[i];
    while (temp)
    { if (temp->status) 
        fprintf(fp,"%d,%s,%s,%.2f,%d,%d,%d,%.2f,%d\n",i,temp->bar,temp->name,temp->price,temp->stock,temp->trans,temp->cap,temp->per,temp->status);
      temp = temp->next;
    }
  }
  fclose(fp);
  return;
}

void SlowStock(Table *tab, char *fileName)
{
  int i;
  FILE *fp = fopen(fileName, "w");
  if (!fp)
  {
    return;
  }
  fprintf(fp, "Index,Barcode,Name,Price,Stock,Transactions,Capacity,Percent,Status\n");
  for (i = 0; i < tab->cap; i++)
  {
    Item *temp = tab->buckets[i];
    while (temp)
    { if (temp->trans > 0 && temp->per > 67) 
        fprintf(fp,"%d,%s,%s,%.2f,%d,%d,%d,%.2f,%d\n",i,temp->bar,temp->name,temp->price,temp->stock,temp->trans,temp->cap,temp->per,temp->status);
      temp = temp->next;
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
    return;
  }
  fprintf(fp, "Index,Barcode,Name,Price,Stock,Transactions,Capacity,Percent,Status\n");
  for (i = 0; i < tab->cap; i++)
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
