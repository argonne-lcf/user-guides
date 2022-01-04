#Example Program and Makefile for Theta

Here is a simple example program and makefile for building on XC40 systems. When you compile your code for our XC40 systems, you are normally cross-compiling for the compute nodes (Intel Xeon Phi 2nd Generation [KNL]), rather than directly compiling for the login nodes (non-KNL). The default, you are using the Intel compiler suite. We also provide the Cray, GNU, and Clang compiler suites via modules.
##C Program
```
/* File pi.c */
#include <mpi.h>
#include <math.h>
#include <stdio.h>

int main(int argc, char** argv)
{
  int n, myid, numprocs, i;
  double PI25DT = 3.141592653589793238462643;
  double mypi, pi, h, sum, x;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  /* Number of intervals */
  n = 25;

  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  h = 1.0 / (double) n;
  sum = 0.0;
  for ( i = myid + 1; i <= n; i += numprocs )
  {
       x = h * ((double) i - 0.5);
       sum += ( 4.0 / (1.0 + x*x) );
  }
  mypi = h * sum;
  MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (myid == 0)
  {
      printf("pi is approximately %.16f, Error is %.16f\n",
               pi, fabs(pi - PI25DT));
  }

  MPI_Finalize();
  return 0;
}
`
##Makefile
`# File makefile
CC = cc

OPTFLAGS = -O3
CFLAGS = $(OPTFLAGS) -g

all: pi

pi: pi.o
	$(CC) $(CFLAGS) -o pi pi.o

pi.o: pi.o
	$(CC) $(CFLAGS) -c pi.c

clean:
	rm pi pi.o
```
##Usage
Session screenshot shows building and running, using an example job script:
```
thetalogin6(pi)> ls
makefile pi.c runpi.bash* 

thetalogin6(pi)> make 
cc -O3 -g -c pi.c 
cc -O3 -g -o pi pi.o 

thetalogin6(pi)> ls 
makefile pi* pi.c pi.o runpi.bash* 
thetalogin6(pi)> cat runpi.bash

 #!/bin/bash 
echo "Starting Cobalt job script" 
export n_nodes=$COBALT_JOBSIZE 
export n_mpi_ranks_per_node=64 
export n_mpi_ranks=$(($n_nodes * $n_mpi_ranks_per_node)) 
aprun -n $n_mpi_ranks -N $n_mpi_ranks_per_node -cc depth -d 4 -j 4 ./pi 

thetalogin6(pi)> qsub -A AProject -n 1 -t 20 runpi.bash 

106040 thetalogin6(pi)> qstat -u user1 

JobID JobName User WallTime QueuedTime RunTime Nodes State attrs Queue Score Project 
====================================================================================
106040 N/A user1 00:20:00 00:00:05 N/A 1 queued {'numa': 'quad', 'mcdram': 'cache'} default 51.0 AProject 

thetalogin6(pi)> qstat -u user1 

JobID JobName User WallTime QueuedTime RunTime Nodes State attrs Queue Score Project 
====================================================================================
106040 N/A user1 00:20:00 00:00:25 00:00:19 1 starting {'numa': 'quad', 'mcdram': 'cache'} default 51.1 AProject 

thetalogin6(pi)> qstat -u user1 

JobID JobName User WallTime QueuedTime RunTime Nodes State attrs Queue Score Project 
====================================================================================
106040 N/A user1 00:20:00 00:00:32 00:00:59 1 running {'numa': 'quad', 'mcdram': 'cache'} default 51.2 AProject 

thetalogin6(pi)> ls -lt 
total 18240 
-rw-r--r-- 1 user1 cobalt 1671 Jun 27 23:08 106040.cobaltlog 
-rw-r--r-- 1 user1 users 185 Jun 27 23:07 106040.output 
-rw-r--r-- 1 user1 users 0 Jun 27 23:07 106040.error 
-rwxr-xr-x 1 user1 users 9300576 Jun 27 23:06 pi* 
-rw-r--r-- 1 user1 users 10256 Jun 27 23:06 pi.o 
-rw-r--r-- 1 user1 users 152 Jun 27 23:05 makefile 
-rwxr-xr-x 1 user1 users 237 Jun 27 23:05 runpi.bash* 
-rw-r--r-- 1 user1 users 816 Jun 27 22:20 pi.c

 thetalogin6(pi)> cat 106040.output 
Starting Cobalt job script pi is approximately 3.1417259869152536, Error is 0.0001333333254605 
Application 3373484 resources: utime ~6s, stime ~6s, Rss ~5036, inblocks ~0, outblocks ~8 
thetalogin6(pi)>
```
