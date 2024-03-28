#!/bin/bash -x

# Double precision Peak
mpicxx -fiopenmp -fopenmp-targets=spir64  flops.cpp -o flops
mpirun -n 1 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./flops
mpirun -n 2 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./flops
mpirun -n 12 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./flops

# Memory Bandwidth (triad)
mpicxx -fiopenmp -fopenmp-targets=spir64  triad.cpp -o triad
mpirun -n 1 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./triad
mpirun -n 2 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./triad
mpirun -n 12 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./triad

# PCIe Bandwidth
mpicxx -fsycl pci.cpp -o pci
mpirun -n 1 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./pci
mpirun -n 2 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./pci
mpirun -n 12 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./pci

mpicxx -fsycl peer2pear.cpp -o peer2pear

# Tile 2 Tile 
mpirun -n 2 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./peer2pear
mpirun -n 12 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./peer2pear

# Gpu 2 Gpu (same plan)
icpx -fsycl -lze_loader topology.cpp -o topology
mpirun -n 2 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_plan_compact.sh ./peer2pear
mpirun -n 12 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_plan_compact.sh ./peer2pear

# GEMM
mpicxx -fsycl gemm.cpp -qmkl -o gemm
mpirun -n 1 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./gemm
mpirun -n 2 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./gemm
mpirun -n 12 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./gemm

# FFT
mpicxx -fsycl fftc2c.cpp -qmkl -o fftc2c
mpirun -n 1 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./fftc2c
mpirun -n 2 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./gemm
mpirun -n 12 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- ./gpu_tile_compact.sh ./fftc2c
