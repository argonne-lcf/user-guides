---
search:
  exclude: true
---

# Quantum Package
## What Is Quantum Package?
Quantum Package (QP) is a Fortran/MPI scalable parallel implementation of selected Configuration Interaction (sCI), approaching the full CI answer when fully converged,. Quantum Package is currently interfaced to Pyscf and QMCPACK and allows the generation of high accuracy trial wavefunctions for small to medium size molecules and solids.  A more complete description of the methods implemented in the code can be found on the QP website [https://quantumpackage.github.io/qp](https://quantumpackage.github.io/qp2/) and in the publication [https://doi.org/10.26434/chemrxiv.7749485.v2](https://doi.org/10.26434/chemrxiv.7749485.v2). While the code is highly scalable, the method underneath sCI scales O(N!) with number of orbitals N. Therefore it’s usage applies to systems smaller than 800 orbitals.

## Using Quantum Package at ALCF

ALCF does not officially support QP software, but provides a binary located and maintained in /soft/applications/quantum_package and guidance to compile your own. For questions, contact us at [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## How to Obtain the Code
Quantum Package is an open source code downloadable at [https://github.com/QuantumPackage/qp2](https://github.com/QuantumPackage/qp2). For more information refer to the code manual [https://quantumpackage.github.io/qp2](https://quantumpackage.github.io/qp2/).

## Building on Theta
While it is recommended to use the version of the code available on /soft/applications/quantum_package , you can build your own version on Theta as follow.

```
git clone https://github.com/QuantumPackage/qp2 cd qp2 sed s/”mpiifort”/”ftn”/g > config/ifort_avx_mpi.cfg ./configure -c  config/ifort_avx_mpi.cfg source quantum_package.rc ./configure –i all source quantum_package.rc ./configure -c config/ifort_avx_mpi.cfg ninja
```

Executables are installed and updated on Theta as often as the new updates are released.

## Running Jobs on Theta

The following is an example script “run_fci.sh” to run an fci job on Theta on 128 nodes with 1MPI task per node and 128 threads.  The job can be submitted with command “qsub run_fci.sh.”

```
cat run_fci.sh #!/bin/bash #COBALT -q default #COBALT -A PRJECT_NAME    #COBALT -n 128 #COBALT -t 180 #COBALT -O MyOutputName #COBALT --attrs mcdram=cache:numa=quad exportATP_ENABLED=1 file_prefix=File  exe=/soft/applications/quantum_package source ${exe}/quantum_package.rc  NCORES=64 HT=2 NTHREADS=$((NCORES * HT)) let SLAVE_NODE=${COBALT_PARTSIZE}-1 aprun -n1-N1-ccdepth -d$NTHREADS-j$HTqp_run fci ${file_prefix}.ezfio>>${file_prefix}-fci.out & sleep 360 aprun -n${SLAVE_NODE}-N1-ccdepth -d$NTHREADS-j$HTqp_run –s fci ${file_prefix}.ezfio>>${file_prefix}-fci-slave.out & wait qsub run_fci.sh
```

