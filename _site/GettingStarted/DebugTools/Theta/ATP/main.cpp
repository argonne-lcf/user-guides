#include <mpi.h>
#include "stdio.h"
#include "stdlib.h"
#define N 10

int main( int argc, char *argv[] )
{
  int myrank, nranks, i, sum, value;
  int *a = NULL;
  int *b = NULL;

  MPI_Init( &argc, &argv );

  MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
  MPI_Comm_size( MPI_COMM_WORLD, &nranks );

  a = (int *)malloc( N*sizeof(int));

  // all ranks initialize
  for( i=0; i < N; i++ )
    {
      a[i] = i;
    }

  value = 0;

  // each rank only sums up part of the array
  for( i=myrank; i < N; i+=nranks )
    {
      // adding a bug in. this if statement tries to reference      
      // array "b", which was never malloc'd.
      // comment out the following if statement to remove the bug.
      if( i == N/2 ) 
	value += b[i];

	value += a[i];
    }

  MPI_Allreduce( &value, &sum, 1, MPI_INT, MPI_SUM,
                 MPI_COMM_WORLD );

  if( sum != (N*(N-1)/2) )
    {
      if( myrank == 0 ) printf( "passed!\n" );
    }
  else
    {
      if( myrank == 0 ) printf( "failed!\n" );
    }

  free( a );
  MPI_Finalize();
  return 0;

}
