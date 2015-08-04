#include <stdio.h>
#include <stdlib.h>

#define MAX 120

int a;

void fun() {
  a = 0;
}

int main(int argc, char **argv) 
{
  int x[MAX], y[MAX];

#pragma omp parallel for
  for (int i = 1; i < MAX; ++i) {
    x[i] = i;
    y[i] = i;
  }
  
  fun();

#pragma omp parallel for
  for (int i = 0; i < MAX; ++i) {
    x[i] = x[y[i]] + 1;
  }

  exit(0);
}
