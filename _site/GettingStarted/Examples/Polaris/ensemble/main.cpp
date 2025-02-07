#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <time.h>

#include <iomanip>
#include <vector>
#include <tuple>

#include <mpi.h>
#include <omp.h>

//https://stackoverflow.com/questions/68823023/set-cuda-device-by-uuid
void uuid_print(cudaUUID_t a){
  std::cout << "GPU";
  std::vector<std::tuple<int, int> > r = {{0,4}, {4,6}, {6,8}, {8,10}, {10,16}};
  for (auto t : r){
    std::cout << "-";
    for (int i = std::get<0>(t); i < std::get<1>(t); i++)
      std::cout << std::hex << std::setfill('0') << std::setw(2) << (unsigned)(unsigned char)a.bytes[i];
  }
  std::cout << std::endl;
}

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
  // Initialize MPI
  
  int rnk, nprocs;
  MPI_Init(&argc, &argv);
  MPI_Comm world = MPI_COMM_WORLD;
  
  MPI_Comm_rank(world, &rnk);
  MPI_Comm_size(world, &nprocs);

  char nname[16];
  gethostname(nname, 16);
 
  // Initialize gpu

  int num_devices;
  cudaGetDeviceCount(&num_devices); // should just be one per MPI rank

  if(rnk == 0) printf("rnk= %i :  # of devices detected= %i\n",rnk, num_devices);
  MPI_Barrier(MPI_COMM_WORLD);
  
  for(int ir=0; ir<nprocs; ++ir) {

    if(ir == rnk) {

      char list_cores[7*CPU_SETSIZE];
      get_cores(list_cores);
      
      printf("To affinity and beyond!! nname= %s  rnk= %d  list_cores= (%s)  num_devices= %i\n",
	     nname, rnk, list_cores, num_devices);

      #pragma omp parallel for ordered
      for(int it=0; it<omp_get_num_threads(); ++it) {
        char list_cores[7*CPU_SETSIZE];
        get_cores(list_cores);
        #pragma omp ordered
        printf("To affinity and beyond!! nname= %s  rnk= %d  tid= %d: list_cores= (%s)\n",
                nname, rnk, omp_get_thread_num(), list_cores);
      }
      printf("\n");

      for(int id=0; id<num_devices; ++id) {	
	cudaDeviceProp prop;
	cudaGetDeviceProperties(&prop, id);
	printf("    [%i,%i] Platform[ Nvidia ] Type[ GPU ] Device[ %s ]  uuid= ",rnk, id, prop.name);
	uuid_print(prop.uuid);
	printf("\n");
      }
      printf("\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);
  }
  
  MPI_Finalize();
}
