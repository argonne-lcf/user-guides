#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <time.h>

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

void dev_properties(int ndev)
{
  int num_devices = omp_get_num_devices();
  int default_device = omp_get_default_device();
  int host = omp_get_initial_device();

  int num_teams = -1;
  int num_threads = -1;
#pragma omp target teams map(tofrom: num_teams, num_threads)
  {
    num_teams = omp_get_num_teams();
    num_threads = omp_get_num_threads();
  }

  const int date = _OPENMP;

  double version;
  if     (date == 201107) version = 3.1;
  else if(date == 201307) version = 4.0;
  else if(date == 201511) version = 4.5;
  else if(date == 201611) version = 5.0;
  else if(date == 201811) version = 5.0;
  else if(date == 202011) version = 5.1;
  else {
    printf("[PM] Error: unknown omp version: omp_data= %i.\n",date);
    exit(1);
  }

  printf("\n  Using OPENMP v%3.1f\n",version);
  printf("  num_devices=     %i\n",num_devices);
  printf("  Default device=  %i\n",default_device);
  printf("  Host=            %i\n",host);
  printf("  num_teams=       %i\n",num_teams);
  printf("  num_threads=     %i\n",num_threads);
}

int main(int argc, char *argv[])
{
  // Initialize MPI
  
  int rnk, nprocs;
  MPI_Init(&argc, &argv);
  MPI_Comm world = MPI_COMM_WORLD;
  
  MPI_Comm_rank(world, &rnk);
  MPI_Comm_size(world, &nprocs);

  char nname[16];
  gethostname(nname, 16);
  
  // Initialize gpu

  int num_devices = omp_get_num_devices();

  int gpu_id = -1;
  #pragma omp target map(tofrom: gpu_id)
  {
    gpu_id = omp_get_device_num();
  }

  if(rnk == 0) dev_properties(gpu_id);

  for(int ir=0; ir<nprocs; ++ir) {

    if(ir == rnk) {

        char list_cores[7*CPU_SETSIZE];
        get_cores(list_cores);

        printf("To affinity and beyond!! nname= %s  rnk= %d  list_cores= (%s)  num_devices= %i  gpu_id= %i\n",
               nname, rnk, list_cores, num_devices, gpu_id);
        //dev_properties(gpu_id);
        printf("\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);
  }
  
//  MPI_Finalize();
}
