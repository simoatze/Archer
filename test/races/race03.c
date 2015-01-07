/**
 * ============================================================================
 * Name        : race03.c
 * Author      : Simone Atzeni
 * Version     : 1.0
 * Copyright   : 2015
 * Description : Data Race in OpenMP Programs: data race on j due to 
 * default sharing of variable j since the inner loop is not parallelized 
 * and the index is treated as a shared variable.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#define N 100

double compute(int i, int j)
{
  return (i + j) * 0.5;
}

int main(int argc, char **argv) 
{
  int i, j;
  double x[N][N];

#pragma omp parallel for
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      x[i][j] = compute(i,j);
    }
  }
  
  exit(0);
}
