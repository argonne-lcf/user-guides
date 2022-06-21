# SYCL 

>SYCL (pronounced ‘sickle’) is a royalty-free, cross-platform abstraction layer that enables code for heterogeneous processors to be written using standard ISO C++ with the host and kernel code for an application contained in the same source file. 

- Specification: https://www.khronos.org/sycl/
- Source code of the compiler: https://github.com/intel/llvm
- ALCF Tutorial: https://github.com/argonne-lcf/sycltrain

```
module use TODO
module avail intel-llvm
```

# Example (memory intilization)

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
$ module use TODO
$ module load intel-llvm/sycl-nightly/latest
$ clang++ -std=c++17 -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend '--cuda-gpu-arch=sm_80' main.cpp
$ ./a.out
```
