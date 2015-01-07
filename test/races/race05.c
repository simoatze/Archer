/**
 * ============================================================================
 * Name        : race05.c
 * Author      : Simone Atzeni
 * Version     : 1.0
 * Copyright   : 2015
 * Description : Data Race in OpenMP Programs: Data race due to incorrect 
 * use of clause master, there is no implicit barrier.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) 
{
  int init, local;
  
  init = 5;

#pragma omp parallel private(local)
  {
#pragma omp master
    {
      init  = 10;
    }
    
    local = init;
    printf("local = %d\n", local);
  }

  exit(0);
}
