#include <stdio.h>
#include <stdlib.h>
#include "foo.h"
#include "bar.h"
#include "bee.h"

#define MAX 100

int main(int argc, char **argv) 
{
  int x[MAX];

  int b = 0;
  int c = 0;
  
#pragma omp parallel for
  for (int i = 1; i < MAX; ++i) {
    x[i] = x[i - 1];
    foo();
  }

#pragma omp parallel for
  for (int i = 1; i < MAX; ++i) {
    x[i] = x[i - 1];
  }

  bee();

#pragma omp parallel for
  for (int i = 1; i < MAX; ++i) {
    x[i] = x[i] + 1;
  }
  
  exit(0);
}
