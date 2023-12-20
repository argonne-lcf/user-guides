# Known Issues

## Running Applications

1. `Cassini Event Queue overflow detected.` errors may occur for certain MPI communications. As a workaround, the following environment variables can be set.

```bash
export FI_CXI_DEFAULT_CQ_SIZE=131072
export FI_CXI_CQ_FILL_PERCENT=20
```

The value of `FI_CXI_DEFAULT_CQ_SIZE` can be set to something smaller if issues persist.


