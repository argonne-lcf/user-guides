# Kokkos

## Kokkos

Kokkos Core implements a programming model in C++ for writing performance
portable applications targeting all major HPC platforms. For that purpose it
provides abstractions for both parallel execution of code and data
management. Kokkos is designed to target complex node architectures with
N-level memory hierarchies and multiple types of execution resources. It
currently can use Serial and OpenMP (threads) for CPU execution spaces
("backends") and CUDA, HIP, SYCL, and OpenMPTarget for GPU execution
spaces. By convention, Kokkos only allows one GPU backend at a time.

### Kokkos Documentation

* [Kokkos-core Wiki](https://kokkos.github.io/kokkos-core-wiki/)
* [Kokkos github](https://github.com/kokkos/kokkos)

### Kokkos on Polaris

The prebuilt Kokkos on polaris includes 3 backends: Serial and OpenMP for CPU
execution and CUDA for GPU execution. To use it, run

```
module use /soft/programming-models
module load kokkos
```



