# Submit interactive job to single node

```
qsub -I -l select=8,walltime=1:00:00,filesystems=home:eagle -A Catalyst -q workq
```

## Compilation

```
[knight@sophia-gpu-02 affinity_gpu]$ make -f Makefile.mpi_stub 
g++ -g -O3 -fopenmp -std=c++0x -I/usr/local/cuda//include -I../mpi_stub -c main.cpp
nvcc -x cu  -c offload.cpp -o offload.o
g++ -o hello_affinity -g -O3 -fopenmp -std=c++0x -I/usr/local/cuda//include -I../mpi_stub main.o offload.o -L/usr/local/cuda//lib64 -lcuda -lcudart

```

## Run single node example w/ 8 GPUs

```
[knight@sophia-gpu-02 affinity_gpu]$ cat ./submit_1node.sh 
#!/bin/bash -l
#PBS -l select=8:system=sophia
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q workq 
#PBS -A Catalyst
#PBS -l filesystems=home:grand:eagle

# Example using all 8 GPUs and 128 cores (1 thread per core)
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=1
NDEPTH=128
NTHREADS=128

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

export OMP_NUM_THREADS=${NTHREADS}
export OMP_PLACES=cores
./hello_affinity

#echo "Affinitying using cpu-bind depth"
#mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth ./hello_affinity
```

The following is output using all 8 GPUs and both 64-core CPUS (128 cores total w/ 1 thread per core).
```
[knight@sophia-gpu-02 affinity_gpu]$ ./submit_1node.sh 
NUM_OF_NODES= 1 TOTAL_NUM_RANKS= 1 RANKS_PER_NODE= 1 THREADS_PER_RANK= 128
rnk= 0 :  # of devices detected= 8
    [0,0] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]  uuid= GPU-aac16f31-b1eb-f3be-04fa-f433422e8325

    [0,1] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]  uuid= GPU-026176ed-8be0-dc3a-d6eb-aca6d8bdbee0

    [0,2] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]  uuid= GPU-9c637407-d357-3b4e-0aa8-6b36a3bf2dd2

    [0,3] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]  uuid= GPU-4f51714d-8ff5-5e70-b88a-371d11af67b9

    [0,4] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]  uuid= GPU-649c74ac-89ce-327c-b7e3-df5c7b156ca7

    [0,5] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]  uuid= GPU-d44f2765-b8fb-bea0-97c7-12d4bf95e5ab

    [0,6] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]  uuid= GPU-aeccc854-3b1e-6298-c312-8727ace27fa3

    [0,7] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]  uuid= GPU-d3e11f50-b404-66f5-ede7-e6310fc67902

    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 0: list_cores= (0,128)
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 1: list_cores= (1,129)
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 2: list_cores= (2,130)
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 3: list_cores= (3,131)
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 4: list_cores= (4,132)
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 5: list_cores= (5,133)
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 6: list_cores= (6,134)
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 7: list_cores= (7,135)
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 8: list_cores= (8,136)
    ...
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 120: list_cores= (120,248)
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 121: list_cores= (121,249)
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 122: list_cores= (122,250)
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 123: list_cores= (123,251)
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 124: list_cores= (124,252)
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 125: list_cores= (125,253)
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 126: list_cores= (126,254)
    -- To affinity and beyond!! nname= sophia-gpu-02  rnk= 0  tid= 127: list_cores= (127,255)
```
