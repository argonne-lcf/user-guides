#!/bin/bash
# ============================================================
# Aurora / ALCF compute-node recipe (from scratch):
#   - Build intel/oneapi-hpckit Apptainer image
#   - Create a persistent venv on $HOME
#   - Install numpy + mpi4py
#   - Install Torch XPU (nightly index) + oneCCL bindings
#   - Run a simple XPU DDP smoke test under mpiexec
#
# Run this script on a COMPUTE NODE (interactive job).
# ============================================================

set -euo pipefail

# ----------------------------
# 0) User-configurable knobs
# ----------------------------
NPROCS="${NPROCS:-12}"
PPN="${PPN:-12}"
MASTER_PORT="${MASTER_PORT:-2345}"

# Host libs used for Intel MPI PMI bootstrap on Aurora
PMIX="${PMIX:-/usr/lib64/libpmix.so.2}"
EVENT_DIR="${EVENT_DIR:-/usr/lib64}"

# Use a writable filesystem for workdir (defaults to $HOME)
WORKROOT="${WORKROOT:-/flare/datascience/vsastry/ALCC}"
PROJECT_NAME="${PROJECT_NAME:-xpu_ddp_recipe}"

# Persistent venv location (on host)
VENV="${VENV:-$HOME/.venv_xpu_ddp312_new}"

# Build targets
WORKDIR="$WORKROOT/$PROJECT_NAME"
SIF="$WORKDIR/oneapi-hpckit-xpu.sif"
SBX="$WORKDIR/oneapi_sbx_xpu"

# ----------------------------
# 1) Basic setup
# ----------------------------
module load apptainer

mkdir -p "$WORKDIR"
cd "$WORKDIR"

echo "=== WORKDIR=$WORKDIR"
echo "=== VENV=$VENV"
echo "=== PMIX=$PMIX"
echo "=== EVENT_DIR=$EVENT_DIR"
echo "=== NPROCS=$NPROCS  PPN=$PPN"

export APPTAINERENV_ZE_FLAT_DEVICE_HIERARCHY=FLAT
export APPTAINERENV_ONEAPI_DEVICE_SELECTOR=level_zero:gpu
export APPTAINERENV_SYCL_DEVICE_FILTER=level_zero:gpu

# ----------------------------
# 2) Build image + sandbox (only if missing)
# ----------------------------
if [[ ! -f "$SIF" ]]; then
  echo "=== Building SIF from docker://intel/oneapi-hpckit"
  #apptainer build --fakeroot "$SIF" docker://intel/oneapi-hpckit
else
  echo "=== SIF exists: $SIF"
fi


if [[ ! -d "$SBX" ]]; then
  echo "=== Building sandbox from SIF"
  #apptainer build --sandbox "$SBX" "$SIF"
else
  echo "=== Sandbox exists: $SBX"
fi

SBX_ABS="$(readlink -f "$SBX")"
echo "=== SBX_ABS=$SBX_ABS"

# ----------------------------
# 3) Verify python/pip/venv are present in the image
#    (Do NOT apt-get install; it can fail under fakeroot)
# ----------------------------
echo "=== Verifying python/pip/venv in container"
apptainer exec --pwd / "$SBX_ABS" /bin/sh -lc '
  set -eux
  python3 -V
  python3 -m pip --version
  python3 -m venv -h >/dev/null
  echo "OK: python/pip/venv present"
'


# ----------------------------
# 4) Create venv (host path) + baseline deps
# ----------------------------
if [[ ! -f "$VENV/bin/activate" ]]; then
  echo "=== Creating venv at $VENV"
  apptainer exec --pwd / "$SBX_ABS" /bin/sh -lc "
    set -eux
    python3 -m venv '$VENV'
    . '$VENV/bin/activate'
    pip install -U pip wheel setuptools cython
    pip install -U numpy
    python -c 'import sys; print(sys.executable); import numpy as np; print(\"numpy\", np.__version__)'
  "
else
  echo "=== venv exists: $VENV"
fi

echo "=== Checking mpi4py in venv"

apptainer exec --pwd / "$SBX_ABS" /bin/sh -lc "
  set -e
  . '$VENV/bin/activate'

  if python -c 'import mpi4py' >/dev/null 2>&1; then
    echo 'mpi4py already installed — skipping build'
  else
    echo 'mpi4py not found — building from source'
    MPICC=mpiicx pip install --no-binary=mpi4py --no-build-isolation mpi4py \
    || MPICC=\"mpicc -cc=gcc\" pip install --no-binary=mpi4py --no-build-isolation mpi4py
  fi
