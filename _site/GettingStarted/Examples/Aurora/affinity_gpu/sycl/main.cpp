#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <time.h>
#include <mpi.h>
#include <iomanip>
#include <sycl/sycl.hpp>

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

void uuid_print(std::array<unsigned char, 16> a){
  std::vector<std::tuple<int, int> > r = {{0,4}, {4,6}, {6,8}, {8,10}, {10,16}};
  int first_time = 0;
  for (auto t : r){
    if(first_time > 1) printf("-");
    first_time++;
    for (int i = std::get<0>(t); i < std::get<1>(t); i++)
      printf( "%02x", (unsigned)(unsigned char)a[i]);

  }
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

  // make an array of all the devices available
  std::vector<sycl::device> devices = sycl::device::get_devices();
  int num_devices = devices.size();

  for(int ir=0; ir<nprocs; ++ir) {

    if(ir == rnk) {

        char list_cores[7*CPU_SETSIZE];
        get_cores(list_cores);

        printf("To affinity and beyond!! nname= %s  rnk= %d  list_cores= (%s)  num_devices= %i  gpu_uuids= ",
               nname, rnk, list_cores, num_devices);


      int device_index =0;
      for(auto &device : devices) {

	// Make a queue that can sends commands to each device.
	// Note that if you make a queue like: "sycl::queue queue(sycl::gpu_selector_v)", you
	// get one queue that feeds into one device as well.

	sycl::queue q(device);
        if (q.get_device().has(sycl::aspect::ext_intel_device_info_uuid)) {
          uuid_print(q.get_device().get_info<sycl::ext::intel::info::device::uuid>());
        }
	else {
	  printf("No ability to get UUID from this device");
	  }
        device_index++;
	if( device_index != num_devices ) printf(", ") ;

      }
	printf("\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);
  }
  
  MPI_Finalize();
}
