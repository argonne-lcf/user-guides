# Launching a Singularity container with MPI

```
#!/bin/bash
SINGULARITYBIN=$(which singularity)
CONTAINER=${HOME}/singularity_images/hpl-mofed5-cuda11runtime_verbs.sif
OMPI_MCA_orte_launch_agent=$(cat <<EOF
$SINGULARITYBIN run --nv $CONTAINER orted
EOF
)
export SINGULARITYENV_OMPI_MCA_orte_launch_agent=${OMPI_MCA_orte_launch_agent}
$SINGULARITYBIN run --nv --cleanenv \
    $CONTAINER \
    mpirun \
      -H thetagpu01:1,thetagpu02:1 \
      -n 2 \
      --mca plm_rsh_args "-p22" \
      hostname
```
