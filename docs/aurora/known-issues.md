# Known Issues

## Running Applications

1. `Cassini Event Queue overflow detected.` errors may occur for certain MPI communications and may happen for a variety of reasons - software and hardware, job placement, job routing, and the sate of the machine. Simply speaking, it means one of the network interfaces is getting messages too fast and cannot keep up to process them

```
libfabric:16642:1701636928::cxi:core:cxip_cq_eq_progress():531<warn> x4204c1s3b0n0: Cassini Event Queue overflow detected.
```

As a workaround, the following environment variables can be set to try alleviating the problem.

```bash
export FI_CXI_DEFAULT_CQ_SIZE=131072
export FI_CXI_OVFLOW_BUF_SIZE=8388608
export FI_CXI_CQ_FILL_PERCENT=20
```

The value of `FI_CXI_DEFAULT_CQ_SIZE` can be set to something smaller if issues persist.