"

# ----------------------------
# 6) Install Torch XPU (ONLY if missing or CPU-only)
# ----------------------------
echo "=== Checking torch (XPU) in venv"
apptainer exec --pwd / "$SBX_ABS" /bin/sh -lc "
  set -e
  . '$VENV/bin/activate'

  have_torch=0
  cpu_only=0

  # Does torch import?
  if python -c 'import torch' >/dev/null 2>&1; then
    have_torch=1
    # Detect CPU-only builds (e.g. 2.10.0+cpu)
    python - <<'PY' >/dev/null 2>&1 || cpu_only=1
import torch
assert '+cpu' not in torch.__version__
PY
  fi

  if [ \"\$have_torch\" -eq 1 ] && [ \"\$cpu_only\" -eq 0 ]; then
    echo 'Torch already installed (non-CPU build) — skipping'
    python - <<'PY'
import torch
print('torch:', torch.__version__)
print('has_xpu:', hasattr(torch, 'xpu'))
print('xpu_avail:', torch.xpu.is_available() if hasattr(torch, 'xpu') else None)
print('xpu_count:', torch.xpu.device_count() if hasattr(torch, 'xpu') else None)
PY
  else
    if [ \"\$have_torch\" -eq 1 ]; then
      echo 'CPU-only torch detected — reinstalling XPU build'
      pip uninstall -y torch torchvision torchaudio >/dev/null 2>&1 || true
    else
      echo 'Torch not found — installing XPU build'
    fi

    # Install Torch XPU from PyTorch XPU wheel index
    #pip install --pre torch --no-cache-dir \
    #  --index-url https://download.pytorch.org/whl/nightly/xpu
    pip install torch==2.10.0 --index-url https://download.pytorch.org/whl/xpu
    # Quick sanity print
    python - <<'PY'
import torch
print('torch:', torch.__version__)
print('has_xpu:', hasattr(torch, 'xpu'))
print('xpu_avail:', torch.xpu.is_available() if hasattr(torch, 'xpu') else None)
print('xpu_count:', torch.xpu.device_count() if hasattr(torch, 'xpu') else None)
PY
  fi
"
# GPU visibility
export APPTAINERENV_ONEAPI_DEVICE_SELECTOR=level_zero:gpu
export APPTAINERENV_SYCL_DEVICE_FILTER=level_zero:gpu
export APPTAINERENV_ZE_FLAT_DEVICE_HIERARCHY=FLAT
export APPTAINERENV_ZE_AFFINITY_MASK=0,1,2,3,4,5,6,7,8,9,10,11

# rendezvous (set ONCE here)
MASTER_ADDR="$(hostname -s).hsn.cm.aurora.alcf.anl.gov"
MASTER_PORT=$((20000 + (RANDOM % 20000)))
export APPTAINERENV_MASTER_ADDR="$MASTER_ADDR"
export APPTAINERENV_MASTER_PORT="$MASTER_PORT"
# oneCCL / ZE fd-sharing workaround
export APPTAINERENV_CCL_ATL_TRANSPORT=ofi
export APPTAINERENV_CCL_ZE_IPC=0
export APPTAINERENV_CCL_ZE_IPC_EXCHANGE=sockets
# Intel MPI PMI
export APPTAINERENV_I_MPI_PMI_LIBRARY=/hostlib/libpmix.so.2

mpiexec -n 12 -ppn 12 \
  -genv I_MPI_HYDRA_BOOTSTRAP pmi \
  -genv I_MPI_FABRICS ofi \
  -genv I_MPI_OFI_PROVIDER tcp \
  -genv FI_PROVIDER tcp \
  apptainer exec \
    --pwd / \
    -B "$WORKDIR:$WORKDIR" \
    -B "$PMIX:/hostlib/libpmix.so.2" \
    -B /usr/lib64:/hostevent \
    "$SBX_ABS" /bin/sh -lc "
      set -e
      . '$VENV/bin/activate'
      export LD_LIBRARY_PATH=/hostevent:\$LD_LIBRARY_PATH
      python -u '$WORKDIR/mlp_ddp.py'
    "
