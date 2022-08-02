# Getting Started on ThetaGPU
## References
In addition to the content below, here is a [getting started video](https://youtu.be/N0Xip1mUZY8) covering the basics of using ThetaGPU and a  related video on Lustre File Striping Basics. This should help you get up and running quickly on the GPU nodes.
- [Video on Getting Started on ThetaGPU](https://youtu.be/N0Xip1mUZY8) 
- [Lustre File Striping Basics](https://www.alcf.anl.gov/support-center/training-assets/file-systems-and-io-performance)

## Login to ThetaGPU
```
ssh -A username@theta.alcf.anl.gov
```
Replace the username with your ALCF username. You will prompted to type in your MFA password. Note: In order to log in to ALCF systems, you need to have an active [ALCF account](https://www.alcf.anl.gov/support-center/get-started/request-account).
## Setup ThetaGPU environment
Once logged in, you land on theta login nodes (thetalogin1 - thetalogin6). 

You can set an environment variable to control which instance the default commands (qsub, qstat, etc) will interact with. The primary use case here will be users who only use GPU nodes, but are working from the Theta login nodes.  To do so, you may do:
```
module load cobalt/cobalt-gpu
```
To switch back you may do `module load cobalt/cobalt-knl` which would make cobalt commands interact with the original Cobalt instance and launch jobs on the KNL nodes.

Alternatively, If you are on a GPU node, for instance, the service nodes (thetagpusn1-2), then commands will default to the GPU instance. To head to a service node from the theta login nodes use:
```
ssh thetagpusnX
```
you can also set ```COBALT_CONFIG_FILES=<path to cobalt config>``` 
- knl config: /etc/cobalt.knl.conf
- gpu config: /etc/cobalt.gpu.conf

You can use suffixed commands to explicitly control which instance you are interacting with. If you regularly use both types of nodes, this is the recommended path to avoid confusion and to prevent launching jobs on the wrong architecture.

All the commands you are used to are there, they take the same command line parameters, etc., they just have either -knl or a -gpu suffix on them. For instance:
- qsub-knl <parameters> would submit a job to the KNL nodes
- qstat-gpu would check the queue status for the GPU nodes

For all the build and development please use ThetaGPU compute nodes. Please avoid using the service nodes thetagpusn[1,2] as they have not been set up for development. 

Using "qstat -Q" to see all available queues. You can submit your job to a specific queue (as long as you are part of that queue) using "qsub -q queue_name".

- For more information on all ThetaGPU queues visit: [Queue Policy on ThetaGPU](https://www.alcf.anl.gov/support-center/theta-gpu-nodes/queue-policy-thetagpu)
- For more information on submitting a job visit: [Submit a job on ThetaGPU](https://www.alcf.anl.gov/support-center/theta/submit-job-theta)
  
## Project Space and Home: 
Every user has a home directory located at /home/username.

The project folder is located at:
```
/grand/project_name or /eagle/project_name
/lus/grand/projects/project_name or /lus/eagle/projects/project_name
```
Please use the project folder when building and running on ThetaGPUcompute nodes

/grand is on an HDR network directly connected to ThetaGPU

/home is on an FDR network which is up-linked to the HDR network via a straw, heavy use of this file system will result in Bad Things™ again.

For more information on all available file systems visit: [Theta File Systems](https://www.alcf.anl.gov/support-center/theta/theta-file-systems)  

## Software
ThetaGPU is new, so it has limited ALCF provided software. ThetaGPU compute nodes are setup with CUDA11

Default Nvidia installed software will just be in your PATH

```which nvcc```

To see the available software via modules

```module avail```

Other ThetaGPU software can be found in

```/soft/thetagpu```

Theta software can be found in  ```/soft```
  – Anything that is not compute specific will be useable on the AMD host CPUs
  – cmake is good example of something that can be used

For more information on compiling and linking on ThetaGPU visit: [Compiling and Linking on ThetaGPU](https://www.alcf.anl.gov/support-center/theta-gpu-nodes/compiling-and-linking-thetagpu)
  
## NVIDIA HPC SDK
  
```module use /soft/thetagpu/hpc-sdk/modulefiles```
  
– Adds more modules for Nvidia SDK
  
```module avail```
  
– Shows you the new modules you have available
– 20.9 version will be loaded by default
– 21.2 version available using CUDA11 driver
– 21.3 version available using CUDA11 driver
  
```nvhpc```
  
– Loads the SDK and sets various compiler environment variables so that build tools will likely pick up the compilers by default
– MPI wrappers disabled
  
```nvhpc-byo-compiler```
  
– Identical to nvhpc but doesn’t set compiler environment variables
  
```nvhpc-nompi```
  
– Excludes MPI libraries

## Proxy
If the node you are on doesn’t have outbound network connectivity, add the following to your ~/.bash_profile file to access the proxy host
```
# proxy settings
export HTTP_PROXY=http://theta-proxy.tmi.alcf.anl.gov:3128
export HTTPS_PROXY=http://theta-proxy.tmi.alcf.anl.gov:3128
export http_proxy=http://theta-proxy.tmi.alcf.anl.gov:3128
export https_proxy=http://theta-proxy.tmi.alcf.anl.gov:3128
```
## I/O
/grand is a Lustre file system. Default stripe size is 1MiB and stripe count is 1. If you have a large file to read or write with high performance (in parallel)
– Set the stripe count higher than 1
– Use a specific directory for these files
```
mkdir big_files
lfs setstripe -c 8 big_files
/raid/scratch for local disk in RAID-0 config
```
## MPI
ALCF provides a few MPI package built specifically for ThetaGPU
– UCX is enabled

```module load openmpi```

– Default module is openmpi/openmpi-4.1.0

```module av openmpi```

List of possible openmpi modules
  
  

  
