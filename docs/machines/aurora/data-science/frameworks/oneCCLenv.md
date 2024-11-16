**OneCCL mandatory environment variables**

The parameters below are recommended to be set all the time as it eigher gives the best performance for all applications or are requires to address potential hang / crash at large scale.

```bash
export CCL_PROCESS_LAUNCHER=pmix  
export CCL_ATL_TRANSPORT=mpi
export CCL_ALLREDUCE=topo
export CCL_ALLREDUCE_SCALEOUT=rabenseifner  # currently best allreduce algorithm at large scale
export CCL_BCAST=double_tree # currently best bcast algorithm at large scale

export CCL_KVS_MODE=mpi
export CCL_CONFIGURATION_PATH=""
export CCL_CONFIGURATION=cpu_gpu_dpcpp
export CCL_KVS_CONNECTION_TIMEOUT=600 

export CCL_ZE_CACHE_OPEN_IPC_HANDLES_THRESHOLD=1024
export CCL_KVS_USE_MPI_RANKS=1

export MPI_PROVIDER=$FI_PROVIDER
unset MPIR_CVAR_CH4_POSIX_COLL_SELECTION_TUNING_JSON_FILE
unset MPIR_CVAR_CH4_COLL_SELECTION_TUNING_JSON_FILE
unset MPIR_CVAR_COLL_SELECTION_TUNING_JSON_FILE
```

**OneCCL optional environment variables**
The impact of the following environment variable might be application dependent.
Users are encourage to try to set them and see whether they help their applications.

```bash
ulimit -c unlimited
export FI_MR_ZE_CACHE_MONITOR_ENABLED=0
export FI_MR_CACHE_MONITOR=disabled
export FI_CXI_RX_MATCH_MODE=hybrid
export FI_CXI_OFLOW_BUF_SIZE=8388608
export FI_CXI_DEFAULT_CQ_SIZE=1048576
export FI_CXI_CQ_FILL_PERCENT=30
export INTELGT_AUTO_ATTACH_DISABLE=1
export PALS_PING_PERIOD=240
export PALS_RPC_TIMEOUT=240
export MPIR_CVAR_GATHERV_INTER_SSEND_MIN_PROCS=-1 #to solve the sync send issue in Horovod seg fault
export CCL_ATL_SYNC_COLL=1 #to avoid potential hang at large scale
export CCL_OP_SYNC=1 #to avoid potential hang at large scale
```

