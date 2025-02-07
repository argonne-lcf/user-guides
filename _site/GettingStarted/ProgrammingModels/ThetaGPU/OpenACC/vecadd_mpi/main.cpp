#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <cassert>
#include "mpi.h"

#include <openacc.h>

#define _N 1024
#define _LOCAL_SIZE 64

#ifdef _SINGLE_PRECISION
  typedef float real_t;
#else
  typedef double real_t;
#endif

// ----------------------------------------------------------------

void _vecadd(real_t * a, real_t * b, real_t * c, int N)
{

#pragma acc parallel present(a, b, c)
  {
#pragma acc loop
    for(int i=0; i<N; ++i) {
      c[i] = a[i] + b[i];
    }
  }
  
}

// ----------------------------------------------------------------
						 
int main( int argc, char* argv[] )
{
  MPI_Init(&argc, &argv);

  int me,nranks;
  MPI_Comm_size(MPI_COMM_WORLD, &nranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &me);

  acc_init(acc_device_nvidia);

  const int N = _N;

  real_t * a = (real_t*) malloc(N*sizeof(real_t));
  real_t * b = (real_t*) malloc(N*sizeof(real_t));
  real_t * c = (real_t*) malloc(N*sizeof(real_t));

  // Initialize host
  for(int i=0; i<N; ++i) {
    a[i] = sin(i)*sin(i);
    b[i] = cos(i)*cos(i);
    c[i] = -1.0;
  }

  int num_devices = acc_get_num_devices(acc_device_nvidia);

  if(me == 0) {
    printf("# of devices= %i\n",num_devices);
  }

  // Device ID

  int device_id = me % num_devices;

  acc_set_device_num(device_id, acc_device_nvidia);
  
  for(int i=0; i<nranks; ++i) {
    if(i == me) {
      printf("Rank %i running on GPU %i!\n",me,device_id);
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
  
#ifdef _SINGLE_PRECISION
  if(me == 0) printf("Using single-precision\n\n");
#else
  if(me == 0) printf("Using double-precision\n\n");
#endif

  // Create device buffers and transfer data to device
  
#pragma acc enter data copyin( a[:N], b[:N], c[:N] )

  // Execute kernel

  _vecadd(a, b, c, N);

  // Transfer data from device

#pragma acc exit data copyout( c[:N] )

  //Check result on host

  double diff = 0;
  for(int i=0; i<N; ++i) diff += (double) c[i];
  diff = diff/(double) N - 1.0;

  double diffsq = diff * diff;

  // Clean up

#pragma acc exit data delete (a, b, c)

  free(a);
  free(b);
  free(c);

  double sum;
  MPI_Reduce(&diffsq, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if(me == 0) {
    if(sum < 1e-6) printf("\nResult is CORRECT!! :)\n");
    else printf("\nResult is WRONG!! :(\n");
  }

  MPI_Finalize();
}
