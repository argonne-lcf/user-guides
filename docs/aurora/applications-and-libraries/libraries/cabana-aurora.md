# Cabana

## Cabana

Cabana is built atop Kokkos. It provides class templates useful for
implementing particle codes

### Cabana Documentation

* [Cabana Wiki](https://github.com/ECP-copa/Cabana/wiki)
* [Cabana github](https://github.com/ECP-copa/Cabana)

### Cabana on Aurora

Built against the prebuilt Kokkos on Aurora, the prebuilt Cabana includes 3
backends: Serial and OpenMP for CPU execution and SYCL for GPU execution. To
use it, run

```
module load cabana
```

Currently, Cabana is a headers-only installation; there are no libraries per se.
