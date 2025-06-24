# Containers on Aurora
Users of Aurora can benefit from container-based workloads for seamless compatibility across Intel systems. This guide details the use of containers on Aurora, including custom container creation, large-scale execution, and common pitfalls.

We support Apptainer and Podman (documentation coming soon) on Aurora.

## Apptainer
Aurora employs Apptainer (formerly known as Singularity) for container management.

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

### Load Modules on Compute Node
```bash linenums="1"
# load apptainer
module load apptainer
```

### Building from Docker or Argonne GitHub Container Registry
Containers on Aurora can be built by writing Dockerfiles on a local machine and then publishing the container to DockerHub, or by directly building them on an ALCF compute node by writing an Apptainer recipe file. If you prefer to use existing containers, you can pull them from various registries like DockerHub and run them on Aurora.

Since Docker requires root privileges, which users do not have on Aurora, existing Docker containers must be converted to Apptainer. To build a Docker-based container on Aurora, use the following as an example:

```bash linenums="1"
## Build an image
apptainer build --fakeroot intel-optimized-pytorch.sing docker://intel/intel-optimized-pytorch
```

### Example to run Hello World using Apptainer
```bash linenums="1"
apptainer exec --fakeroot docker://ghcr.io/apptainer/lolcow cowsay 'Fresh from the internet'
```

### Example to run Postgres Database using Apptainer
To run Postgres on an Aurora compute node, below is a full example.

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
