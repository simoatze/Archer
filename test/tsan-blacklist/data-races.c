#include <stdio.h>
#include <stdlib.h>

#define MAX 120

int main(int argc, char **argv) 
{
  int a[MAX], b[MAX];
  int i;

  // Vector initialization
#pragma omp parallel for
  for (i = 0; i < MAX; ++i) {
    a[i] = i;
    b[i] = i;
  }
  
#pragma omp parallel for
  for (i = 0; i < MAX - 1; ++i) {
    a[i] = a[i + 1] + 1;
  }

#pragma omp parallel for
  for (i = 0; i < MAX - 1; ++i) {
    b[i] = b[i + 1] + 1;
  }

  printf("Terminated!\n");

  exit(0);
}
