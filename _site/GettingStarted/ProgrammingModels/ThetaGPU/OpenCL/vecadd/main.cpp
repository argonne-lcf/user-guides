#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <cassert>

#include <CL/opencl.h>

#define _N 1024
#define _LOCAL_SIZE 64
#define _GPU

#ifdef _SINGLE_PRECISION
  typedef float real_t;
#else
  typedef double real_t;
#endif

// copied from intel_ocl_samples/CapsBasic
#define _OCL_CHECK_ERRORS(ERR)		   \
  if(ERR != CL_SUCCESS) {		   \
    std::cout				   \
      << "OpenCL error with code " << ERR  \
      << " happened in file " << __FILE__  \
      << " at line " << __LINE__	   \
      << ". Exiting...\n";		   \
    exit(1);				   \
  }

#define _OCL_PRINT_TEXT_PROPERTY(DEVICE, NAME)		\
  {							\
    /* When we query for string properties, first we */	\
    /* need to get string length:                    */	\
    size_t property_length = 0;				\
    cl_int err = clGetDeviceInfo(			\
				 DEVICE,		\
				 NAME,			\
				 0,			\
				 0,			\
				 &property_length);	\
    _OCL_CHECK_ERRORS(err);				\
    /* Then allocate buffer. No need to add 1 symbol */	\
    /* to store terminating zero; OpenCL takes care  */	\
    /* about it:                                     */	\
    char* property_value = new char[property_length];	\
    err = clGetDeviceInfo(				\
			  DEVICE,			\
			  NAME,				\
			  property_length,		\
			  property_value,		\
			  0);				\
    _OCL_CHECK_ERRORS(err);				\
    std::cout						\
      << "    " << #NAME << ": "			\
      << property_value << std::endl;			\
    delete [] property_value;				\
  }

#define _OCL_PRINT_NUMERIC_PROPERTY(DEVICE, NAME, TYPE)		 \
  {								 \
    TYPE property_value;					 \
    size_t property_length = 0;					 \
    cl_int err = clGetDeviceInfo(				 \
				 DEVICE,			 \
				 NAME,				 \
				 sizeof(property_value),	 \
				 &property_value,		 \
				 &property_length);		 \
    assert(property_length == sizeof(property_value));		 \
    _OCL_CHECK_ERRORS(err);					 \
    std::cout							 \
      << "    " << #NAME << ": "				 \
      << property_value << std::endl;				 \
  }

// ----------------------------------------------------------------

// kernel to offload
  
#ifdef _SINGLE_PRECISION
  
const char * kernel_source =                  "\n" \
"__kernel void vecadd(  __global float * a,    \n" \
"                       __global float * b,    \n" \
"                       __global float * c,    \n" \
"                       const int n)           \n" \
"{                                             \n" \
"    //global thread ID                        \n" \
"    int id = get_global_id(0);                \n" \
"                                              \n" \
"    if (id < n) c[id] = a[id] + b[id];        \n" \
"}                                             \n" ;

#else
						 
const char * kernel_source =                  "\n" \
"#pragma OPENCL EXTENSION cl_khr_fp64 : enable \n" \
"__kernel void vecadd(  __global double * a,   \n" \
"                       __global double * b,   \n" \
"                       __global double * c,   \n" \
"                       const int n)           \n" \
"{                                             \n" \
"    //global thread ID                        \n" \
"    int id = get_global_id(0);                \n" \
"                                              \n" \
"    if (id < n) c[id] = a[id] + b[id];        \n" \
"}                                             \n" ;

#endif
 
// ----------------------------------------------------------------
						 
int main( int argc, char* argv[] )
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
  size_t global_size = ceil(N / (double)_LOCAL_SIZE) * _LOCAL_SIZE;
  size_t local_size = _LOCAL_SIZE;
  
  // OpenCL Platform
  cl_platform_id platform;
  cl_int err = clGetPlatformIDs(1, &platform, NULL);
  
  // Device ID
  cl_device_id device;
#if defined(_GPU)
  printf("Running on GPU!\n");
  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
#else
  printf("Running on CPU!\n");
  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
#endif
  _OCL_CHECK_ERRORS(err);

  
#ifdef _SINGLE_PRECISION
  printf("Using single-precision\n\n");
#else
  printf("Using double-precision\n\n");
#endif
  
  // check opencl version
  _OCL_PRINT_TEXT_PROPERTY(device, CL_DEVICE_NAME);
  _OCL_PRINT_TEXT_PROPERTY(device, CL_DEVICE_VERSION);
  _OCL_PRINT_TEXT_PROPERTY(device, CL_DEVICE_OPENCL_C_VERSION);
  
  _OCL_PRINT_NUMERIC_PROPERTY(device, CL_DEVICE_MAX_COMPUTE_UNITS, cl_uint);
  _OCL_PRINT_NUMERIC_PROPERTY(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, cl_uint);
  _OCL_PRINT_NUMERIC_PROPERTY(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, size_t);
  
  // OpenCL context 
  cl_context context;
  context = clCreateContext(0, 1, &device, NULL, NULL, &err);
  _OCL_CHECK_ERRORS(err);
  
  // OpenCL queue
  cl_command_queue queue; 
  queue = clCreateCommandQueue(context, device, 0, &err);
  _OCL_CHECK_ERRORS(err);
  
  // Program from source
  cl_program program;
  program = clCreateProgramWithSource(context, 1, (const char **) & kernel_source, NULL, &err);
  _OCL_CHECK_ERRORS(err);
  clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  
  // Create kernel 
  cl_kernel kernel;
  kernel = clCreateKernel(program, "vecadd", &err);
  _OCL_CHECK_ERRORS(err);
  
  // Create device buffers
  cl_mem d_a;
  cl_mem d_b;
  cl_mem d_c;

  int size = N * sizeof(real_t);
  
  d_a = clCreateBuffer(context, CL_MEM_READ_ONLY,  size, NULL, NULL);
  d_b = clCreateBuffer(context, CL_MEM_READ_ONLY,  size, NULL, NULL);
  d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size, NULL, NULL);
  
  // Transfer data to device
  err  = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0, size, a, 0, NULL, NULL);
  err |= clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0, size, b, 0, NULL, NULL);
  _OCL_CHECK_ERRORS(err);
  
  // Set arguments
  err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
  err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
  err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
  err |= clSetKernelArg(kernel, 3, sizeof(int),    &N);
  _OCL_CHECK_ERRORS(err);
  
  // Execute kernel 
  err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
  _OCL_CHECK_ERRORS(err);
  
  // Synchronize
  clFinish(queue);
  
  // Transfer data from device
  err = clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0, size, c, 0, NULL, NULL );
  _OCL_CHECK_ERRORS(err);
  
  //Check result on host
  double diff = 0;
  for(int i=0; i<N; ++i) diff += (double) c[i];
  diff = diff/(double) N - 1.0;
  
  if(diff*diff < 1e-6) printf("\nResult is CORRECT!! :)\n");
  else printf("\nResult is WRONG!! :(\n");
  
  // Clean up
  clReleaseMemObject(d_a);
  clReleaseMemObject(d_b);
  clReleaseMemObject(d_c);
  
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);
  
  free(a);
  free(b);
  free(c);
}


