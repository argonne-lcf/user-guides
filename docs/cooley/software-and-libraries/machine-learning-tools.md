# Machine Learning Tools on Cooley
Cooley is designed as a visualization cluster, because of the support for Nvidia GPUs. However,  it is also possible to run machine learning workflows on Cooley. 

Horovod will not have any significant impact on performance on single GPUs, however since the K80 nodes have 2 GPUs per node it is recommended you use horovod with data parallel learning to take advantage of both GPUs.

## Running a machine learning workflow on Cooley with containers
For more information about building containers for Cooley, please see the Singularity on Cooley page.  This section will focus on using containers for machine learning and deep learning workflows.

Because singularity is setting up a containerized system, there are several important steps to take note of:

  1. Use the `--nv` option to singularity exec to enable Nvidia GPU drivers within the container.  Without this, you will not be able to take advantage of Nvidia gpu acceleration.

  2. Make sure you bind necessary directories correctly.  By default, not all areas mounted on the host system (outside the container) are available inside the container.  To access an area, you can bind it with the -B outside_loc:inside_loc syntax.  For example, to access the theta projects area from inside a container on Cooley, use `-B /lus:/lus` as part of your singularity command.

  3. Run the container inside of mpirun calls.  For example, do `mpirun -n 2 singularity exec --nv -B /lus:/lus $IMAGE /path/to/python/script.py` and NOT ` singularity exec $IMAGE mpirun -n 2 /path/to/python/script.py` (where $IMAGE is the path to the container you want to run)

Running the mpi containers with both GPUs per node has been demonstrated to scale to many nodes on Cooley, so distributed learning is feasible on Cooley.

## Extending Available Software in Containers
If you start with an existing Singularity container, it is possible to add additional software.  The most straightforward path is to install it via pip while in the container, using the `--user` flag if you can.  In this way, you can add extensions to tensorflow/pytorch, or IO frameworks, etc. Email [support@alcf.anl.gov](https://mailto:support@alcf.anl.gov) for questions concerning these techniques.

## Non-container software solutions
It is perfectly possible to run tensorflow, pytorch, etc outside of a container on Cooley.  We don't support official builds or distributions of this, but because Nvidia GPUs are very common for ML and DL software, there are many excellent tools available for getting GPU optimized tensorflow, pytorch, etc.  Solutions that can work on Cooley are pip, conda, and virtualenv, and possibly others.  Note that you will need to add Cuda libraries from softenv to use these tools.

