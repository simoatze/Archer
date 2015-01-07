/**
 * ============================================================================
 * Name        : race06.c
 * Author      : Simone Atzeni
 * Version     : 1.0
 * Copyright   : 2015
 * Description : Data Race in OpenMP Programs: Data race due to 
 * not thread-safe library
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

int icount;

void lib_func() {
  icount++;
}

int main(int argc, char **argv) 
{
#pragma omp parallel 
  {
    lib_func();
  }
  
  printf("icount: %d\n", icount);
}
