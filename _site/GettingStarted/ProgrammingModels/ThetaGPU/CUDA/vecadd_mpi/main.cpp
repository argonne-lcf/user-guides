#include <stdio.h>
#include "mpi.h"

extern double offload(int, int);

// ----------------------------------------------------------------
						 
int main( int argc, char* argv[] )
{
  MPI_Init(&argc, &argv);

  int me,nranks;
  MPI_Comm_size(MPI_COMM_WORLD, &nranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &me);

  double result_ = offload(me,nranks);

  double sum;
  MPI_Reduce(&result_, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if(me == 0) {
    if(sum < 1e-6) printf("\nResult is CORRECT!! :)\n");
    else printf("\nResult is WRONG!! :(\n");
  }

  MPI_Finalize();
}
