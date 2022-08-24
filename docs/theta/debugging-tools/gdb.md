# GDB
## Introduction
GDB4HPC is a command-line debugging tool provided by Cray. It works similarly to [GBD](https://www.gnu.org/software/gdb/), but allows the user to debug multiple parallel processes without multiple windows. GDB4HPC can be used to investigate deadlocked code, segfaults, and other errors for C/C++ and Fortran code. Users can single-step code and focus on specific processes groups to help identify unexpected code behavior.

This page focuses on using GDB4HPC during an interactive login session by launching the code through the debugger. (GDB4HPC can also be used to attach to a running process, but this is not covered here.)

## Using GDB4HPC

### A. Launching the code through the debugger

**1. Compile Code**
To add debugging symbols to your code, compile it with the “-g” flag. Note that sometimes it is useful to decrease the compiler optimization level. At higher optimization levels, the compiler will reorganize or optimize out variables, and when you step through the code with the debugger, the line numbers may no longer match what you see.

For example, for source file test.c:
```user@thetalogin1:~> cc -g -O0 test.c```

**2. Log into an interactive session**
To debug interactively, log into an interactive session. An interactive session logs you into a MOM/launch node ("thetamom1" below), where GDB4HPC will be run. (GDB4HPC will iitself launch jobs from the MOM/launch node to the compute nodes.)

For example, submitting a interactive qsub job for 2 nodes in the debug-cache-quad queue:
```
user@thetalogin1:~> qsub -I -n 2 -q debug-cache-quad -t 60 -A <project>
Job routed to queue "debug-cache-quad".
Memory mode set to cache quad for queue debug-cache-quad
Wait for job 314931 to start...
Opening interactive session to 3827
user@thetamom1:~>
```

**3. Setup the environment**
To use GDB4HPC, the appropriate modules should be loaded so that the correct libraries and applications are in your path:
```user@thetamom1:~> module load gdb4hpc```

**4. Start the debugger**
Start the debugger by calling "gdb4hpc" from the shell on the MOM node:
```
user@thetamom1:~> gdb4hpc
gdb4hpc 3.0 - Cray Line Mode Parallel Debugger
With Cray Comparative Debugging Technology.
Copyright 2007-2018 Cray Inc. All Rights Reserved.
Copyright 1996-2016 University of Queensland. All Rights Reserved.
Type "help" for a list of commands.
Type "help <cmd>" for detailed help about a command.
dbg all>
```
**5. Launch the application through the debugger**
As an example, say that we want to debug a job we would normally run with `aprun -n 128 -N 64 ./a.out` on Theta, which launches 128 MPI ranks total, with 64 on each node.

To debug the executable "a.out", the run can be launched with the command `launch $a{128} --launcher-args="-N 64" ./a.out` (as shown below). The "launch" command invokes aprun to run the job on the compute nodes, and then pauses the execution for commands from the user.

A short explanation of the syntax for the launch command: In GDB4HPC, MPI ranks are grouped into process sets. Here the process set that we want to launch is defined as $a{128}, which defines a set of 128 MPI ranks to launch, with the variable array name $a. Additional arguments to the launcher can be passed with the `--launcher-args=` flag. Here, we pass `--launcher-args="-N 64"` to pass the number of ranks per node to the launcher.

```
dbg all> launch $a{128} --launcher-args="-N 64" ./a.out
launch $a{128} --launcher-args="-N 64" ./a.out
Starting application, please wait...
Creating MRNet communication network...
Waiting for debug servers to attach to MRNet communications network...
Timeout in 400 seconds. Please wait for the attach to complete.
Number of dbgsrvs connected: [21];  Timeout Counter: [0]
Number of dbgsrvs connected: [128];  Timeout Counter: [0]
Finalizing setup...
Launch complete.
a{0..127}: Initial breakpoint, main at /gpfs/mira-home/user/test.c:8
dbg all>
```
**6.  Investigating the execution of the program**
Many of the standard gdb commands can be issued to investigate the execution of the code. Several examples are shown below, using the example of a 128 MPI rank-application with 64 ranks per node.

List source lines to see where execution is:
```
a{0..127}: Initial breakpoint, main at /gpfs/mira-home/user/test.c:8
dbg all> list
list
a{0..127}: 8      int *a = NULL;
a{0..127}: 9      int passed = 0;
a{0..127}: 10 
a{0..127}: 11     MPI_Init( &argc, &argv );
a{0..127}: 12 
a{0..127}: 13     MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
a{0..127}: 14     MPI_Comm_size( MPI_COMM_WORLD, &nranks );
a{0..127}: 15 
a{0..127}: 16     a = (int *)malloc( N*sizeof(int));
a{0..127}: 17
dbg all>
```
**Note:** a{0} corresponds to MPI rank 0, and a{0..127} corresponds to all 128 MPI ranks.

### Single step
```
dbg all> step
step
a{0..127}: main at /gpfs/mira-home/user/test.c:9
dbg all> list
list
a{0..127}: 9      int passed = 0;
a{0..127}: 10   
a{0..127}: 11     MPI_Init( &argc, &argv );
a{0..127}: 12   
a{0..127}: 13     MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
a{0..127}: 14     MPI_Comm_size( MPI_COMM_WORLD, &nranks );
a{0..127}: 15   
a{0..127}: 16     a = (int *)malloc( N*sizeof(int));
a{0..127}: 17   
a{0..127}: 18     // all ranks initialize
dbg all>
```

### Set a breakpoint at line 32 of test.c
```
dbg all> break test.c:32
break test.c:32
a{0..127}: Breakpoint 1: file /gpfs/mira-home/user/test.c, line 32.
dbg all>
```

### Continue
```
dbg all> continue
continue
a{0..127}: Breakpoint 1, main at /gpfs/mira-home/user/test.c:32
dbg all> list
list
a{0..127}: 32     if( sum == (N*(N-1)/2) )
a{0..127}: 33       {
a{0..127}: 34         passed = 1;
a{0..127}: 35         printf( "passed for rank %d!", myrank );
a{0..127}: 36       }
a{0..127}: 37     else
a{0..127}: 38       {
a{0..127}: 39         printf( "failed for rank %d!recieved: %d correct: %d",
a{0..127}: 40                 myrank, sum, N*(N-1)/2 );
a{0..127}: 41       }
dbg all>
```

### Backtrace
```
dbg all> backtrace
backtrace
a{0..127}: #0  0x000000000040a7ac in main at /gpfs/mira-home/user/test.c:32
dbg all>
```
### See what all ranks have for variable "value"
```
dbg all> print value
print value
a{104}: 3416
a{105}: 3423
a{106}: 3430
a{107}: 3437
a{108}: 3444
a{109}: 3451
a{110}: 3458
a{111}: 3465
a{112}: 3472
...
dbg all>
```
### See what rank 104 has for variable "value"
```
dbg all> print $a{104}::value
print $a{104}::value
a{104}: 3416
dbg all>
```
The code used in the example is shown below:
```
user@thetamom1:~> cat test.c
cat test.c
#include <mpi.h>
#include "stdio.h"
#define N 1000

int main( int argc, char *argv[] )
{
  int myrank, nranks, i, sum, value;
  int *a = NULL;
  int passed = 0;

  MPI_Init( &argc, &argv );

  MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
  MPI_Comm_size( MPI_COMM_WORLD, &nranks );

  a = (int *)malloc( N*sizeof(int));

  // all ranks initialize
  for( i=0; i < N; i++ )
    a[i] = i;

  value = 0;

  // each rank only sums up part of the array
  for( i=myrank; i < N; i+=nranks )
    value += a[i];

  MPI_Allreduce( &value, &sum, 1, MPI_INT, MPI_SUM,
                 MPI_COMM_WORLD );

  if( sum == (N*(N-1)/2) )
    {
      passed = 1;
      printf( "passed for rank %d!\n", myrank );
    }
  else
    {
      printf( "failed for rank %d!\nrecieved: %d correct: %d\n",
              myrank, sum, N*(N-1)/2 );
    }

  free( a );
  MPI_Finalize();
  return 0;

}
```

# References and Resources
- GDB4HPC manual page on Theta (gdb4hpc module should be loaded first): ```man gdb4hpc```



