# Profiling Deep Learning Applications

On Aurora, we can use the `unitrace` profiler from Intel to profile deep learning applications. Refer to the [`unitrace` documentation page](https://github.com/intel/pti-gpu/tree/master/tools/unitrace) for details.

PyTorch's own [`torch.profiler`](https://pytorch.org/docs/stable/profiler.html) also works on Aurora and attributes time to PyTorch operators rather than device-level activity. The two are complementary; see [PyTorch Profiler](#pytorch-profiler) below.

## Example Usage

We can use `unitrace` to trace an application running on multiple ranks and multiple nodes. A simple example, where we use a wrapper script to trace rank 0 on each node of a 4-node job running a PyTorch application, is below.

There are several important shell variables in the wrapper, which may require modification:

```bash linenums="1" title="unitrace_wrapper.sh"
#!/bin/bash
## This wrapper should be used with unitrace to trace in any number of nodes.
## The script for this example is set up to trace rank 0 of the first 4 nodes in the case of
## profiling a job running on more than 4 nodes.
FNAME_EXT=$(basename "$2")
FNAME="${FNAME_EXT%%.*}"

NNODES=`wc -l < $PBS_NODEFILE`

WORK_DIR=/path/to/the/Python/program
UNITRACE_EXE=$(command -v unitrace) # (1)!
UNITRACE_BIN=$(dirname ${UNITRACE_EXE})
UNITRACE_DIR=$(dirname ${UNITRACE_BIN})
UNITRACE_LIB=${UNITRACE_DIR}/lib64
DTAG=$(date +%F_%H%M%S)
UNITRACE_OUTDIR=${WORK_DIR}/logs/unitrace_profiles/name_of_choice_json_n${NNODES}_${DTAG}/${FNAME}_n${NNODES}_${DTAG}
mkdir -p ${UNITRACE_OUTDIR}
UNITRACE_OPTS=" --ccl-summary-report --chrome-mpi-logging --chrome-sycl-logging \
--chrome-device-logging \
--chrome-ccl-logging --chrome-call-logging --chrome-dnn-logging --device-timing --host-timing \
--output-dir-path ${UNITRACE_OUTDIR} --output ${UNITRACE_OUTDIR}/UNITRACE_${FNAME}_n${NNODES}_${DTAG}.txt "  # (2)!

export LD_LIBRARY_PATH=${UNITRACE_LIB}:${UNITRACE_BIN}:$LD_LIBRARY_PATH

# Use $PMIX_RANK for MPICH and $SLURM_PROCID with srun.
PROFRANK=0 # (3)!
RANKCUTOFF=48 # (4)!

if [[ $PALS_LOCAL_RANKID -eq $PROFRANK ]] && [[ $PMIX_RANK -lt $RANKCUTOFF ]]; then
  echo "On rank $PMIX_RANK, collecting traces "
  $UNITRACE_EXE $UNITRACE_OPTS "$@"
else
  "$@"
fi
```

1. `UNITRACE_EXE`: The `unitrace` on your `PATH`, provided by the `pti-gpu` module that `module load frameworks` loads. The other `UNITRACE_*` paths are derived from it, so they follow programming environment updates. To use a different build, set `UNITRACE_EXE` to its full path.
2. `UNITRACE_OPTS`: These are the options that `unitrace` uses to trace data at different levels. Based on the number of options, the sizes of the output profiles will vary. Usually, enabling more options leads to a larger profile (in terms of storage in MB).
3. `PROFRANK`: As implemented, this variable is set by the user to trace the rank of choice. For example, this wrapper will trace rank 0 on each node.
4. `RANKCUTOFF`: This variable is Aurora-specific. As we can run as many as 12 ranks per node (without using CCS), the first 4 nodes of a job will have 48 ranks running. This provides the upper cutoff of the label (in number) of ranks, beyond which `unitrace` will not trace any rank. A user can change the number according to the number of maximum ranks running per node to set up how many ranks to be traced. `unitrace` will produce a profile (`json` file, by default) per traced rank. This profile can be viewed using the [Perfetto trace viewer](https://ui.perfetto.dev/).

### Deployment

The wrapper above can be deployed using the following PBS job script:

```bash linenums="1" title="job_script.sh"
#!/bin/bash -x
#PBS -l select=4
#PBS -l place=scatter
#PBS -l walltime=00:10:00
#PBS -q debug-scaling
#PBS -l filesystems=<fs1:fs2>
#PBS -A <project>

WORK_DIR=/path/to/the/Python/program
UNITRACE_WRAPPER=${WORK_DIR}/unitrace_wrapper.sh

# MPI and OpenMP settings
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=12

NRANKS=$((NNODES * NRANKS_PER_NODE))

module load frameworks

mpiexec --pmi=pmix -n ${NRANKS} -ppn ${NRANKS_PER_NODE} -l --line-buffer \
${UNITRACE_WRAPPER} python ${WORK_DIR}/application.py 
```
## PyTorch Profiler

`unitrace` traces at the Level Zero / oneCCL layer. When the question is "which PyTorch operator is slow" rather than "which SYCL kernel is slow", `torch.profiler` attributes time to operators and Python frames instead. It is common to use `torch.profiler` to find the expensive operator, then `unitrace` to see what the device is doing underneath it.

On Aurora, `torch.profiler` needs `ProfilerActivity.XPU` in its activity list:

```python linenums="1" title="profile_snippet.py"
import contextlib
import os

import torch
from torch.profiler import ProfilerActivity, profile, schedule

activities = [ProfilerActivity.CPU]
if hasattr(torch, "xpu") and torch.xpu.is_available():
    activities.append(ProfilerActivity.XPU)  # (1)!

def trace_handler(p):
    print(p.key_averages().table(sort_by="self_xpu_time_total", row_limit=20))
    p.export_chrome_trace(f"trace-rank0-step{p.step_num}.json")

rank = int(os.environ.get("PMIX_RANK", 0))  # (2)!
ctx = (
    profile(
        activities=activities,
        schedule=schedule(wait=1, warmup=2, active=3, repeat=1),  # (3)!
        on_trace_ready=trace_handler,
        record_shapes=True,
        with_stack=True,
    )
    if rank == 0
    else contextlib.nullcontext()
)

with ctx as prof:
    for step, batch in enumerate(dataloader):
        train_step(batch)
        if prof is not None:
            prof.step()  # (4)!
```

1. `ProfilerActivity.XPU` is what captures device-side activity on Aurora's Intel GPUs. Without it the trace contains only CPU events, which is a common cause of "the profiler ran but shows no GPU work".
2. Profile one rank. Each rank writes its own trace file, and with 12 ranks per node those add up to gigabytes quickly.
3. The schedule skips `wait` steps, runs `warmup` steps to let the profiler settle, then records `active` steps. The first trace therefore appears only after `wait + warmup + active` steps -- six with these values. A loop shorter than that produces **no output at all**.
4. `prof.step()` advances the schedule. If it is never called the profiler stays in `wait` indefinitely and writes nothing, **without raising an error** -- the most common reason a profiled run produces no trace.

The resulting JSON files load in the [Perfetto trace viewer](https://ui.perfetto.dev/), `chrome://tracing`, or TensorBoard -- the same viewer used for `unitrace` output above.

### Using ezpz

[`ezpz`](https://github.com/saforem2/ezpz) wraps the above, including the device detection and rank gating, so the same command profiles on Aurora, Sunspot, Polaris, and Perlmutter:

```bash
module load frameworks
pip install "git+https://github.com/saforem2/ezpz"  # (1)!

ezpz launch python3 -m ezpz.examples.profiler --profile --rank-zero-only
```

1. Install from the repository, not PyPI: the name `ezpz` on PyPI belongs to an unrelated package.

That runs a small distributed training loop, writes a Chrome trace per completed profiling cycle, and logs a key-averages table. [`get_torch_profiler()`](https://saforem2.github.io/ezpz/python/Code-Reference/profile/) selects the activity set from what is available -- `XPU` on Aurora and Sunspot, `CUDA` on Polaris and Perlmutter -- so no Aurora-specific branch is needed in user code.

The same `--profile` flag works on the full training examples:

```bash
ezpz launch python3 -m ezpz.examples.fsdp    --model small --profile --rank-zero-only
ezpz launch python3 -m ezpz.examples.fsdp_tp --model small --tp 2 --profile --rank-zero-only --no-with-stack  # (1)!
```

1. `--no-with-stack` is **required on Aurora** for `fsdp_tp`. With Python
   stacks enabled, `key_averages()` walks them recursively in torch's
   `profiler_util.py`, and FSDP2 + TP call stacks exceed Python's
   1000-frame recursion limit:

    ```output
    RecursionError: maximum recursion depth exceeded
    ```

    The run exits 143 having written **zero** traces. Measured on 2 nodes
    (24 ranks): with stacks, 0 traces; with `--no-with-stack`, 5 traces.
    Raising `sys.setrecursionlimit()` does not help — it replaces the
    crash with a hang. See
    [saforem2/ezpz#275](https://github.com/saforem2/ezpz/issues/275).

Schedule shape is controlled by `--pytorch-profiler-{wait,warmup,active,repeat}`. Note that **every rank profiles unless `--rank-zero-only` is passed**. Full details, including how to shrink large traces, are in the [ezpz profiling guide](https://saforem2.github.io/ezpz/examples/profiler/).

/// warning | Profiling `fsdp_tp` is not portable today

The small `ezpz.examples.profiler` loop profiles cleanly everywhere,
but `fsdp_tp` at `--tp 2` does not:

| system | `--profile` on `fsdp_tp --tp 2` |
|---|---|
| Polaris | works |
| **Aurora** | needs `--no-with-stack` (see above) |
| **Perlmutter** | **hangs**, killed at timeout, zero traces |

On Perlmutter the profiler alone is sufficient to wedge the run — a
controlled 2×2 shows both profiled arms timing out and both
unprofiled arms completing, on either NCCL transport. No flag avoids
it today. Tracked in
[saforem2/ezpz#275](https://github.com/saforem2/ezpz/issues/275).
///

#### From Python

The same wrapper is callable directly, for profiling your own training loop rather than one of the examples. `get_profiling_context` is the higher-level entry point -- it builds the schedule and a trace handler that writes a Chrome trace and logs a key-averages table:

```python linenums="1" title="profile_with_ezpz.py"
from ezpz.profile import get_profiling_context

with get_profiling_context(
    profiler_type="torch",
    wait=1, warmup=2, active=3, repeat=1,
    rank_zero_only=True,  # (1)!
    outdir="./traces",
) as prof:
    for step, batch in enumerate(dataloader):
        train_step(batch)
        if prof is not None:  # (2)!
            prof.step()
```

1. With `rank_zero_only=True`, every rank other than 0 receives a `contextlib.nullcontext`, so only rank 0 writes traces.
2. Required, not stylistic: on the non-profiling ranks `prof` **is** `None`, so calling `.step()` unguarded raises `AttributeError` on every rank but one.

For full control over activities and the trace handler, `get_torch_profiler` is the thinner wrapper -- it selects `ProfilerActivity.XPU` / `CUDA` / `CPU` for you and applies the same rank gating, then passes everything else through to `torch.profiler.profile`:

```python linenums="1" title="profile_with_ezpz_lowlevel.py"
import torch
import ezpz
from ezpz.profile import get_torch_profiler

def trace_handler(p):
    p.export_chrome_trace(f"trace-step{p.step_num}.json")

with get_torch_profiler(
    rank=ezpz.get_rank(),
    schedule=torch.profiler.schedule(wait=1, warmup=2, active=3, repeat=1),
    on_trace_ready=trace_handler,
    rank_zero_only=True,
    with_stack=True,
) as prof:
    for step, batch in enumerate(dataloader):
        train_step(batch)
        if prof is not None:
            prof.step()
```

Both are documented in the [`ezpz.profile` API reference](https://saforem2.github.io/ezpz/python/Code-Reference/profile/).
