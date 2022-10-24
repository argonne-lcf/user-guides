# Known Issues

This is a collection of known issues that have been encountered during Polaris' early user phase. Documentation will be updated as issues are resolved.

1. The `nsys` profiler packaged with `nvhpc/21.9` in some cases appears to be presenting broken timelines with start times not lined up. The issue does not appear to be present when `nsys` from `cudatoolkit-standalone/11.2.2` is used. We expect this to no longer be an issue once `nvhpc/22.5` is made available as the default version.

2. With PrgEnv-nvhpc/8.3.3, if you are using nvcc to indirectly invoke nvc++ and compiling C++17 code (as, for example, in building Kokkos via nvcc_wrapper), you will get compilation errors with C++17 constructs. See [our documentation on NVIDIA Compilers](./compiling-and-linking/nvidia-compiler-polaris.md#known-issues-and-workarounds) for a workaround.
