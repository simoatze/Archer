/**
 * ============================================================================
 * Name        : race02.c
 * Author      : Simone Atzeni
 * Version     : 1.0
 * Copyright   : 2015
 * Description : Data Race in OpenMP Programs: data race because it is missing 
 * the clause private(x) and x is shared by default.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#define N 100

int main(int argc, char **argv) 
{
  int i;
  double h, x, sum, pi;
  
  h = 1.0 / (double) N;
  sum = 0.0;
  
#pragma omp parallel for reduction(+:sum) shared(h)
  for (i = 1; i <= N; i++) {
    x = h * ((double) i - 0.5);
    sum += (1.0 / (1.0 + x * x));
  }
  pi = h * sum;
  
  printf("Pi: %lf\n", pi);
  
  exit(0);
}
