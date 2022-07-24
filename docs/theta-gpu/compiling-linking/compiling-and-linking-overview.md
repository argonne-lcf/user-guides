# Compiling and Linking on ThetaGPU
## Overview
ThetaGPU has AMD processors on the service nodes (thetagpusn1,2) and AMD processors and NVIDIA A100 GPUs on the compute nodes [see overview page]. The service nodes can be used to create containers and launch jobs.

Until the cross-compiling environment is set up or dedicated build nodes get added, the compute nodes will have to be used for compiling. Do not compile codes on service nodes (thetagpusn1,2).

The default programming environment on the ThetaGPU compute nodes is the GNU compiler tools coupled with NVIDIA’s CUDA toolkit. 

Note: Symlinks to the project directories are not available on the compute nodes. Use the full path (eg: /lus/theta-fs0/projects/<projectname>) to access the project directory.

