#include <stdio.h>
#include <stdlib.h>

#define MAX 100

void foo();
void bar();

int main(int argc, char **argv) 
{
  int x[MAX];
  
#pragma omp parallel for
  for (int i = 1; i < MAX; ++i) {
    x[i] = x[i - 1];
    foo();
  }
  
  exit(0);
}

void foo()
{
  int a = 5782364;
  printf("Num: %d\n", a);
  bar();
}

void bar()
{
  int b = 10;
}
