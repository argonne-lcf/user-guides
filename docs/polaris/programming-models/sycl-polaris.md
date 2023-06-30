# SYCL

>SYCL (pronounced ‘sickle’) is a royalty-free, cross-platform abstraction layer that enables code for heterogeneous processors to be written using standard ISO C++ with the host and kernel code for an application contained in the same source file.

- Specification: [https://www.khronos.org/sycl/](https://www.khronos.org/sycl/)
- Source code of the compiler: [https://github.com/intel/llvm](https://github.com/intel/llvm)
- ALCF Tutorial: [https://github.com/argonne-lcf/sycltrain](https://github.com/argonne-lcf/sycltrain)

```
module load oneapi
```

:warning: This module (compilers, libraries) gets built periodically from the latest open-source rather than releases. As such, these compilers will get new features and updates quickly that may break on occasion.

## Dependencies
- SYCL programming model is supported through `oneapi` compilers that were built from source-code
- Loading this module switches the default programming environment to GNU and with the following dependencies
  - PrgEnv-gnu
  - cudatoolkit-standalone
- Environment Variable set: `SYCL_DEVICE_SELECTOR=ext_oneapi_cuda:gpu`

## Example (memory intilization)

```c++
#include <sycl/sycl.hpp>

int main(){
    const int N= 100;
    sycl::queue Q;
    float *A = sycl::malloc_shared<float>(N, Q);

    std::cout << "Running on "
              << Q.get_device().get_info<sycl::info::device::name>()
              << "\n";

    // Create a command_group to issue command to the group
    Q.parallel_for(N, [=](sycl::item<1> id) { A[id] = 0.1 * id; }).wait();

    for (size_t i = 0; i < N; i++)
        std::cout << "A[ " << i << " ] = " << A[i] << std::endl;
    return 0;
}
```

Compile and Run
```bash
$ clang++ -std=c++17 -sycl-std=2020 -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend --cuda-gpu-arch=sm_80 main.cpp
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
module load oneapi
module load mpiwrappers/cray-mpich-oneapi
export MPICH_GPU_SUPPORT_ENABLED=1
```

Compile and Run

```bash
$ mpicxx -L/opt/cray/pe/mpich/8.1.16/gtl/lib -lmpi_gtl_cuda -std=c++17 -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend --cuda-gpu-arch=sm_80 main.cpp
$ mpiexec -n 2 --ppn 2 --depth=1 --cpu-bind depth ./set_affinity_gpu_polaris.sh ./a.out
```
For further details regarding the arguments passed to `mpiexec` command shown above, please visit the [Job Scheduling and Execution section](../../running-jobs/job-and-queue-scheduling.md). A simple example describing the details and execution of the `set_affinity_gpu_polaris.sh` file can be found [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Polaris/affinity_gpu).

**Note:** By default, there is no GPU-aware MPI library linking support.  The example above shows how the user can enable the linking by specifying the path to the GTL (GPU Transport Layer) library (`libmpi_gtl_cuda`) to the link line.

# oneAPI Math Kernel Library (oneMKL) Interfaces
[oneMKL Interfaces](https://github.com/oneapi-src/oneMKL) is an open-source implementation of the oneMKL Data Parallel C++ (DPC++) interface according to the [oneMKL specification](https://spec.oneapi.io/versions/latest/elements/oneMKL/source/index.html). It works with multiple devices (backends) using device-specific libraries underneath.

oneMKL is part of oneAPI. Various backend supported are shown below. More Information [here](https://github.com/oneapi-src/oneMKL#supported-configurations).
| User Application | Third-Party Library                                          |
|------------------|--------------------------------------------------------------|
|                  | [cuBLAS](https://docs.nvidia.com/cuda/cublas/index.html)     |
| oneMKL interface | [cuSOLVER](https://docs.nvidia.com/cuda/cusolver/index.html) |
|                  | [cuRAND](https://docs.nvidia.com/cuda/curand/index.html)     |


## Example (using onemkl::gemm)
The following snippet shows how to compile and run a SYCL code with oneMKL library. For instance, a GPU-based GEMM is performed using `mkl::gemm` API and the results are compared to a CPU-based GEMM performed using the traditional blas (e.g., AOCL-BLIS) library.
```c++
#include <limits>
#include <random>

#include <sycl/sycl.hpp>

#include <oneapi/mkl.hpp>  // ONEMKL GPU header
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
  oneapi::mkl::transpose transA = oneapi::mkl::transpose::nontrans;
  oneapi::mkl::transpose transB = oneapi::mkl::transpose::nontrans;

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
  double *C_host_onemkl, *C_cblas;

  A = new double[M * N]{};
  B = new double[N * P]{};
  C_cblas = new double[M * P]{};
  C_host_onemkl = new double[M * P]{};

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

  // Resultant matrix: C_onemkl
  sycl::queue q(sycl::property_list{sycl::property::queue::in_order{}});
  std::cout << "Device: " << q.get_device().get_info<info::device::name>() << std::endl << std::endl;

  double* A_dev        = sycl::malloc_device<double>(M*N, q);
  double* B_dev        = sycl::malloc_device<double>(N*P, q);
  double* C_dev_onemkl = sycl::malloc_device<double>(M*P, q);

  q.memcpy(A_dev, A, (M*N) * sizeof(double));
  q.memcpy(B_dev, B, (N*P) * sizeof(double));

  auto gemm_event = oneapi::mkl::blas::column_major::gemm(q, transB, transA, n, m, k, alpha, B_dev, ldB, A_dev, ldA, beta, C_dev_onemkl, ldC);

  q.memcpy(C_host_onemkl, C_dev_onemkl, (M*P) * sizeof(double));

  q.wait();
  std::cout << "Verify results between OneMKL & CBLAS: ";
  int result_cblas = VerifyResult(C_cblas, C_host_onemkl);

  delete[] A;
  delete[] B;
  delete[] C_cblas;
  delete[] C_host_onemkl;
  sycl::free(A_dev, q);
  sycl::free(B_dev, q);
  sycl::free(C_dev_onemkl, q);
  return result_cblas;
}
```

Compile and Run
The user would need to provide paths the math-libraris as shown below. Also please provide AOCL library for CPU GEMM by `module load aocl`.
Environment variables `MKLROOT` is defined with `oneapi` module & `AOCL_ROOT` is defined with `aocl` module.
Note: Please pay attention to the linker options for AOCL & oneMKL libraries.
```bash
$ clang++ -std=c++17 -sycl-std=2020 -O3 -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend --cuda-gpu-arch=sm_80 -L$AOCL_ROOT/lib -lblis -L$MKLROOT/lib -lonemkl sycl_onemkl_gemm.cpp -o sycl_onemkl_gemm.out
```
