# Cabana

## Cabana

Cabana is built atop Kokkos. It provides class templates useful for
implementing particle codes

### Cabana Documentation

* [Cabana Wiki](https://github.com/ECP-copa/Cabana/wiki)
* [Cabana github](https://github.com/ECP-copa/Cabana)

### Cabana on Polaris

Built against the prebuilt Kokkos on polaris, the prebuilt Cabana includes 3
backends: Serial and OpenMP for CPU execution and CUDA for GPU execution. To
use it, run

```
module use /soft/programming-models
module load cabana
```

Currently, Cabana is a headers-only installation; there are no libraries per se.
