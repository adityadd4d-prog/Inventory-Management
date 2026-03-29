#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

int main(void)
{
  extern Table *g_tab;
  extern char g_file[STR];
  g_tab = NULL;
  g_file = "inventory.csv";
}
