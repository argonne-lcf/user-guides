# ezpz

[`ezpz`](https://github.com/saforem2/ezpz) is a thin layer over PyTorch
distributed that removes the per-machine boilerplate: rank and topology
detection, backend selection, hostfile parsing, and launcher invocation.
The same script runs on Aurora, Polaris, and Perlmutter without an
`if machine == ...` branch.

Full documentation: [ezpz.cool](https://ezpz.cool).

## The problem it solves

Moving a working PyTorch script between ALCF systems — or from ALCF to
NERSC — normally means rewriting the parts that have nothing to do with
the model:

| Concern | Aurora / Sunspot | Polaris / Sophia | Perlmutter |
|---|---|---|---|
| Scheduler | PBS Pro | PBS Pro | SLURM |
| Launcher | `mpiexec` | `mpiexec` | `srun` |
| Accelerator | Intel XPU | NVIDIA | NVIDIA |
| Backend | `xccl` | `nccl` | `nccl` |
| Hostfile | `$PBS_NODEFILE` | `$PBS_NODEFILE` | `$SLURM_NODELIST` |
| Local rank var | `PALS_LOCAL_RANKID` | `PMI_LOCAL_RANK` | `SLURM_LOCAL_ID` |

`ezpz` detects all of it at runtime. The
[Supported Systems](https://ezpz.cool/notes/systems/) page lists the full
matrix, including the hostname prefixes used for detection (`x4*` for
Aurora, `x1*` for Sunspot, and so on).

It also sets the Aurora-specific environment that is easy to get wrong:

- **`ZE_FLAT_DEVICE_HIERARCHY=FLAT`**, so all 12 tiles are visible. Without
  it only 6 devices appear and every rank with local rank ≥ 6 fails with
  `device index out of range`.
- **`CCL_OP_SYNC=1`**, which works around an oneCCL async-completion hang
  that affects FSDP2 with tensor parallel degree > 1
  ([AuroraBugTracking#174](https://github.com/argonne-lcf/AuroraBugTracking/issues/174)).

## Setup

```bash
module load frameworks
pip install "git+https://github.com/saforem2/ezpz"  # (1)!
```

1. Install from the repository. The name `ezpz` on PyPI belongs to an
   unrelated package.

Then, inside a job script or an interactive allocation:

```bash
source <(curl -LsSf https://bit.ly/ezpz-utils)
ezpz_setup_env
```

`ezpz_setup_env` loads the right modules for the current machine,
activates the Python environment, and exports the environment variables
above. Verify with:

```bash
ezpz doctor   # environment and dependency check
ezpz test     # distributed PyTorch smoke test
```

## Launching

`ezpz launch` wraps the scheduler's launcher. It reads the hostfile,
computes ranks-per-node from the detected accelerator count, and applies
CPU binding:

```bash
ezpz launch python3 -m ezpz.examples.test
```

That command is unchanged across every supported machine; on Aurora it
becomes a 12-rank-per-node `mpiexec --pmi=pmix`, on Perlmutter an `srun`.
See [`ezpz launch`](https://ezpz.cool/cli/launch/) for the flags, including
`--auto-retry` for resubmitting around node failures.

!!! example "Minimal PBS job on Aurora"

    ```bash linenums="1" title="job.sh"
    #!/bin/bash -l
    #PBS -l select=2
    #PBS -l walltime=00:30:00
    #PBS -q debug
    #PBS -A <ProjectName>
    #PBS -l filesystems=home:flare

    cd "${PBS_O_WORKDIR}"
    source <(curl -LsSf https://bit.ly/ezpz-utils)
    ezpz_setup_env

    ezpz launch python3 -m ezpz.examples.test --model small
    ```

## Using it in your own script

Two calls replace the usual distributed preamble:

```python
import ezpz

rank = ezpz.setup_torch()          # (1)!
device = ezpz.get_torch_device_type()   # "xpu" on Aurora, "cuda" elsewhere

model = MyModel().to(device)
model = ezpz.distributed.wrap_model_for_ddp(model)
```

1. Initializes the process group with the correct backend, binds the
   device, and returns the global rank. No `MASTER_ADDR` / `MASTER_PORT`
   plumbing.

See [Distributed Training](https://ezpz.cool/guides/distributed-training/)
for the full API.

## Features

| Area | What it does | Details |
|---|---|---|
| Launcher | Scheduler-aware `mpiexec` / `srun` wrapper with CPU binding and `--auto-retry` | [`ezpz launch`](https://ezpz.cool/cli/launch/) |
| Setup | Per-machine module loading and environment export | [Quickstart](https://ezpz.cool/quickstart/) |
| Distributed | Backend selection, rank/topology detection, DDP and FSDP2 wrapping | [Distributed Training](https://ezpz.cool/guides/distributed-training/) |
| Parallelism | FSDP2, tensor parallel, sequence parallel, 2D meshes | [Parallelism](https://ezpz.cool/notes/parallelism/) |
| Examples | Runnable training examples with a shared model-size ladder (`debug` → `xxxl`) | [Examples](https://ezpz.cool/examples/) |
| Profiling | `torch.profiler` wrapper with automatic XPU/CUDA activity selection | [Profiling](https://ezpz.cool/examples/profiler/) |
| Metrics | `History` for per-iteration metrics, plots, and W&B / MLflow logging | [History](https://ezpz.cool/history/) |
| Checkpointing | Distributed checkpoint save/restore, resumable after node failure | [Checkpoint & Restart](https://ezpz.cool/guides/checkpoint-restart/) |
| Fault handling | Node-kill injection and automatic retry | [Fault Injection](https://ezpz.cool/guides/fault-injection/) |
| Diagnostics | `ezpz doctor`, `ezpz test` | [`ezpz test`](https://ezpz.cool/cli/test/) |

## Examples

Each example takes a `--model` size from `debug` (laptop-runnable) through
`xxxl` (~10B), so the same command scales from a smoke test to a real job:

```bash
ezpz launch python3 -m ezpz.examples.test    --model small
ezpz launch python3 -m ezpz.examples.fsdp    --model large
ezpz launch python3 -m ezpz.examples.fsdp_tp --model large --tp 2   # (1)!
```

1. FSDP2 + tensor parallel. On Aurora this requires `CCL_OP_SYNC=1`, which
   `ezpz_setup_env` sets; see
   [AuroraBugTracking#174](https://github.com/argonne-lcf/AuroraBugTracking/issues/174).

The full set is listed under [Examples](https://ezpz.cool/examples/), and
[`ezpz benchmark`](https://ezpz.cool/examples/) runs them in sequence with a
summary report.

## Other systems

The same workflow applies on Polaris and Sophia (PBS, `nccl`) and on
Perlmutter (SLURM, `srun`); the NERSC specifics are in the
[Perlmutter guide](https://ezpz.cool/guides/perlmutter/).

## Getting help

Issues and questions: [saforem2/ezpz](https://github.com/saforem2/ezpz/issues).
Common failure modes are collected in
[Troubleshooting](https://ezpz.cool/troubleshooting/).
