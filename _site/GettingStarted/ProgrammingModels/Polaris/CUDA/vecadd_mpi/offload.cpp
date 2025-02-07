#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <cassert>

#define _N 1024
#define _LOCAL_SIZE 64

#ifdef _SINGLE_PRECISION
  typedef float real_t;
#else
  typedef double real_t;
#endif

#define _CUDA_CHECK_ERRORS()               \
{                                          \
  cudaError err = cudaGetLastError();	   \
  if(err != cudaSuccess) {		   \
    std::cout				   \
      << "CUDA error with code "           \
      << cudaGetErrorString(err)	   \
      << " in file " << __FILE__           \
      << " at line " << __LINE__	   \
      << ". Exiting...\n";		   \
    exit(1);				   \
  }                                        \
}

// ----------------------------------------------------------------

// kernel to offload
  
__global__ void _vecadd(real_t * a, real_t * b, real_t * c, int n)
{
  int id = blockIdx.x * blockDim.x + threadIdx.x;

  if(id < n) c[id] = a[id] + b[id];
}
 
// ----------------------------------------------------------------
						 
double offload(int me, int nranks)
{
  // Allocate memory on host

  const int N = _N;
  
  real_t * a = (real_t*) malloc(N*sizeof(real_t));
  real_t * b = (real_t*) malloc(N*sizeof(real_t));
  real_t * c = (real_t*) malloc(N*sizeof(real_t));
  
  // Initialize host
  for(int i=0; i<N; ++i) {
    a[i] = sin(i)*sin(i);
    b[i] = cos(i)*cos(i);
    c[i] = -1.0;
  }
  
  // Number of total work items
  
  size_t grid_size = (int)ceil((_N)/_LOCAL_SIZE);
  size_t block_size = _LOCAL_SIZE;
  
  // Cuda Properties
  int num_devices;
  cudaGetDeviceCount(&num_devices);
  _CUDA_CHECK_ERRORS();

  if(me == 0) {
    printf("# of devices= %i\n",num_devices);
  
    for(int i=0; i<num_devices; ++i) {
      cudaDeviceProp prop;
      cudaGetDeviceProperties(&prop, i);
      _CUDA_CHECK_ERRORS();
    
      char name[256];
      strcpy(name, prop.name);
    
      printf("  [%i] Platform[ Nvidia ] Type[ GPU ] Device[ %s ]\n", i, name);
    }  
  }

  // Device ID
  
  int device_id = me % num_devices;
  printf("Rank %i running on GPU %i!\n",me,device_id);
  
#ifdef _SINGLE_PRECISION
  if(me == 0) printf("Using single-precision\n\n");
#else
  if(me == 0) printf("Using double-precision\n\n");
#endif
  
  // check cuda version
  
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device_id);
  _CUDA_CHECK_ERRORS();
  
  if(me == 0) {
    printf("  Name= %s\n",prop.name);
    printf("  Locally unique identifier= %s\n",prop.luid);
  
    printf("  Clock Frequency(KHz)= %i\n",prop.clockRate);
    printf("  Compute Mode= %i\n",prop.computeMode);
    printf("  Major compute capability= %i\n",prop.major);
    printf("  Minor compute capability= %i\n",prop.minor);
  
    printf("  Number of multiprocessors on device= %i\n",prop.multiProcessorCount);
    printf("  Warp size in threads= %i\n",prop.warpSize);
    printf("  Single precision performance ratio= %i\n",prop.singleToDoublePrecisionPerfRatio);
  }

  // Create device buffers
  
  real_t * d_a;
  real_t * d_b;
  real_t * d_c;

  int size = N * sizeof(real_t);

  cudaMalloc(&d_a, size);
  cudaMalloc(&d_b, size);
  cudaMalloc(&d_c, size);
  
  // Transfer data to device

  cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);  _CUDA_CHECK_ERRORS();
  cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);  _CUDA_CHECK_ERRORS();
  cudaMemcpy(d_c, c, size, cudaMemcpyHostToDevice);  _CUDA_CHECK_ERRORS();
  
  // Execute kernel

  _vecadd<<<grid_size, block_size>>>(d_a, d_b, d_c, _N);
  _CUDA_CHECK_ERRORS();
  
  // Transfer data from device

  cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost); _CUDA_CHECK_ERRORS();
  
  //Check result on host
  
  double diff = 0;
  for(int i=0; i<N; ++i) diff += (double) c[i];
  diff = diff/(double) N - 1.0;
  
  double diffsq = diff * diff;
  
  // Clean up

  cudaFree(d_a);
  cudaFree(d_b);
  cudaFree(d_c);
  
  free(a);
  free(b);
  free(c);

  return diffsq;
}
