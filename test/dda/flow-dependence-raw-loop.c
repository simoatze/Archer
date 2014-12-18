#include <stdio.h>
#include <stdlib.h>

#define MAX 120

void fun() {}

int main(int argc, char **argv) 
{
  int x[MAX], y[MAX];
  int a;

#pragma omp parallel for
  for (int i = 1; i < MAX; ++i) {
    x[i] = x[i - 1];
    x[i - 1] = x[i];
  }
  
#pragma omp parallel for
  for (int i = 0; i < MAX; ++i) {
    y[i] = y[i] + 1;
  }

  fun();

#pragma omp parallel for
  for (int i = 0; i < MAX; ++i) {
    x[i] = x[i] + 1;
  }

  exit(0);
}
