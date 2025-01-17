# Containers on Aurora
Users of Aurora can benefit from container-based workloads for seamless compatibility across Intel systems. This guide details the use of containers on Aurora, including custom container creation, large-scale execution, and common pitfalls.

We support Apptainer and Podman (documentation coming soon) on Aurora.

## Apptainer

### Setup

Aurora employs Apptainer (formerly known as Singularity) for container management. To set up Apptainer, run on a compute node the following:

```bash linenums="1"
qsub -l select=1 -l walltime=60:00 -A <Projectname> -q <Queue> -I
# proxy settings
export HTTP_PROXY="http://proxy.alcf.anl.gov:3128"
export HTTPS_PROXY="http://proxy.alcf.anl.gov:3128"
export http_proxy="http://proxy.alcf.anl.gov:3128"
export https_proxy="http://proxy.alcf.anl.gov:3128"
export ftp_proxy="http://proxy.alcf.anl.gov:3128"
export no_proxy="admin,polaris-adminvm-01,localhost,*.cm.polaris.alcf.anl.gov,polaris-*,*.polaris.alcf.anl.gov,*.alcf.anl.gov"

# load apptainer
module load spack-pe-gcc
module load apptainer
module load fuse-overlayfs

# example to run a simple hello world
apptainer exec docker://ghcr.io/apptainer/lolcow cowsay 'Fresh from the internet'
```

### Building from Docker or Argonne GitHub Container Registry

Containers on Aurora can be built by writing Dockerfiles on a local machine and then publish the container to DockerHub, or by directly building them on ALCF compute node by writing an Apptainer recipe file. If you prefer to use existing containers, you can pull them from various registries like DockerHub and run them on Aurora.

Since Docker requires root privileges, which users do not have on Aurora, existing Docker containers must be converted to Apptainer. To build a Docker-based container on Aurora, use the following as an example:

```bash
qsub -l select=1 -l walltime=60:00 -A <Projectname> -q <Queue> -I
# proxy settings
export HTTP_PROXY="http://proxy.alcf.anl.gov:3128"
export HTTPS_PROXY="http://proxy.alcf.anl.gov:3128"
export http_proxy="http://proxy.alcf.anl.gov:3128"
export https_proxy="http://proxy.alcf.anl.gov:3128"
export ftp_proxy="http://proxy.alcf.anl.gov:3128"
export no_proxy="admin,polaris-adminvm-01,localhost,*.cm.polaris.alcf.anl.gov,polaris-*,*.polaris.alcf.anl.gov,*.alcf.anl.gov"

# load apptainer
module load spack-pe-gcc
module load apptainer
module load fuse-overlayfs

apptainer build intel-optimized-pytorch.sing docker://intel/intel-optimized-pytorch
```


### Example to run Postgres Database using Apptainer

To run Postgres on Aurora compute node,

```bash linenums="1"
qsub -l select=1 -l walltime=60:00 -A <Projectname> -q <Queue> -I
# proxy settings
export HTTP_PROXY="http://proxy.alcf.anl.gov:3128"
export HTTPS_PROXY="http://proxy.alcf.anl.gov:3128"
export http_proxy="http://proxy.alcf.anl.gov:3128"
export https_proxy="http://proxy.alcf.anl.gov:3128"
export ftp_proxy="http://proxy.alcf.anl.gov:3128"
export no_proxy="admin,polaris-adminvm-01,localhost,*.cm.polaris.alcf.anl.gov,polaris-*,*.polaris.alcf.anl.gov,*.alcf.anl.gov"

# load apptainer
module load spack-pe-gcc
module load apptainer
module load fuse-overlayfs

apptainer build postgres.sing docker://postgres #do this once

# create an environment file
cat >> pg.env <<EOF
> export POSTGRES_USER=pguser
> export POSTGRES_PASSWORD=mypguser123
> export POSTGRES_DB=mydb
> export POSTGRES_INITDB_ARGS="--encoding=UTF-8"
> EOF

# Create a data and run directory to bind to the running container
mkdir -p pgdata pgrun

# Start an instance of the container using nohup
nohup apptainer run -B pgrun:/var/run/postgresql -B pgdata:/var/lib/postgresql/data --env-file pg.env postgres.sing postgres &

# Save PID
echo $! > pg_pid.txt
echo "Postgres started with PID $(cat pg_pid.txt)"

# Interact with running container using psql client
apptainer exec \
  -B pgrun:/var/run/postgresql \
  -B pgdata:/var/lib/postgresql/data \
  --env-file pg.env \
  postgres.sing psql -h 127.0.0.1 -p 5432  -U pguser -d mydb -c "SELECT version();"

# Stop the container and kill the process
kill "$(cat pg_pid.txt)"
rm pg_pid.txt
```




