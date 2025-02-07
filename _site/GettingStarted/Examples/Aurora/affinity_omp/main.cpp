#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <mpi.h>
#include <omp.h>

// xthi.c from http://docs.cray.com/books/S-2496-4101/html-S-2496-4101/cnlexamples.html

// util-linux-2.13-pre7/schedutils/taskset.c
void get_cores(char *str)
{
  cpu_set_t mask;
  sched_getaffinity(0, sizeof(cpu_set_t), &mask);

  char *ptr = str;
  int i, j, entry_made = 0;
  for (i = 0; i < CPU_SETSIZE; i++) {
    if (CPU_ISSET(i, &mask)) {
      int run = 0;
      entry_made = 1;
      for (j = i + 1; j < CPU_SETSIZE; j++) {
        if (CPU_ISSET(j, &mask)) run++;
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
}

int main(int argc, char *argv[])
{
  int rnk,nrnk;
  char nname[16];
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rnk);
  MPI_Comm_size(MPI_COMM_WORLD, &nrnk);
  gethostname(nname, 16);

  for(int i=0; i<nrnk; ++i) {
    if(i == rnk) { 
      #pragma omp parallel for ordered
      for(int it=0; it<omp_get_num_threads(); ++it) {
        char list_cores[7*CPU_SETSIZE];
        get_cores(list_cores);
        #pragma omp ordered
        printf("To affinity and beyond!! nname= %s  rnk= %d  tid= %d: list_cores= (%s)\n",
                nname, rnk, omp_get_thread_num(), list_cores);
      }
      printf("\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
  MPI_Finalize();
}
