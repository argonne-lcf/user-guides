# Containers on Aurora
Users of Aurora can benefit from container-based workloads for seamless compatibility across Intel systems. This guide details the use of containers on Aurora, including custom container creation, large-scale execution, and common pitfalls.

<!-- We support [Apptainer](https://github.com/apptainer/apptainer) and Podman (documentation coming soon) on Aurora. -->

## Apptainer
Aurora employs [Apptainer](https://github.com/apptainer/apptainer) (formerly known as Singularity) for container management.

### Login and queue a job
```bash
ssh <username>@aurora.alcf.anl.gov
```
Refer to [Getting Started on Aurora](../getting-started-on-aurora.md) for additional information. In particular, you need to set the environment variables that provide access to the proxy host.

!!! note

    The instructions below should be **run directly from a compute node**.

    Explicitly, to request an interactive job (from `aurora-uan`):
    ```bash
    qsub -I -q <your_Queue> -l select=1,walltime=60:00 -A <your_ProjectName> -l filesystems=<fs1:fs2>
    ```

    Refer to [job scheduling and execution](../../running-jobs/index.md) for additional information.

### Loading Apptainer module on a compute node
```bash linenums="1"
# load apptainer
module load apptainer
```

### Building an image from Docker or Argonne GitHub Container Registry
Container images on Aurora can be built by writing Dockerfiles on a local machine and then publishing the image to DockerHub, or by directly building them on an ALCF compute node by writing an Apptainer recipe file. If you prefer to use existing images, you can pull them from various registries like DockerHub and run them on Aurora.

Aurora does not support directly running Docker containers because Docker depends on a root-privileged daemon. However, Docker images can be used as a source format and converted into Apptainer images. These Apptainer images can then be executed as unprivileged containers by users. The following example shows how to build an Apptainer image from a Docker image on Aurora:

```bash linenums="1"
## Build an image
apptainer build --fakeroot intel-optimized-pytorch.sing docker://intel/intel-optimized-pytorch
```

### Example: Hello World using Apptainer
```bash linenums="1"
apptainer exec --fakeroot docker://ghcr.io/apptainer/lolcow cowsay 'Fresh from the internet'
```

### Example: Postgres database using Apptainer
```bash linenums="1" title="apptainer_aurora_example.sh"
# qsub from a UAN/login node
qsub -l select=1 -l walltime=60:00 -A <Projectname> -q <Queue> -l filesystems=<fs1:fs2> -I

# Set proxy on compute node
export HTTP_PROXY="http://proxy.alcf.anl.gov:3128"
export HTTPS_PROXY="http://proxy.alcf.anl.gov:3128"
export http_proxy="http://proxy.alcf.anl.gov:3128"
export https_proxy="http://proxy.alcf.anl.gov:3128"
export ftp_proxy="http://proxy.alcf.anl.gov:3128"
export no_proxy="admin,*.hostmgmt.cm.aurora.alcf.anl.gov,*.alcf.anl.gov,localhost"

# Load apptainer
module load apptainer

# Build postgres image
apptainer build --fakeroot postgres.sing docker://postgres # do this once

# Create an environment file
cat >> pg.env <<EOF
export POSTGRES_USER=pguser
export POSTGRES_PASSWORD=mypguser123
export POSTGRES_DB=mydb
export POSTGRES_INITDB_ARGS="--encoding=UTF-8"
EOF

# Create a data and run directory to bind to the running container
mkdir -p pgdata pgrun

# Start an instance of the container using nohup
nohup apptainer run --fakeroot -B pgrun:/var/run/postgresql -B pgdata:/var/lib/postgresql/data --env-file pg.env postgres.sing postgres &

# Save PID
echo $! > pg_pid.txt
echo "Postgres started with PID $(cat pg_pid.txt)"

# Interact with running container using psql client
apptainer exec \
  --fakeroot \
  -B pgrun:/var/run/postgresql \
  -B pgdata:/var/lib/postgresql/data \
  --env-file pg.env \
  postgres.sing psql -h 127.0.0.1 -p 5432 -U pguser -d mydb -c "SELECT version();"

# Stop the container and kill the process
kill "$(cat pg_pid.txt)"
rm pg_pid.txt
```

### Example: simple MPI + PyTorch DDP workload
On a single Aurora compute node (from an interactive job or non-interactive PBS script), run the following script. The script is an end-to-end example: it builds an Apptainer executable, installs optimized Python, MPI, PyTorch packages, and then executes the DDP test across all 12 XPUs on the node:
```bash linenums="1" title="test_container.sh"
---8<---
docs/aurora/containers/src/test_container.sh
---8<---
```
