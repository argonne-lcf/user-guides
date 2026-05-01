#!/usr/bin/env bash
# Build and install mpifileutils and dependencies.
# Supports:
#   --with-daos              Enable DAOS support (default)
#   --without-daos           Disable DAOS support
#   --base-dir <path>        Source/build directory (default: /path/to/apps)
#   --base-install-dir <p>   Install root (default: /path/to/apps/install)
#   --daos-install-dir <p>   DAOS prefix (default: /usr)

set -euo pipefail

# -----------------------------
# Defaults (override via flags)
# -----------------------------
BASE_DIR="/path/to/apps"
BASE_INSTALL_DIR="/path/to/apps/install"
DAOS_INSTALL_DIR="/usr"
WITH_DAOS=1

usage() {
  cat <<EOF
Usage: $0 [options]
  --with-daos
  --without-daos
  --base-dir <path>
  --base-install-dir <path>
  --daos-install-dir <path>
  -h, --help
EOF
}

# -----------------------------
# Parse args
# -----------------------------
while [[ $# -gt 0 ]]; do
  case "$1" in
    --with-daos) WITH_DAOS=1; shift ;;
    --without-daos) WITH_DAOS=0; shift ;;
    --base-dir) BASE_DIR="$2"; shift 2 ;;
    --base-install-dir) BASE_INSTALL_DIR="$2"; shift 2 ;;
    --daos-install-dir) DAOS_INSTALL_DIR="$2"; shift 2 ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown argument: $1"; usage; exit 1 ;;
  esac
done

INSTALL_PREFIX="${BASE_INSTALL_DIR}/mpifileutils"

# Load required build tools (environment-specific; adjust as needed)
module load cmake autoconf/2.72

mkdir -p "${BASE_DIR}" "${INSTALL_PREFIX}"
pushd "${BASE_DIR}" >/dev/null

# -----------------------------
# Download dependency tarballs
# -----------------------------
wget -nc https://github.com/hpc/libcircle/releases/download/v0.3/libcircle-0.3.0.tar.gz
wget -nc https://github.com/llnl/lwgrp/releases/download/v1.0.4/lwgrp-1.0.4.tar.gz
wget -nc https://github.com/llnl/dtcmp/releases/download/v1.1.4/dtcmp-1.1.4.tar.gz
wget -nc https://github.com/libarchive/libarchive/releases/download/v3.5.1/libarchive-3.5.1.tar.gz
wget -nc https://github.com/hpc/mpifileutils/releases/download/v0.12/mpifileutils-v0.12.tgz

# -----------------------------
# Build libcircle
# -----------------------------
tar -zxf libcircle-0.3.0.tar.gz
pushd libcircle-0.3.0 >/dev/null
./configure --prefix="${INSTALL_PREFIX}"
make -j"$(nproc)"
make install
popd >/dev/null
rm -f libcircle-0.3.0.tar.gz

# -----------------------------
# Build lwgrp
# -----------------------------
tar -zxf lwgrp-1.0.4.tar.gz
pushd lwgrp-1.0.4 >/dev/null
./configure --prefix="${INSTALL_PREFIX}"
make -j"$(nproc)"
make install
popd >/dev/null
rm -f lwgrp-1.0.4.tar.gz

# -----------------------------
# Build dtcmp (depends on lwgrp)
# -----------------------------
tar -zxf dtcmp-1.1.4.tar.gz
pushd dtcmp-1.1.4 >/dev/null
./configure --prefix="${INSTALL_PREFIX}" --with-lwgrp="${INSTALL_PREFIX}"
make -j"$(nproc)"
make install
popd >/dev/null
rm -f dtcmp-1.1.4.tar.gz

# -----------------------------
# Build libarchive
# -----------------------------
tar -zxf libarchive-3.5.1.tar.gz
pushd libarchive-3.5.1 >/dev/null
./configure --prefix="${INSTALL_PREFIX}"
make -j"$(nproc)"
make install
popd >/dev/null
rm -f libarchive-3.5.1.tar.gz

# -----------------------------
# Build mpifileutils
# -----------------------------
tar -zxf mpifileutils-v0.12.tgz
pushd mpifileutils-v0.12 >/dev/null
mkdir -p build
pushd build >/dev/null

CC="$(which gcc)"

CMAKE_ARGS=(
  ..
  -DWITH_LibArchive_PREFIX="${INSTALL_PREFIX}"
  -DWITH_DTCMP_PREFIX="${INSTALL_PREFIX}"
  -DWITH_LibCircle_PREFIX="${INSTALL_PREFIX}"
  -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}"
)

if [[ "${WITH_DAOS}" -eq 1 ]]; then
  CMAKE_ARGS+=(
    -DWITH_DAOS_PREFIX="${DAOS_INSTALL_DIR}"
    -DENABLE_DAOS=ON
  )
else
  CMAKE_ARGS+=(-DENABLE_DAOS=OFF)
fi

cmake "${CMAKE_ARGS[@]}"
make -j8 install

popd >/dev/null
popd >/dev/null
popd >/dev/null

echo "Done. Installed to: ${INSTALL_PREFIX}"
echo "DAOS enabled: ${WITH_DAOS}"
