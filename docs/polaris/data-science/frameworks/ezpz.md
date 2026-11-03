---
description: Launch distributed PyTorch on Polaris with ezpz, which detects ranks, backend, and launcher for each system.
tags:
  - PyTorch
  - Distributed Training
keywords:
  - torchrun
  - multi-node
  - data parallel
authors:
  - Sam Foreman
last_verified: 2026-10-12
---

# ezpz

--8<-- "./docs/aurora/data-science/frameworks/ezpz.md:intro"

## Setup

```bash
module use /soft/modulefiles
module load conda
conda activate
pip install "git+https://github.com/saforem2/ezpz"  # (1)!
```

1. Install from the repository. The name `ezpz` on PyPI belongs to an
   unrelated package.

--8<-- "./docs/aurora/data-science/frameworks/ezpz.md:setup"

## Launching

--8<-- "./docs/aurora/data-science/frameworks/ezpz.md:launch"

That command is unchanged across every supported machine; on Polaris it
becomes a 4-rank-per-node `mpiexec`, one rank per A100 GPU.
See [`ezpz launch`](https://ezpz.cool/cli/launch/) for the flags, including
`--auto-retry` for resubmitting around node failures.

/// example | Minimal PBS job on Polaris

```bash linenums="1" title="job.sh"
#!/bin/bash -l
#PBS -l select=2:system=polaris
#PBS -l walltime=00:30:00
#PBS -q debug
#PBS -A <project>
#PBS -l filesystems=home:eagle

cd "${PBS_O_WORKDIR}"
source <(curl -LsSf https://bit.ly/ezpz-utils)
ezpz_setup_env

ezpz launch python3 -m ezpz.examples.test --model small
```

///

--8<-- "./docs/aurora/data-science/frameworks/ezpz.md:api"

```bash
ezpz launch python3 -m ezpz.examples.test    --model small
ezpz launch python3 -m ezpz.examples.fsdp    --model large
ezpz launch python3 -m ezpz.examples.fsdp_tp --model large --tp 2   # FSDP2 + tensor parallel
```

--8<-- "./docs/aurora/data-science/frameworks/ezpz.md:examples-more"

## Other systems

The same workflow applies on [Aurora](../../../aurora/data-science/frameworks/ezpz.md),
which needs a few extra environment settings that `ezpz_setup_env` handles.

--8<-- "./docs/aurora/data-science/frameworks/ezpz.md:help"
