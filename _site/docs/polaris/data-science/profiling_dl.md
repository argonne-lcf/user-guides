# Profiling Deep Learning Applications

We can use both a framework-specific (for example, PyTorch-specific) native profiler and the vendor-specific NVIDIA [Nsys profiler](https://developer.nvidia.com/nsight-systems/get-started) to get high-level profiling information and a timeline of execution for an application. For kernel-level information, we may use [Nsight Compute profiler](https://developer.nvidia.com/tools-overview/nsight-compute/get-started). Refer to the respective documentation for more details:

- [Nsight System User Guide](https://docs.nvidia.com/nsight-systems/UserGuide/index.html)
- [Nsight Compute Documentation](https://docs.nvidia.com/nsight-compute/)
- [Nsight Compute CLI](https://docs.nvidia.com/nsight-compute/NsightComputeCli/index.html)
- [PyTorch Profiler](https://pytorch.org/tutorials/recipes/recipes/profiler_recipe.html)

## Example Usage

Both the `nsys` and `ncu` profiler commands take the following generic structure:

```
nsys profile -o profile python application.py
```

If we want to launch the profiled application with MPI, then `mpiexec` must be used:

```
mpiexec ... nsys profile ... python application.py ... 
```

These two commands show the basic command-line structure of deploying the profilers. Below we discuss important use cases that are relevant in large-scale distributed profiling.

We can use `nsys` to trace an application running on multiple ranks and multiple nodes. A simple example, where we use a wrapper script to trace the rank 0 on each node of a 2-node job running a PyTorch application, is below:

```bash linenums="1" title="nsys_wrapper.sh"
#!/bin/bash
## This wrapper should be used with nsys profiler to trace in any number of nodes
## The script is set up to trace rank 0 of the first 2 Nodes in the case of
## profiling a job running on more than 2 nodes.
FNAME_EXT=$(basename "$2")
FNAME="${FNAME_EXT%%.*}"

NNODES=`wc -l < $PBS_NODEFILE`

WORK_DIR=/path/to/the/Python/application
DTAG=$(date +%F_%H%M%S)
PROFILER_OUTDIR=${WORK_DIR}/profiles/choice_of_name_nsys_n${NNODES}_${DTAG}/${FNAME}_n${NNODES}_${DTAG}
RUN_ID=choice_of_name_nsys_n${NNODES}_${DTAG}

mkdir -p ${PROFILER_OUTDIR}
NSYS_OPTS=" -o ${PROFILER_OUTDIR}/${RUN_ID}_%q{PMI_RANK} --stats=true --show-output=true "

PROFRANK=0
RANKCUTOFF=8

if [[ $PALS_LOCAL_RANKID -eq $PROFRANK ]] && [[ $PMI_RANK -lt $RANKCUTOFF ]]; then
  echo "On rank ${PMI_RANK}, collecting traces "
  nsys profile $NSYS_OPTS "$@"
else
  "$@"
fi
```

There are a few important things to notice in the wrapper.

- `NSYS_OPTS`: These are the options that `nsys` uses to trace data at different levels. An exhaustive list of options can be found in the [nsys user guide](https://docs.nvidia.com/nsight-systems/UserGuide/index.html). Note that `%q{PMI_RANK}` is essential to get a per-rank profile.

- `PROFRANK`: As implemented, this variable is set by the user to trace the rank of choice. For example, this wrapper will trace the rank 0 on each node.

- `RANKCUTOFF`: This variable is Polaris specific. As we can run as many as 4 ranks per node (without using MPS), the first 2 nodes of a job will have 8 ranks running. This provides the upper cutoff of the label (in number) of ranks, beyond which `nsys` will not trace any rank. A user can change the number according to the number of maximum ranks running per node to set up how many ranks to be traced. `nsys` will produce a profile (`nsys-rep` file, by default) per traced rank.

To view the produced trace files, we need to use NVIDIA's Nsight Systems on the local machine.

[Getting Started, Download Nsys](https://developer.nvidia.com/nsight-systems/get-started)

#### Deployment

The wrapper above can be deployed using the following PBS job script:

```bash linenums="1" title="pbs_jobscript_nsys.sh"
#!/bin/bash -l
#PBS -l select=2:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:05:00
#PBS -q debug-scaling
#PBS -l filesystems=home:eagle
#PBS -A YOUR ALLOCATION

# What's the working directory for the benchmark?
WORK_DIR=/path/to/the/Python/program
TEMPORARY_DIR=/path/to/a/temporary/directory/for/`nsys`/to/use
NSYS_WRAPPER=${WORK_DIR}/nsys_wrapper.sh

# MPI and OpenMP settings
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=4

let NRANKS=${NNODES}*${NRANKS_PER_NODE}

module use /soft/modulefiles/
module load conda/2024-04-29
conda activate

mpiexec -n ${NRANKS} -ppn ${NRANKS_PER_NODE} --env TMPDIR=${TEMPORARY_DIR} -l --line-buffer \
${NSYS_WRAPPER} python ${WORK_DIR}/application.py
```

Note that `--env TMPDIR=${TEMPORARY_DIR}` is essential for `nsys` to function correctly.

We can get kernel-level information (for example, roofline, Tensor Core usage) using NVIDIA's Nsight Compute profiler. Below is a simple wrapper script to show the usage.

```bash linenums="1" title="ncu_wrapper.sh"
#!/bin/bash
FNAME_EXT=$(basename "$2")
FNAME="${FNAME_EXT%%.*}"

NNODES=`wc -l < $PBS_NODEFILE`

WORK_DIR=/path/to/the/Python/program
DTAG=$(date +%F_%H%M%S)
PROFILER_OUTDIR=${WORK_DIR}/profiles/choice_of_name_ncu_n${NNODES}_${DTAG}/${FNAME}_n${NNODES}_${DTAG}
RUN_ID=choice_of_name_ncu_n${NNODES}_${DTAG}

mkdir -p ${PROFILER_OUTDIR}
#KERNEL_NAME=ampere_sgemm_128x128_tn
KERNEL_NAME=ampere_bf16_s16816gemm_bf16_128x256_ldg8_f2f_stages_64x3_tn
#NCU_OPTS_DETAILED=" --set detailed -k ${KERNEL_NAME} -o ${PROFILER_OUTDIR}/${RUN_ID}_%q{PMI_RANK} "
NCU_OPTS_ROOFLINE=" --set roofline -k ${KERNEL_NAME} -o ${PROFILER_OUTDIR}/${RUN_ID}_%q{PMI_RANK} "
#NCU_OPTS_FULL=" --set full -k ${KERNEL_NAME} -o ${PROFILER_OUTDIR}/${RUN_ID}_%q{PMI_RANK} "

PROFRANK=0
RANKCUTOFF=8

if [[ $PALS_LOCAL_RANKID -eq $PROFRANK ]] && [[ $PMI_RANK -lt $RANKCUTOFF ]]; then
  echo "On rank ${PMI_RANK}, collecting traces "
  ncu $NCU_OPTS_DETAILED "$@"
else
  "$@"
fi
```

This wrapper can be deployed as the `nsys` example above. In the `ncu` wrapper, we explicitly set the name of the kernel that we want to analyze (a GEMM kernel in this case). The exhaustive list of options to set the amount of data collection can be found in the [command line section](https://docs.nvidia.com/nsight-compute/NsightComputeCli/index.html#command-line-options) of the documentation. Here we only show standard options; either of the three could be chosen. Note that invoking each option will lead to varying amounts of time the profiler needs to run. This will be important in setting the requested walltime for your batch job.

`ncu` will generate `ncu-rep` files for each traced rank, and we will need NVIDIA's Nsight Compute system on the local machine.

[Download Nsight Compute](https://developer.nvidia.com/tools-overview/nsight-compute/get-started)

The next step is to load the `nsys-rep` files in the Nsight Systems GUI, and the `ncu-rep` files to the Nsight Compute GUI.

### Single Rank Run

#### `nsys` profiles

In the single rank case, we go to the top left, go `file` --> `open` and select the file that we want to look at. For this particular example, we have focused on the GPU activities. This activity is shown on the second column from the left, named as `CUDA HW ...`. If we expand the `CUDA HW ...` tab, we find an `NCCL` tab. This tab shows the communication library calls.

#### `ncu` profiles

The primary qualitative distinction between the `nsys-rep` files and the `ncu-rep` files is that the `nsys-rep` file presents data for the overall execution of the application, whereas the `ncu-rep` file presents data for the execution of one particular kernel. Our setup here traces only one kernel, but multiple kernels could be traced at a time, but that can become a time-consuming process.

We use the `--stats=true --show-output=true` (see `nsys_wrapper.sh`) options while collecting the `nsys` data. As a result, we get a system-wide summary in our `.OU` files (if run with a job submission script, otherwise on the terminal), and find the names of the kernels that have been called/used for compute and communication. Often we would start with investigating the kernels that have been called the most times or the ones where we spent the most time executing them. In this particular instance, we chose to analyze the `gemm` kernels, which are related to the matrix multiplication. The full name of this kernel is passed to the `ncu` profiler with the option `-k` (see `ncu_wrapper.sh`).

Loading the `ncu-rep` files works similarly as the `nsys-rep` files. Here, the important tab is the `Details` tab. We find that at the 3rd row from the top. Under that tab, we have the `GPU Speed of Light Throughput` section. In this section, we can find plots showing GPU compute and memory usage. On the right-hand side of the tab, there is a menu bar which gives us the option to select which plot to display, either the roofline plot or the compute-memory throughput chart.

### For a Multi-Rank Run

#### `nsys` profiles

In the case where we have traced multiple ranks, whether from a single node or multiple nodes, `nsys` GUI allows us to view the reports in a combined fashion on a single timeline (same time-axis for both reports). This is done through the "multi-report view", `file` --> `New multi-report view` or `file` --> `Open` and selecting however many reports we would like to see in a combined timeline, `nsys` prompts the user to allow for a "multi-report view". These can also be viewed separately.

### Profiler Options

In both cases, `nsys` and `ncu`, we have used the standard option sets to generate the profiles. The exhaustive list could be found in the respective documentation pages:

- [Nsight System User Guide](https://docs.nvidia.com/nsight-systems/UserGuide/index.html)
- [Nsight Compute Documentation](https://docs.nvidia.com/nsight-compute/)
- [Nsight Compute CLI](https://docs.nvidia.com/nsight-compute/NsightComputeCli/index.html)

There is much other information provided through these reports. Here we have discussed the way to view the high-level information.

### PyTorch Profiler

Using the PyTorch profiler requires changes in the application source code. A simple example is the following:

```python linenums="1" title="pytorch_profiler_example.py"
from torch.profiler import profile, record_function, ProfilerActivity

# A tracer decorator for a function to be traced
def trace_func(func):
   def wrapper(*args, **kwargs):
      try:
         function_name = func.__func__.__qualname__
      except:
         function_name = func.__qualname__
      with record_function(function_name):
         return func(*args, **kwargs)
   return wrapper

@trace_func
def trace_this_function(a, b, c):
    ...
    ...
    return x, y, z

activities = [ProfilerActivity.CPU, ProfilerActivity.CUDA]

with profile(activities=activities, record_shapes=True) as prof:
    result = trace_this_function(a, b, c)
prof.export_chrome_trace(f"{/path/to/the/trace/dir}/{name/of/the/trace}-{rank}-of-{world_size}.json")
```

The procedure described above works for both single and multi-rank deployments.