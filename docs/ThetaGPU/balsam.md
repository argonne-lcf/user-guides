# Balsam

!!! warning "Experimental"
    Balsam support on ThetaGPU is under active development and testing
    
A Balsam environment configured for ThetaGPU can be obtained by replacing your home directory
configuration and sourcing the setup script:

```bash
rm -r ~/.balsam
source /lus/theta-fs0/software/thetagpu/balsam/setup.sh
```

You may then proceed to initialize and activate Balsam databases as usual:

```bash
balsam init my-db
```

Jobs submitted with `--job-mode serial` will use a ThetaGPU job template that places jobs on GPUs with round-robin assignment.
To leverage this ability, users should submit jobs with the appropriate `node_packing_count`.  For more details, refer to the [balsam documentation](https://balsam.readthedocs.io/en/latest/).
