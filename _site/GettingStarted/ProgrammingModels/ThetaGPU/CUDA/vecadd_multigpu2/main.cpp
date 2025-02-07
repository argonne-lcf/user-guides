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

// kernels to offload
  
__global__ void _vecadd_2(real_t * res, real_t * a, real_t * b, int n)
{
  int id = blockIdx.x * blockDim.x + threadIdx.x;

  if(id < n) res[id] = a[id] + b[id];
}

__global__ void _vecadd_1(real_t * res, real_t * a, int n)
{
  int id = blockIdx.x * blockDim.x + threadIdx.x;

  if(id < n) res[id] += a[id];
}
 
// ----------------------------------------------------------------

// Helper functions

void gpu_barrier(int n, cudaStream_t * s)
{

  for(int i=0; i<n; ++i) {
    cudaSetDevice(i);
    cudaStreamSynchronize(s[i]);
  }
  
}

// ----------------------------------------------------------------
						 
int main( int argc, char* argv[] )
{
  
  int num_devices;
  cudaGetDeviceCount(&num_devices);
  _CUDA_CHECK_ERRORS();
  printf("# of devices detected= %i\n",num_devices);
  
  int ngpus = num_devices;
  
  int pos = 1;
  while(pos < argc) {
    if(strcmp(argv[pos],"-ngpus") == 0) {
      ngpus = atoi(argv[pos+1]);
      pos++;
    }
    
    pos++;
  }

  // check each gpu can access peer device

  int err = 0;
  printf("\nChecking P2P Access\n");
  for(int ig=0; ig<ngpus; ++ig) {
    cudaSetDevice(ig);
    printf("Device i= %i\n",ig);
    
    int n = 1;
    for(int jg=0; jg<ngpus; ++jg) {
      if(jg != ig) {
	int access;
	cudaDeviceCanAccessPeer(&access, ig, jg);
	n += access;

	printf("  --  Device j= %i  access= %i\n",jg,access);
      }
    }
    if(n != ngpus) err += 1;
  }
  if(err) {
    printf("P2P Access not enabled\n");
    exit(1);
  } else printf("P2P Access is enabled\n");
  printf("\n");
  
  // Allocate memory on host

  const int N = _N;
  
  real_t * a = (real_t*) malloc(N*sizeof(real_t));
  real_t * b = (real_t*) malloc(N*sizeof(real_t));
  real_t * c = (real_t*) malloc(N*sizeof(real_t));
  real_t * r = (real_t*) malloc(N*sizeof(real_t));
  
  // Initialize host
  for(int i=0; i<N; ++i) {
    a[i] = sin(i)*sin(i);
    b[i] = cos(i)*cos(i);
    c[i] = -1.0;
    r[i] = 0.0;
  }
  
  // Number of total work items
  
  size_t grid_size = (int)ceil((_N)/_LOCAL_SIZE);
  size_t block_size = _LOCAL_SIZE;
  
  // Cuda Properties
  
  for(int i=0; i<num_devices; ++i) {
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, i);
    _CUDA_CHECK_ERRORS();
    
    char name[256];
    strcpy(name, prop.name);
    
    printf("  [%i] Platform[ Nvidia ] Type[ GPU ] Device[ %s ]\n", i, name);
  }  
  
  // Device ID
  
  printf("\nRunning on %i GPUs!\n",ngpus);
  
#ifdef _SINGLE_PRECISION
  printf("Using single-precision\n\n");
#else
  printf("Using double-precision\n\n");
