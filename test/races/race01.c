/**
 * ============================================================================
 * Name        : race01.c
 * Author      : Simone Atzeni
 * Version     : 1.0
 * Copyright   : 2015
 * Description : Data Race in OpenMP Programs: data race because of 
 * loop-carried data dependence in parallel for-loop.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX 100

int main(int argc, char **argv) 
{
  int x[MAX];
  
#pragma omp parallel for
  for (int i = 1; i < MAX; ++i) {
    x[i] = x[i - 1];
  }
  
  exit(0);
}
