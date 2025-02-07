# GettingStarted

Contained herein is a collection of material (source code, scripts, examples, etc...) to be used as examples for setting up user environments, running jobs, debugging and tuning applications, and related tasks for successful use of computational resources at the Argonne Leadership Computing Facility (ALCF).

Material here is typically developed for and used during ALCF training workshops, such as the ALCF Computational Performance Workshops typically held in the Spring and the ALCF Simulation, Data, and Learning Workshops in the Fall.

If not sure where to start, then a good place may be perusing ProgrammingModels for the resource of interest to gain experience compiling simple codes and submitting jobs.

## Contents

<ol>
<li> Applications: Example Makefiles, build scripts, etc... for some applications
<li> DataScience: Data & Learning examples with pointers to additional Tutorials
<li> Debug: Simple examples using available debugging tools
<li> Examples: Collection of examples demonstrating common tasks (e.g. compilation & setting affinity)
<li> Performance: Simple examples using availble performance tools
<li> ProgrammingModels: Simple examples using supported programming models
</ol>

# Resources

## Cooley

Cooley is a data analysis and visualization cluster consisting of 126 compute nodes, each with 12 Intel Haswell cores and an NVIDIA Tesla K80 dual-GPU card. Additional hardware information is available [here][1].

## Theta

Theta is a Cray XC40, 11.7 petaflops system based on the second-generation Intel Xeon Phi processor codenamed Knights Landing (KNL). There are 4,392 compute nodes available to users each with 64 cores, 192 GiB DDR4 & 16 GiB MCDRAM memory, and a 128 GiB SSD. Additional hardware information is available [here][2].

## ThetaGPU

ThetaGPU is an extension of Theta and is comprised of 24 NVIDIA DGX A100 nodes, each with eight NVIDIA A100 Tensor Core GPUs and two AMD Rome CPUs. Additional hardware information is available [here][3]. 

## Polaris

Polaris is the newest ALCF system with 560 nodes, each with one AMD Milan CPU (32 cores) and four NVIDIA A100 GPUs. Additional information on using the system is available [here][4].

[1]: https://www.alcf.anl.gov/support-center/cooley/cooley-system-overview

[2]: https://www.alcf.anl.gov/support-center/theta/theta-thetagpu-overview

[3]: https://www.alcf.anl.gov/support-center/theta/theta-thetagpu-overview#theta-gpu

[4]: https://www.alcf.anl.gov/support/user-guides/polaris/hardware-overview/machine-overview/index.html
