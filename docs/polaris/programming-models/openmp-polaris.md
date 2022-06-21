# OpenMP

## Overview

The OpenMP API is an open standard for parallel programming. The specification document can be found here: https://www.openmp.org. The specification describes directives, runtime routines, and environment variables that allow an application developer to express parallelism (e.g. shared memory multiprocessing and device offloading). Many compiler vendors provide implementations of the OpenMP specification (https://www.openmp.org/specifications).

## Using OpenMP on Polaris

- TODO: modules available

## Building on Polaris

- TODO: instructions for different compilers

## Running on Polaris

- TODO: how to run

## Example

```
$ cat hello.cpp
#include <stdio.h>
#include <omp.h>

int main( int argv, char** argc ) {

  printf( "Number of devices: %d\n", omp_get_num_devices() );

  #pragma omp target
  {
    if( !omp_is_initial_device() )
      printf( "Hello world from accelerator.\n" );
    else
      printf( "Hello world from host.\n" );
  }
  return 0;
}

$ cat hello.F90
program  main
  use omp_lib
  implicit none

  write(*,*) "Number of devices:", omp_get_num_devices()

  !$omp target 
    if( .not. omp_is_initial_device() ) then
      write(*,*) "Hello world from accelerator"
    else
      write(*,*) "Hello world from host"
   endif
  !$omp end target

end program main

$ module load TODO
$ ...

```