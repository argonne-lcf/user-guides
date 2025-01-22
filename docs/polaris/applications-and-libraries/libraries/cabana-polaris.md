# Cabana

## Cabana

Cabana is built atop Kokkos. It provides class templates useful for implementing particle codes.

### Cabana Documentation

* [Cabana Wiki](https://github.com/ECP-copa/Cabana/wiki)
* [Cabana GitHub](https://github.com/ECP-copa/Cabana)

### Cabana on Polaris

Following the [Polaris upgrade to HPCM 1.10](https://www.alcf.anl.gov/support-center/facility-updates/polaris-upgraded-hpcm-110-0), the module setup to use the prebuilt Kokkos has changed.

Built against the [prebuilt Kokkos on Polaris](../../programming-models/kokkos-polaris.md), the prebuilt Cabana includes three backends: Serial and OpenMP for CPU execution, and CUDA for GPU execution. To use it, run:

```
module load craype-x86-milan
module load craype-accel-nvidia80
module swap PrgEnv-nvhpc PrgEnv-gnu
module use /soft/modulefiles
module load cuda-PrgEnv-nvidia/12.2.91
module load kokkos cabana
```

Cabana is a headers-only package; there are no actual libraries installed.