#endif
  
  // check cuda version on first gpu
  
  int device_id = 0;
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device_id);
  _CUDA_CHECK_ERRORS();

  printf("  Info from device 0\n");
  printf("  Name= %s\n",prop.name);
  printf("  Locally unique identifier= %s\n",prop.luid);
  
  printf("  Clock Frequency(KHz)= %i\n",prop.clockRate);
  printf("  Compute Mode= %i\n",prop.computeMode);
  printf("  Major compute capability= %i\n",prop.major);
  printf("  Minor compute capability= %i\n",prop.minor);
  
  printf("  Number of multiprocessors on device= %i\n",prop.multiProcessorCount);
  printf("  Warp size in threads= %i\n",prop.warpSize);
  printf("  Single precision performance ratio= %i\n",prop.singleToDoublePrecisionPerfRatio);
  
  // Create device buffers
  
  real_t ** d_a = (real_t**) malloc(ngpus*sizeof(real_t*));
  real_t ** d_b = (real_t**) malloc(ngpus*sizeof(real_t*));
  real_t ** d_c = (real_t**) malloc(ngpus*sizeof(real_t*));
  real_t ** d_buf = (real_t**) malloc(ngpus*sizeof(real_t*));

  int size = N * sizeof(real_t);

  cudaStream_t * s = (cudaStream_t*) malloc(ngpus*sizeof(cudaStream_t));

  // Transfer data to device
  
  for(int ig=0; ig<ngpus; ++ig) {

    cudaSetDevice(ig);

    cudaStreamCreate(&s[ig]);
  
    cudaMalloc(&d_a[ig], size);
    cudaMalloc(&d_b[ig], size);
    cudaMalloc(&d_c[ig], size);
    cudaMalloc(&d_buf[ig], size);
    
    // Transfer data to device
    
    cudaMemcpyAsync(d_a[ig], a, size, cudaMemcpyHostToDevice, s[ig]);  _CUDA_CHECK_ERRORS();
    cudaMemcpyAsync(d_b[ig], b, size, cudaMemcpyHostToDevice, s[ig]);  _CUDA_CHECK_ERRORS();
    cudaMemcpyAsync(d_c[ig], c, size, cudaMemcpyHostToDevice, s[ig]);  _CUDA_CHECK_ERRORS();
  }

  // device barrier

  gpu_barrier(ngpus, s);

  // Execute initial compute kernel
  
  for(int ig=0; ig<ngpus; ++ig) {
    cudaSetDevice(ig);
    
    _vecadd_2<<<grid_size, block_size, 0, s[ig]>>>(d_c[ig], d_a[ig], d_b[ig], _N);
    _CUDA_CHECK_ERRORS();
  }

  // device barrier
  
  gpu_barrier(ngpus, s);


  // ----------------------------------------------------------------
  // Binary tree reduction
  // ----------------------------------------------------------------

  int ngpus_recv = num_devices / 2;

  int ngpus_active = ngpus;

  int il = 0;
  while(ngpus_recv > 0) {

    printf("\n GPU-GPU Reduction  il= %i  ngpus_active= %i  ngpu_recv= %i\n",il,ngpus_active,ngpus_recv);
    if(ngpus_active > ngpus_recv) {
    
      int ngpus_send = ngpus_active - ngpus_recv;
      
      for(int i=0; i<ngpus_send; ++i) {
	
	int dest = i;
	int src = ngpus_recv + i;

	printf("    %i --> %i\n", src, dest);
	
	cudaMemcpyPeerAsync(d_buf[dest], dest, d_c[src], src, size, s[dest]);
	
	cudaSetDevice(dest);
	
	_vecadd_1<<<grid_size, block_size, 0, s[dest]>>>(d_c[dest], d_buf[dest], _N);
      }
      
      ngpus_active = ngpus_recv;
    }

    ngpus_recv /= 2;
    il++;
    
    // device barrier

#if 1
    gpu_barrier(ngpus_active, s);
#endif
  }
  
  // Transfer data from device 0 to host

  int ig = 0;
  cudaSetDevice(ig);
    
  cudaMemcpyAsync(c, d_c[ig], size, cudaMemcpyDeviceToHost, s[ig]); _CUDA_CHECK_ERRORS();
  
  cudaStreamSynchronize(s[ig]);

  for(int i=0; i<N; ++i) r[i] += c[i];

  //Check result on host
  
  double diff = 0;
  double host_result = ngpus * 1.0; // ngpus * (# of reduction levels)
  printf("\n  r[0]= %f  host_result= %f\n",r[0],host_result);
  for(int i=0; i<N; ++i) diff += (r[i] - host_result) * (r[i] - host_result);
  
  if(diff*diff < 1e-6) printf("\nResult from all GPUs is CORRECT!! :)\n");
  else printf("\nResult from all GPUs is WRONG!! :(\n");

  for(int ig=0; ig<ngpus; ++ig) {
    cudaSetDevice(ig);
    
    cudaFree(d_a[ig]);
    cudaFree(d_b[ig]);
    cudaFree(d_c[ig]);
    cudaFree(d_buf[ig]);

    cudaStreamDestroy(s[ig]);
  }
  
  // Clean up

  free(d_a);
  free(d_b);
  free(d_c);
  free(d_buf);
  free(s);
 
  free(a);
  free(b);
  free(c);
  free(r);
}
