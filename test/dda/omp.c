#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAX 1000000

int main(int argc, char **argv) 
{
  int x[MAX]; 
  int i;

  for (i = 0; i < MAX; i++) { 
    printf("ThreadID: %d\n", omp_get_thread_num());
    x[i] = 1;
    //x[i + 1] = 3;
  }

  exit(0);
}
