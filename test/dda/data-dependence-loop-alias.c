#include <stdio.h>
#include <stdlib.h>

#define MAX 100

int main(int argc, char **argv) 
{
  int x[MAX], *y; 

  y = x;
  for (int i = 0; i < MAX - 1; ++i) {
    x[i] = y[i+1];
  }

  exit(0);
}
