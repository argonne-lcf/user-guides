# Overview

The OpenMP API is an open standard for parallel programming. The specification document can be found here: https://www.openmp.org/. The specification describes directives, runtime routines, and environment variables that allow an application developer to express parallelism (e.g. shared memory multiprocessing and device offloading). Many compiler vendors provide implementations of the OpenMP specification. 

# Using OpenMP at ALCF

On Theta, all of the standard programming environments (PrgEnv-cray, PrgEnv-intel, PrgEnv-gnu, PrgEnv-llvm modules) support OpenMP. An application can be compiled with OpenMP support using the flags listed below in the “Building on Theta” section for each vendor. At runtime, how OpenMP threads are mapped to physical cores can be controlled with environment variables and the aprun command, as discussed below in the “Running Jobs with OpenMP on Theta” section.

ALCF provides assistance with build instructions, compiling executables, submitting jobs, and providing prebuilt binaries. For questions, contact us at support@alcf.anl.gov.

# Building on Theta

To enable OpenMP, use the following flags in your compile line, depending on the programming environment you have loaded:

## Compiler flags
| Vendor       | Programming Environment Module	| Flag to enable OpenMP
| -------------|  -------------                 |  -------------   
| Intel	       | PrgEnv-intel                   |   “-qopenmp"
| Cray (9.0.2) | PrgEnv-cray                    |   “-fopenmp”
| GNU          | PrgEnv-gnu                     |   "-fopenmp"
| LLVM         | PrgEnv-llvm                    |   "-fopenmp"


# Running Jobs with OpenMP on Theta

To run jobs on Theta with OpenMP threads, the OpenMP environment variable OMP_NUM_THREADS will need to be set to the desired number of threads per MPI rank, and certain flags in the aprun command will need to be set. Some examples are given below, and more information about running is here: https://www.alcf.anl.gov/support-center/theta/affinity-theta.

Source code for xthi.c:

```
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <mpi.h>
#include <omp.h>

/* Borrowed from util-linux-2.13-pre7/schedutils/taskset.c */
static char *cpuset_to_cstr(cpu_set_t *mask, char *str)
{
  char *ptr = str;
  int i, j, entry_made = 0;
  for (i = 0; i < CPU_SETSIZE; i++) {
    if (CPU_ISSET(i, mask)) {
      int run = 0;
      entry_made = 1;
      for (j = i + 1; j < CPU_SETSIZE; j++) {
        if (CPU_ISSET(j, mask)) run++;
        else break;
      }
      if (!run)
        sprintf(ptr, "%d,", i);
      else if (run == 1) {
        sprintf(ptr, "%d,%d,", i, i + 1);
        i++;
      } else {
        sprintf(ptr, "%d-%d,", i, i + run);
        i += run;
      }
      while (*ptr != 0) ptr++;
    }
  }
  ptr -= entry_made;
  *ptr = 0;
  return(str);
}

int main(int argc, char *argv[])
{
  int rank, thread;
  cpu_set_t coremask;
  char clbuf[7 * CPU_SETSIZE], hnbuf[64];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  memset(clbuf, 0, sizeof(clbuf));
  memset(hnbuf, 0, sizeof(hnbuf));
  (void)gethostname(hnbuf, sizeof(hnbuf));
#pragma omp parallel private(thread, coremask, clbuf)
  {
    thread = omp_get_thread_num();
    (void)sched_getaffinity(0, sizeof(coremask), &coremask);
    cpuset_to_cstr(&coremask, clbuf);
    #pragma omp barrier
    printf("Hello from rank %d, thread %d, on %s. (core affinity = %s)\n",
           rank, thread, hnbuf, clbuf);
  }
  MPI_Finalize();
  return(0);
}
```

1. Compile with appropriate flag for the Programming Environment

` $ cc -qopenmp xthi.c -o xthi # PrgEnv-intel`

2. Run with aprun (either in a batch script that is submitted to the job scheduler a or on the command line as part of an interactive session. See https://www.alcf.anl.gov/support-center/theta/running-jobs-and-submission-scripts for more details about how to run.)

Mapping of OpenMP threads to hardware threads on a KNL node can be achieved with `--cc` option in `aprun`.

One common option (described in more detail here: https://www.alcf.anl.gov/support-center/theta/affinity-theta) is to use `--cc depth` with the `-d` and `-j` flags:
```
$ aprun -n 1 -N 1 -d 8 -j 1 -cc depth -e OMP_NUM_THREADS=8 ./a.out 
Hello from rank 0, thread 5, on nid03554. (core affinity = 5)
Hello from rank 0, thread 0, on nid03554. (core affinity = 0)
Hello from rank 0, thread 3, on nid03554. (core affinity = 3)
Hello from rank 0, thread 4, on nid03554. (core affinity = 4)
Hello from rank 0, thread 2, on nid03554. (core affinity = 2)
Hello from rank 0, thread 7, on nid03554. (core affinity = 7)
Hello from rank 0, thread 1, on nid03554. (core affinity = 1)
Hello from rank 0, thread 6, on nid03554. (core affinity = 6)
Application 19165961 resources: utime ~1s, stime ~1s, Rss ~6284, inblocks ~0, outblocks ~8
```

Another option is to use `--cc none` with OpenMP affinity controls: 
```
$ aprun -n 1 -N 1 -cc none -e OMP_NUM_THREADS=8 -e OMP_PROC_BIND=spread -e OMP_PLACES=cores ./a.out
Hello from rank 0, thread 0, on nid03554. (core affinity = 0,64,128,192)
Hello from rank 0, thread 5, on nid03554. (core affinity = 40,104,168,232)
Hello from rank 0, thread 1, on nid03554. (core affinity = 8,72,136,200)
Hello from rank 0, thread 2, on nid03554. (core affinity = 16,80,144,208)
Hello from rank 0, thread 4, on nid03554. (core affinity = 32,96,160,224)
Hello from rank 0, thread 3, on nid03554. (core affinity = 24,88,152,216)
Hello from rank 0, thread 7, on nid03554. (core affinity = 56,120,184,248)
Hello from rank 0, thread 6, on nid03554. (core affinity = 48,112,176,240)
Application 19165962 resources: utime ~0s, stime ~1s, Rss ~6284, inblocks ~0, outblocks ~8
```

# Performance Notes

Affinity of OpenMP threads to hardware threads can have a large impact on performance. Please see https://www.alcf.anl.gov/support-center/theta/affinity-theta for more information. 
 
