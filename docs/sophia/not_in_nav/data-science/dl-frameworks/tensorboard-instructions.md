# TensorBoard Instructions

If you are able to install TensorBoard on your local machine, it is often easiest to copy the requisite files from ALCF file systems (via `sftp`, `scp`, Globus, etc.) to your local machine and run TensorBoard there.

However, if that is not possible, or if you have many and/or large files that TensorBoard needs to process located on ALCF file systems, there are several ways to run a TensorBoard server remotely.

## TensorBoard server on a ThetaGPU compute node

This approach can be useful to have TensorBoard analyze live training progress. After you have logged into ThetaGPU and have an interactive job running, you'll need to know the name of one of your worker nodes so you can SSH to it.

```bash
PORT0=9991 
PORT1=9992 
PORT3=9993 
# Select a theta login node N where N=[1-6]
ssh -L $PORT0:localhost:$PORT1 $USER@thetaloginN.alcf.anl.gov 

# after reaching thetaloginN 

# Replace NN with your thetagpu worker node
ssh -L $PORT1:thetagpuNN:$PORT3 $USER@thetagpusn1 
# after reaching thetagpusn1 

# login to worker node 
ssh thetagpuNN 

# now setup your tensorflow environment 
# for instance run the conda setup.sh script created during the install_tensorflow.sh script 

# now run tensorboard 
tensorboard --logdir </path/to/logs> --port $PORT3 --bind_all
```

## TensorBoard server on a ThetaKNL login node

If you do not require the use of a GPU during analysis while TensorBoard runs, and you do not require a cutting-edge version of TensorBoard (this will load version 2.6.0), you can avoid additional SSH tunnel hops by running the TensorBoard server on a ThetaKNL login node:

```bash
ssh -D <some-port-number> theta.alcf.anl.gov

module load conda/2021-09-22
export LD_LIBRARY_PATH=/soft/thetagpu/cuda/cuda_11.3.0_465.19.01_linux/lib64:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/soft/thetagpu/cuda/cuda_11.3.0_465.19.01_linux/extras/CUPTI/lib64/:$LD_LIBRARY_PATH
```