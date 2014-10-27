/*
 ============================================================================
 Name        : data_race01.c
 Author      : Simone Atzeni
 Version     : 1.0
 Copyright   : 2014
 Description : Data Race in OpenMP Programs
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX 120

int main(int argc, char **argv) 
{
  int a[MAX];
  int i;

  // Vector initialization
#pragma omp parallel for
  for (i = 0; i < MAX; ++i) {
    a[i] = i;
  }
  
#pragma omp parallel for
  for (i = 0; i < MAX - 1; ++i) {
    a[i] = a[i + 1] + 1;
  }

  printf("Terminated!\n");

  exit(0);
}
