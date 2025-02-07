#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <cassert>
#include "mpi.h"

#include <sycl/sycl.hpp>

#define _N 1024
#define _LOCAL_SIZE 64

#ifdef _SINGLE_PRECISION
  typedef float real_t;
#define MY_MPI_TYPE MPI_FLOAT
#else
  typedef double real_t;
#define MY_MPI_TYPE MPI_DOUBLE
#endif

// ----------------------------------------------------------------
						 
int main( int argc, char* argv[] )
{
  MPI_Init(&argc, &argv);

  MPI_Comm world = MPI_COMM_WORLD;
  
  int me, nranks;
  MPI_Comm_size(world, &nranks);
  MPI_Comm_rank(world, &me);

  const int N = _N;

  std::vector<real_t> a(N);
  std::vector<real_t> b(N);
  std::vector<real_t> c(N);
  std::vector<real_t> d(N);

  // Initialize host
  for(int i=0; i<N; ++i) {
    a[i] = sin(i)*sin(i);
    b[i] = cos(i)*cos(i);
    c[i] = -1.0;
    d[i] = 0.0;
  }

  // https://github.com/argonne-lcf/sycltrain/blob/master/9_sycl_of_hell/0_tiny_sycl_info.cpp
  
  if(me == 0) std::cout << "List Platforms and Devices" << std::endl;
  std::vector<sycl::platform> platforms = sycl::platform::get_platforms();
  for (const auto &plat : platforms) {
    // get_info is a template. So we pass the type as an `arguments`.
    if(me == 0) std::cout << "Platform: "
			  << plat.get_info<sycl::info::platform::name>() << " "
			  << plat.get_info<sycl::info::platform::vendor>() << " "
			  << plat.get_info<sycl::info::platform::version>() << std::endl;
    // Trivia: how can we loop over argument?
    
    std::vector<sycl::device> devices = plat.get_devices();
    for (const auto &dev : devices) {
      if(me == 0) std::cout << "-- Device: "
			    << dev.get_info<sycl::info::device::name>() << " "
			    << (dev.is_gpu() ? "is a gpu" : " is not a gpu") << std::endl;
      // sycl::info::device::device_type exist, one can use a Map to translate to a nice sting
      // https://github.com/intel/llvm/blob/ef4e6ddce56fe21621bca7d62932713152de3f0e/sycl/source/detail/device_filter.cpp#L22
    }
  }

  // the following assumes each platform is a single GPU
  int platform_id = 0;
  std::vector<sycl::device> devices = platforms[platform_id].get_devices();
  int num_devices = devices.size();

  // Device ID

  int device_id = me % num_devices;

  sycl::queue Q( devices[device_id] );
  
  for(int i=0; i<nranks; ++i) {
    if(i == 0 && me == 0) {
      printf("Running on %i ranks\n",nranks);
      printf("# of devices= %i\n\n",num_devices);
    }
    if(i == me) printf("Rank %i running on GPU %i!\n",me,device_id);
    MPI_Barrier(MPI_COMM_WORLD);
  }
  
#ifdef _SINGLE_PRECISION
  if(me == 0) printf("Using single-precision\n\n");
#else
  if(me == 0) printf("Using double-precision\n\n");
#endif

  // Create device buffers and transfer data to device

  real_t * d_a = malloc_device<real_t>(N, Q);
  real_t * d_b = malloc_device<real_t>(N, Q);
  real_t * d_c = malloc_device<real_t>(N, Q);
  real_t * d_d = malloc_device<real_t>(N, Q);

  // Transfer host-to-device
  
  Q.memcpy(d_a, a.data(), N*sizeof(real_t));
  Q.memcpy(d_b, b.data(), N*sizeof(real_t));  
  Q.memcpy(d_c, c.data(), N*sizeof(real_t));
  Q.memcpy(d_d, d.data(), N*sizeof(real_t));
  Q.wait();
  
  int send = (me < nranks-1) ? me + 1: 0;
  int recv = (me > 0) ? me - 1: nranks-1;

  //  printf("(%i) sending to %i and receiving from %i\n",me,send,recv);
  
  // Zero array (silly use of MPI, but you get idea...)
  
  if(nranks > 1) {

    // Initialize c array to zero using MPI and gpu buffers
  
    MPI_Request request;
    MPI_Irecv(d_c, N, MY_MPI_TYPE, recv, 0, world, &request);
    MPI_Send(d_d, N, MY_MPI_TYPE, send, 0, world);
    MPI_Wait(&request, MPI_STATUS_IGNORE);
    
  } else {

    Q.memcpy(d_c, d_d, N*sizeof(real_t));
    
  }
  
  // Execute kernel

  Q.submit([&](sycl::handler &cgh) {

    cgh.parallel_for(N, [=](sycl::id<1> idx) {
      d_c[idx] += d_a[idx] + d_b[idx];
	}
      );
    
  });

  Q.wait();
  
  // Transfer data from device

  Q.memcpy(c.data(), d_c, N*sizeof(real_t));

  Q.wait();
  
  //Check result on host

  double diff = 0;
  for(int i=0; i<N; ++i) diff += (double) c[i];
  diff = diff/(double) N - 1.0;

  double diffsq = diff * diff;

  // Clean up

  double sum = 0;
  MPI_Reduce(&diffsq, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  
  if(me == 0) {
    if(sum < 1e-6) printf("\nResult is CORRECT!! :)\n");
    else printf("\nResult is WRONG!! :(\n");
  }

  MPI_Finalize();
}
