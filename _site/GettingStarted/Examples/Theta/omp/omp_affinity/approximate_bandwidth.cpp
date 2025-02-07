#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <assert.h>
#include <omp.h>

#define N 32000000
#define ITERS 400

// dummy function to prevent the compiler from optimizinf
// away the outer timing loop
// THIS SHOULD NEVER BE CALLED
void dummy( double *a, double *b, double *c)
{
  for(int i=0; i< N; i++)
    a[i] = b[i] + c[i];
}

int main( int argc, char *argv[])
{
  double *a = (double *)malloc(N*sizeof(double));
  double *b = (double *)malloc(N*sizeof(double));
  double *c = (double *)malloc(N*sizeof(double));
  double scalar = 3.0;
  int errors = 0;

#pragma omp parallel for
  for( int i = 0; i < N; i++) {
    a[i] = 0;
    b[i] = i;
    c[i] = i;  
  }

  double init_time = omp_get_wtime();

  // outer loop is to get timings
  for(int i = 0; i < ITERS; i++){

#pragma omp parallel for 
    for( int j = 0; j < N; j++){
      a[j] = scalar*b[j] + c[j];
    }

    // this is a dummy routine to prevent the 
    // compiler from optimizing away the outer timing loop 
    if(i < -1 ) dummy(a,b,c);
  }

  double final_time = omp_get_wtime();

  // error checking
  for( int i = 0; i < N; i++) {
    if( abs(a[i] - (scalar*i+i)) > 0.00001 ) errors++;
  }
  
  if( errors > 0) printf("Errors! \n");
  assert( errors == 0);

  printf( " ### Approx. BW: %lf MB/s, Time: %lf\n\n",
	  3*8*N / ((final_time-init_time)/(ITERS))*1.0E-06,
	  final_time-init_time );

  free( a );
  free( b );
  free( c );
  return 0;
}
