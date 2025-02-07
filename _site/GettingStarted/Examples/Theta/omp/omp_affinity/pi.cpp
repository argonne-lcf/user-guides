#include "stdio.h"
#include "omp.h"

int main() 
{    
  double x, pi, sum   =    0.0;
  int    num_steps    =    1000000000;
  double step         =    1.0/(double) num_steps;

  double init_time    =    omp_get_wtime();

  // outer loop is to get timings
  for(int j=0; j<50; j++)
    {
      sum = 0.0;
#pragma omp parallel for private(x) reduction(+:sum)
      for( int i=0; i<=num_steps; i++ )
	{
	  x = (i+0.5) * step;
	  sum =sum + 4.0 / (1.0+x*x);
	}
    }
  double final_time=omp_get_wtime();
  pi = step * sum;

  printf( " ### Time: %lf\n\n", final_time-init_time );
  return 0;
}   
