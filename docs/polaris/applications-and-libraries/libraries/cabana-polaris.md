# Cabana

## Cabana

Cabana is built atop Kokkos. It provides class templates useful for
implementing particle codes

### Cabana Documentation

* [Cabana Wiki](https://github.com/ECP-copa/Cabana/wiki)
* [Cabana github](https://github.com/ECP-copa/Cabana)

### Cabana on Polaris

Built against the [prebuilt Kokkos on
polaris](../../programming-models/kokkos-polaris.md), the prebuilt Cabana
includes 3 backends: Serial and OpenMP for CPU execution and CUDA for GPU
execution. To use it, run

```
module use /soft/modulefiles
module swap PrgEnv-nvhpc PrgEnv-gnu
module swap gcc/12.2.0 gcc/11.2.0
module load cudatoolkit-standalone/11.8.0
module load kokkos
module load cabana
```

(Since the SlingShot 11 upgrade, you must use `PrgEnv-gnu` and the `gcc` and
`cudatoolkit` version changes indicated, at least until some subsequent Polaris
sytem updates have been completed.)

Currently, Cabana is a headers-only installation; there are no libraries per se.
