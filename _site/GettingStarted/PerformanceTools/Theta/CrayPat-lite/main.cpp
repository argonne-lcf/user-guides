#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

#define N 200000

int main(int argc, char *argv[])
{
  int rnk, numprocs,sum,sum_over_ranks;
  double average;
  int repeats = 200000;
  int a[N], b[N];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rnk);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

  // initializations
  average = 0;
  for( int j=0; j< N; j++ )
      a[j] = 0;

  for( int  i=0; i< repeats; i++)
    {

      // each rank does a "computation", and stores results in sections of an array
      for( int j=rnk; j< N; j+=numprocs )
	{
	  a[j] = rand() % 20; 
	}

      // each rank sums up the values it "computed" in the array
      sum = 0;
      for( int j=0;j<N;j++ ) {
	sum += a[j];
      }

      // sum up the value over all ranks
      MPI_Allreduce( &sum, &sum_over_ranks, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD );

      average = average + ((double)sum_over_ranks)/N;
    }

  if( rnk == 0 ) printf( "Average: %lf\n", average/repeats ); 

  MPI_Finalize();
}

