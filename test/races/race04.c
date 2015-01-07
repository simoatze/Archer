/**
 * ============================================================================
 * Name        : race04.c
 * Author      : Simone Atzeni
 * Version     : 1.0
 * Copyright   : 2015
 * Description : Data Race in OpenMP Programs: Data race due to incorrect use 
 * of nowait clause
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define N 100

int main(int argc, char **argv) 
{
  int i;
  int a[N], b[N], z = 0;

#pragma omp parallel for
  for (i = 0; i < N; ++i) {
    a[i] = i;
    b[i] = i;
  }

#pragma omp parallel
{
#pragma omp single nowait
  for (i = 0; i < N; i++)
    a[i] = 2.0;

#pragma omp for nowait
  for (i = 0; i < N; i++)
    b[i] = a[i] / 2.0;

#pragma omp for reduction(+:z)
  for (i = 0; i < N; i++)
    z += sqrt(a[i] * b[i]);
 }
 
 printf("z: %d\n", z);
}
