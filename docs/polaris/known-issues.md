# Known Issues

This is a collection of known issues that have been encountered during Polaris' early user phase. Documentation will be updated as issues are resolved.

1. Issues are encountered with GPU-enabled MPI applications communicating between nodes. Users are likely to encounter MPICH Errors and MPIDI_OFI_send_normal:Bad address type of errors in their applications. It is recommend to disable GPU-MPI for the time being if possible.

2. The `nsys` profiler packaged with `nvhpc/21.9` in some cases appears to be presenting broken timelines with start times not lined up. The issue does not appear to be present when `nsys` from `cudatoolkit-standalone/11.2.2` is used. We expect this to no longer be an issue once `nvhpc/22.5` is made available as the default version.