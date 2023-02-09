# SYCL 

>SYCL (pronounced ‘sickle’) is a royalty-free, cross-platform abstraction layer that enables code for heterogeneous processors to be written using standard ISO C++ with the host and kernel code for an application contained in the same source file. 

- Specification: [https://www.khronos.org/sycl/](https://www.khronos.org/sycl/)
- Source code of the compiler: [https://github.com/intel/llvm](https://github.com/intel/llvm)
- ALCF Tutorial: [https://github.com/argonne-lcf/sycltrain](https://github.com/argonne-lcf/sycltrain)

```
module use /soft/compilers
module load llvm-sycl/2022-06 
```

## Example (memory intilization)

```
$ cat main.cpp
int main(){
  const int N= 100;
  sycl::queue Q;
  int *A = sycl::malloc_shared<int>(N, Q);

  std::cout << "Running on "
            << Q.get_device().get_info<sycl::info::device::name>()
            << "\n";

  // Create a command_group to issue command to the group
  Q.parallel_for(N, [=](sycl::item<1> id) { A[id] = id; }).wait();

  for (size_t i = 0; i < global_range; i++)
    std::cout << "A[ " << i << " ] = " << A[i] << std::endl;
  return 0;
}
module use /soft/compilers
module load llvm-sycl/2022-06 
$ clang++ -std=c++17 -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend '--cuda-gpu-arch=sm_80' main.cpp
$ ./a.out
```

## Example (using GPU-aware MPI)

```
$ cat main.cpp
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#include <CL/sycl.hpp>

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

    sycl::queue q{sycl::default_selector()};

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

```
module use /soft/compilers
module load mpiwrappers/cray-mpich-llvm
module load llvm-sycl
module load nvhpc/21.9
export MPICH_GPU_SUPPORT_ENABLED=1
```

Compile and Run

```
LIBS=/opt/cray/pe/mpich/8.1.16/gtl/lib

$ mpicxx -L$(LIBS) -lmpi_gtl_cuda -std=c++17 -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend '--cuda-gpu-arch=sm_80' main.cpp
$ mpiexec -n 2 --ppn 2 --depth=1 --cpu-bind depth ./set_affinity_gpu_polaris.sh ./a.out
```
For further details regarding the arguments passed to `mpiexec` command shown above, please visit the [Job Scheduling and Execution section](../../running-jobs/job-and-queue-scheduling.md). A simple example describing the details and execution of the `set_affinity_gpu_polaris.sh` file can be found [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Polaris/affinity_gpu).

**Note:** By default, there is no GPU-aware MPI library linking support.  The example above shows how the user can enable the linking by specifying the path to the GTL (GPU Transport Layer) library (`libmpi_gtl_cuda`) to the link line.   
