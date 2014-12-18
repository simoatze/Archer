#include <stdio.h>
#include <stdlib.h>

#define MAX 100

int main(int argc, char **argv) 
{
  int x[MAX], y[MAX];

  for (int i = 1; i < MAX; i++) {
    y[i] = i;
    x[i] = y[i + 1];
  }
  
  exit(0);
}
