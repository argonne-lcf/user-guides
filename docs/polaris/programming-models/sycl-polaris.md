# SYCL

> SYCL (pronounced ‘sickle’) is a royalty-free, cross-platform abstraction layer that enables code for heterogeneous processors to be written using standard ISO C++ with the host and kernel code for an application contained in the same source file.

- Specification: [https://www.khronos.org/sycl/](https://www.khronos.org/sycl/)
- Source code of the compiler: [https://github.com/intel/llvm](https://github.com/intel/llvm)
- ALCF Tutorial: [https://github.com/argonne-lcf/sycltrain](https://github.com/argonne-lcf/sycltrain)

```
module load oneapi/upstream
```

!!! note

    This module (compilers, libraries) is built periodically from the latest open-source rather than releases. For more details on the release version of the compiler, please find the details [here](../compiling-and-linking/oneapi-compiler.md). As such, these compilers will get new features and updates quickly that may break on occasion. Please submit any issues at the respective GitHub repositories for the compilers and libraries.

## Components
- These are the components associated with this module:

| User Application  | Component                                               |
|-------------------|---------------------------------------------------------|
| Compilers         | [DPC++](https://github.com/intel/llvm)                  |
| oneMath Interface | [oneMath](https://github.com/uxlfoundation/onemath)     |
| oneDPL            | [oneDPL](https://github.com/oneapi-src/onedpl)          |
| SYCLomatic/DPCT   | [dpct](https://github.com/oneapi-src/syclomatic)        |

## Dependencies
- The SYCL programming model is supported through `oneapi` compilers that were built from source code.
- Loading this module switches the default programming environment to GNU with the following dependencies:
  - PrgEnv-gnu
  - cuda-PrgEnv-nvidia
- An environment variable is set when loading the module: `ONEAPI_DEVICE_SELECTOR=cuda:gpu`

## Example: How to use SYCL with MPI and OpenMP 

<details>
<summary>Toggle for SYCL example with OpenMP & MPI for CPU-side</summary>

```c++
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <mpi.h>
#include <sched.h>
#include <sycl/sycl.hpp>
#include <omp.h>

// SYCL port of https://code.ornl.gov/olcf/hello_jobstep
// To compile: mpicxx -fsycl -fopenmp -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend --cuda-gpu-arch=sm_80 hello_jobstep.cpp -o hello_jobstep.out

int main(int argc, char *argv[]){

  MPI_Init(&argc, &argv);

  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  char name[MPI_MAX_PROCESSOR_NAME];
  int resultlength;
  MPI_Get_processor_name(name, &resultlength);

  // If CUDA_VISIBLE_DEVICES is set, capture visible GPUs
  const char* gpu_id_list;
  const char* cuda_visible_devices = getenv("CUDA_VISIBLE_DEVICES");
  if(cuda_visible_devices == NULL){
    gpu_id_list = "N/A";
  }
  else{
    gpu_id_list = cuda_visible_devices;
  }

  // Find how many GPUs L0 runtime says are available
  int num_devices = 0;
  std::vector<sycl::device> sycl_all_devs = sycl::device::get_devices(sycl::info::device_type::gpu);
  num_devices = sycl_all_devs.size();

  int hwthread;
  int thread_id = 0;

  if(num_devices == 0){
#pragma omp parallel default(shared) private(hwthread, thread_id)
    {
      thread_id = omp_get_thread_num();
      hwthread = sched_getcpu();

      printf("MPI %03d - OMP %03d - HWT %03d - Node %s\n",
             rank, thread_id, hwthread, name);

    }
  }
  else{

    std::string busid = "";

    std::string busid_list = "";
    std::string rt_gpu_id_list = "";

    // Loop over the GPUs available to each MPI rank
    for(int i=0; i<num_devices; i++){

      // // Get the PCIBusId for each GPU and use it to query for UUID
      busid = sycl_all_devs[i].get_info<sycl::ext::intel::info::device::pci_address>();
      busid_list.append(busid);

      // Concatenate per-MPIrank GPU info into strings for print
      if(i > 0) rt_gpu_id_list.append(",");
      rt_gpu_id_list.append(std::to_string(i));
    }

#pragma omp parallel default(shared) private(hwthread, thread_id)
    {
#pragma omp critical
      {
        thread_id = omp_get_thread_num();
        hwthread = sched_getcpu();

        printf("MPI %03d - OMP %03d - HWT %03d - Node %s - RT_GPU_ID %s - GPU_ID %s - Bus_ID %s\n",
               rank, thread_id, hwthread, name, rt_gpu_id_list.c_str(), gpu_id_list, busid_list.c_str());
      }
    }
  }

  MPI_Finalize();

  return 0;
}
```
</details>

Compile and Run
```bash
$ mpiexec -n 4 --ppn 4 --env OMP_NUM_THREADS=1 ./set_affinity_gpu_polaris.sh ./hello_jobstep.out

MPI 000 - OMP 000 - HWT 000 - Node x3200c0s37b0n0 - RT_GPU_ID 0 - GPU_ID 3 - Bus_ID 0000:C7:00.0
MPI 001 - OMP 000 - HWT 001 - Node x3200c0s37b0n0 - RT_GPU_ID 0 - GPU_ID 2 - Bus_ID 0000:85:00.0
MPI 003 - OMP 000 - HWT 003 - Node x3200c0s37b0n0 - RT_GPU_ID 0 - GPU_ID 0 - Bus_ID 0000:07:00.0
MPI 002 - OMP 000 - HWT 002 - Node x3200c0s37b0n0 - RT_GPU_ID 0 - GPU_ID 1 - Bus_ID 0000:46:00.0
$ ./a.out
```

## Example (using GPU-aware MPI)

```c++
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#include <sycl/sycl.hpp>

// Modified from NERSC website:
// https://docs.nersc.gov/development/programming-models/mpi
int main(int argc, char *argv[]) {

    int myrank, num_ranks;
    double *val_device;
    double *val_host;
    char machine_name[MPI_MAX_PROCESSOR_NAME];
    int name_len=0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
    MPI_Get_processor_name(machine_name, &name_len);

    sycl::queue q{sycl::gpu_selector_v};

    std::cout << "Rank #" << myrank << " runs on: " << machine_name
              << ", uses device: "
              << q.get_device().get_info<sycl::info::device::name>() << "\n";

    MPI_Barrier(MPI_COMM_WORLD);
    int one=1;
    val_host = (double *)malloc(one*sizeof(double));
    val_device = sycl::malloc_device<double>(one,q);

    const size_t size_of_double = sizeof(double);
    *val_host = -1.0;
    if (myrank != 0) {
        std::cout << "I am rank " << myrank
                  << " and my initial value is: " << *val_host << "\n";
    }

    if (myrank == 0) {
        *val_host = 42.0;
        q.memcpy(val_device,val_host,size_of_double).wait();
        std::cout << "I am rank " << myrank
                  << " and will broadcast value: " << *val_host << "\n";
    }

    MPI_Bcast(val_device, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double check = 42.0;
    if (myrank != 0) {
        //Device to Host
        q.memcpy(val_host,val_device,size_of_double).wait();
        assert(*val_host == check);
        std::cout << "I am rank " << myrank
                  << " and received broadcast value: " << *val_host << "\n";
    }

    sycl::free(val_device,q);
    free(val_host);

    MPI_Finalize();

    return 0;
}
```

Load Modules

```bash
module load oneapi/upstream
module load mpiwrappers/cray-mpich-oneapi-upstream
module load craype-accel-nvidia80
export MPICH_GPU_SUPPORT_ENABLED=1
```

Compile and Run

```bash
$ mpicxx -L/opt/cray/pe/mpich/8.1.28/gtl/lib -lmpi_gtl_cuda -std=c++17 -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend --cuda-gpu-arch=sm_80 main.cpp
$ mpiexec -n 2 --ppn 2 --depth=1 --cpu-bind depth ./set_affinity_gpu_polaris.sh ./a.out
```
For further details regarding the arguments passed to the `mpiexec` command shown above, please visit the [Job Scheduling and Execution section](../../running-jobs/index.md). A simple example describing the details and execution of the `set_affinity_gpu_polaris.sh` file can be found [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Polaris/affinity_gpu).

**Note:** By default, there is no GPU-aware MPI library linking support. The example above shows how the user can enable the linking by specifying the path to the GTL (GPU Transport Layer) library (`libmpi_gtl_cuda`) to the link line.

# oneAPI Math Library (oneMath)
[oneMath](https://github.com/uxlfoundation/oneMath) is an open-source implementation of the oneMath interface according to the [oneMath specification](https://oneapi-spec.uxlfoundation.org/specifications/oneapi/latest/elements/onemath/source/). It works with multiple devices (backends) using device-specific libraries underneath.

oneMath is part of oneAPI. Various backends supported are shown below. More information [here](https://github.com/uxlfoundation/oneMath#supported-configurations).

| User Application  | Third-Party Library                                          |
|-------------------|--------------------------------------------------------------|
|                   | [cuBLAS](https://docs.nvidia.com/cuda/cublas/index.html)     |
| oneMath interface | [cuSOLVER](https://docs.nvidia.com/cuda/cusolver/index.html) |
|                   | [cuRAND](https://docs.nvidia.com/cuda/curand/index.html)     |

## Example (using math::gemm)

The following snippet shows how to compile and run a SYCL code with the oneMath library. For instance, a GPU-based GEMM is performed using the `math::gemm` API and the results are compared to a CPU-based GEMM performed using the traditional BLAS (e.g., AOCL-BLIS) library.
```c++
#include <limits>
#include <random>

#include <sycl/sycl.hpp>

#include <oneapi/math.hpp>  // ONEMATH GPU header
#include <cblas.h>         // BLIS   CPU header

// Matrix size constants
#define SIZE 4800 // Must be a multiple of 8.
#define M SIZE / 8
#define N SIZE / 4
#define P SIZE / 2

//////////////////////////////////////////////////////////////////////////////////////////

bool ValueSame(double a, double b) { return std::fabs(a - b) < 1.0e-08; }
int VerifyResult(double *c_A, double *c_B) {
  bool MismatchFound = false;

  for (size_t i = 0; i < M; i++) {
    for (size_t j = 0; j < P; j++) {
      if (!ValueSame(c_A[i * P + j], c_B[i * P + j])) {
        std::cout << "fail - The result is incorrect for element: [" << i << ", " << j
                  << "], expected: " << c_A[i * P + j] << " , but got: " << c_B[i * P + j]
                  << std::endl;
        MismatchFound = true;
      }
    }
  }

  if (!MismatchFound) {
    std::cout << "SUCCESS - The results are correct!" << std::endl;
    return 0;
  } else {
    std::cout << "FAIL - The results mis-match!" << std::endl;
    return -1;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////

int main() {
  std::random_device rd;  // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<> dis(1.0, 2.0);

  // C = alpha * op(A) * op(B)  + beta * C
  oneapi::math::transpose transA = oneapi::math::transpose::nontrans;
  oneapi::math::transpose transB = oneapi::math::transpose::nontrans;

  // matrix data sizes
  int m = M;
  int n = P;
  int k = N;

  // leading dimensions of data
  int ldA = k;
  int ldB = n;
  int ldC = n;

  // set scalar fp values
  double alpha = 1.0;
  double beta = 0.0;

  // 1D arrays on host side
  double *A;
  double *B;
  double *C_host_onemath, *C_cblas;

  A = new double[M * N]{};
  B = new double[N * P]{};
  C_cblas = new double[M * P]{};
  C_host_onemath = new double[M * P]{};

  // prepare matrix data with ROW-major style
  // A(M, N)
  for (size_t i = 0; i < M; i++)
    for (size_t j = 0; j < N; j++)
      A[i * N + j] = dis(gen);
  // B(N, P)
  for (size_t i = 0; i < N; i++)
    for (size_t j = 0; j < P; j++)
      B[i * P + j] = dis(gen);

  std::cout << "Problem size: c(" << M << "," << P << ") = a(" << M << "," << N << ") * b(" << N
            << "," << P << ")" << std::endl;

  // Resultant matrix: C_cblas
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A, ldA, B, ldB, beta,
              C_cblas, ldC);

  // Resultant matrix: C_onemath
  sycl::queue q(sycl::property_list{sycl::property::queue::in_order{}});
  std::cout << "Device: " << q.get_device().get_info<sycl::info::device::name>() << std::endl << std::endl;

  double* A_dev        = sycl::malloc_device<double>(M*N, q);
  double* B_dev        = sycl::malloc_device<double>(N*P, q);
  double* C_dev_onemath = sycl::malloc_device<double>(M*P, q);

  q.memcpy(A_dev, A, (M*N) * sizeof(double));
  q.memcpy(B_dev, B, (N*P) * sizeof(double));

  auto gemm_event = oneapi::math::blas::column_major::gemm(q, transB, transA, n, m, k, alpha, B_dev, ldB, A_dev, ldA, beta, C_dev_onemath, ldC);

  q.memcpy(C_host_onemath, C_dev_onemath, (M*P) * sizeof(double));

  q.wait();
  std::cout << "Verify results between Onemath & CBLAS: ";
  int result_cblas = VerifyResult(C_cblas, C_host_onemath);

  delete[] A;
  delete[] B;
  delete[] C_cblas;
  delete[] C_host_onemath;
  sycl::free(A_dev, q);
  sycl::free(B_dev, q);
  sycl::free(C_dev_onemath, q);
  return result_cblas;
}
```

Compile and Run

The user would need to provide paths to the math libraries as shown below. Also, please provide the AOCL library for CPU GEMM by `module load aocl`.
Environment variables `MKLROOT` is defined with the `oneapi` module & `AOCL_ROOT` is defined with the `aocl` module.
Note: Please pay attention to the linker options for AOCL & oneMath libraries.
```bash
$ clang++ -std=c++17 -sycl-std=2020 -O3 -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend --cuda-gpu-arch=sm_80 -L$AOCL_ROOT/lib -lblis -L$MKLROOT/lib -lonemath sycl_onemath_gemm.cpp -o sycl_onemath_gemm.out
```

## Further documentation

There is additional documentation on how to get good performance available on the [Codeplay Developer website](https://developer.codeplay.com/products/oneapi/nvidia/latest/guides/performance/common-optimizations).
