#include <stdio.h>
#include <stdlib.h>

#define MAX 100

int main(int argc, char **argv) 
{
  int x[MAX]; 

  for (int i = 1; i < MAX - 1; i++) { 
    x[i] = 1;
    x[i + 1] = x[i + 1] + 3;
  }

  exit(0);
}