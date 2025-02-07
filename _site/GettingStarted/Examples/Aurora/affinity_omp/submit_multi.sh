#!/bin/sh
#PBS -l select=1
#PBS -l place=scatter
#PBS -l walltime=0:15:00
#PBS -q workq
#PBS -A Catalyst
#PBS -l filesystems=home

#cd ${PBS_O_WORKDIR}

export OMP_NUM_THREADS=1
export ZE_ENABLE_PCI_ID_DEVICE_ORDER=1

export ZE_AFFINITY_MASK=0.0
mpiexec --np 4 --ppn 4 --cpu-bind list:0:1:2:3 ./hello_affinity 

export ZE_AFFINITY_MASK=0.1
mpiexec -n 4 --ppn 4 --cpu-bind list:4:5:6:7 ./hello_affinity 

export ZE_AFFINITY_MASK=1.0
mpiexec -n 4 --ppn 4 --cpu-bind list:8:9:10:11 ./hello_affinity 


export ZE_AFFINITY_MASK=5.1
mpiexec -n 4 --ppn 4 --cpu-bind list:40:41:42:43 ./hello_affinity 

wait

