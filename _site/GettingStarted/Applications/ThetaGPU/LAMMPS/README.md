
# -- Makefile to build gpu library; copy to lammps-<version>/lib/gpu.
Makefile.gpu_thetagpu  

# -- Makefiles to build lammps executables; copy to lammps-<version>/src/MAKE/MACHINES.
Makefile.thetagpu  
Makefile.thetagpu_kokkos  

# -- Example submission scripts for single-gpu and full-node queues
submit_full-node.sh  
submit_single-gpu.sh


# -- To build LAMMPS w/ GPU package
cd lammps-<version>/lib/gpu
make -f Makefile.gpu_thetagpu -j 8

cd ../../src
make yes-GPU
make thetagpu -j 8


# -- To build LAMMPS w/ KOKKOS package
cd lammps-<version>/src
make yes-KOKKOS
make thetagpu_kokkos -j 8

