#!/bin/sh

qsub -n 1 -t 10 -A Catalyst --attrs filesystems=home,grand,eagle ./run_lammps.sh


