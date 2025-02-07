module restore
module load craype-accel-nvidia80
module swap PrgEnv-nvhpc PrgEnv-gnu
module use /soft/modulefiles
module load cudatoolkit-standalone

module load spack-pe-base cmake
