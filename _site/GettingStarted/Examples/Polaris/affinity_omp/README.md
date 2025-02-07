# Compilation w/ Cray compiler wrappers
Users are able to build applications on the Polaris login nodes, but may find it convenient to build and test applications on the Polaris compute nodes in short interactive jobs. This also has the benefit of allowing one to quickly submission scripts and allow one to quickly explore `mpiexec` settings within a single job.
```
qsub -I -l select=2,walltime=0:30:00 -q debug -l filesystems=home:grand:eagle -A <PROJECT>

make clean
make

./submit.sh
```
## Example output:
This examples launches 16 MPI ranks on each node with 4 OpenMP threads each bound to a single core. The example below is from running in an interactive job on a Polaris compute node.
```
$ ./submit.sh 
NUM_OF_NODES= 2 TOTAL_NUM_RANKS= 32 RANKS_PER_NODE= 16 THREADS_PER_RANK= 4
To affinity and beyond!! nname= x3007c0s13b0n0  rnk= 0  tid= 0: list_cores= (0)
To affinity and beyond!! nname= x3007c0s13b0n0  rnk= 0  tid= 1: list_cores= (1)
To affinity and beyond!! nname= x3007c0s13b0n0  rnk= 0  tid= 2: list_cores= (2)
To affinity and beyond!! nname= x3007c0s13b0n0  rnk= 0  tid= 3: list_cores= (3)

To affinity and beyond!! nname= x3007c0s13b0n0  rnk= 1  tid= 0: list_cores= (4)
To affinity and beyond!! nname= x3007c0s13b0n0  rnk= 1  tid= 1: list_cores= (5)
To affinity and beyond!! nname= x3007c0s13b0n0  rnk= 1  tid= 2: list_cores= (6)
To affinity and beyond!! nname= x3007c0s13b0n0  rnk= 1  tid= 3: list_cores= (7)
...
To affinity and beyond!! nname= x3007c0s13b0n0  rnk= 15  tid= 0: list_cores= (60)
To affinity and beyond!! nname= x3007c0s13b0n0  rnk= 15  tid= 1: list_cores= (61)
To affinity and beyond!! nname= x3007c0s13b0n0  rnk= 15  tid= 2: list_cores= (62)
To affinity and beyond!! nname= x3007c0s13b0n0  rnk= 15  tid= 3: list_cores= (63)

To affinity and beyond!! nname= x3007c0s13b1n0  rnk= 16  tid= 0: list_cores= (0)
To affinity and beyond!! nname= x3007c0s13b1n0  rnk= 16  tid= 1: list_cores= (1)
To affinity and beyond!! nname= x3007c0s13b1n0  rnk= 16  tid= 2: list_cores= (2)
To affinity and beyond!! nname= x3007c0s13b1n0  rnk= 16  tid= 3: list_cores= (3)
...
To affinity and beyond!! nname= x3007c0s13b1n0  rnk= 31  tid= 0: list_cores= (60)
To affinity and beyond!! nname= x3007c0s13b1n0  rnk= 31  tid= 1: list_cores= (61)
To affinity and beyond!! nname= x3007c0s13b1n0  rnk= 31  tid= 2: list_cores= (62)
To affinity and beyond!! nname= x3007c0s13b1n0  rnk= 31  tid= 3: list_cores= (63)

```
