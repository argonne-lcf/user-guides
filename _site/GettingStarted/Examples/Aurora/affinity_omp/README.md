# Compilation w/ oneAPI compiler wrappers
Users are able to build applications on the Aurora login nodes, but may find it convenient to build and test applications on the Aurora compute nodes in short interactive jobs. This also has the benefit of allowing one to quickly submission scripts and allow one to quickly explore `mpiexec` settings within a single job.
```
qsub -I -l select=2,walltime=0:30:00,place=scatter -q EarlyAppAccess -A <PROJECT>

make clean
make

./submit.sh
```
## Example output:
This examples launches 16 MPI ranks on each node with 4 OpenMP threads each bound to a single core. The example below is from running in an interactive job on a Polaris compute node.
```
$ ./submit.sh 
NUM_OF_NODES= 2 TOTAL_NUM_RANKS= 52 RANKS_PER_NODE= 26 THREADS_PER_RANK= 4
To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 0  tid= 0: list_cores= (0)
To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 0  tid= 1: list_cores= (1)
To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 0  tid= 2: list_cores= (2)
To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 0  tid= 3: list_cores= (3)

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 1  tid= 0: list_cores= (4)
To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 1  tid= 1: list_cores= (5)
To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 1  tid= 2: list_cores= (6)
To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 1  tid= 3: list_cores= (7)
...
To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 25  tid= 0: list_cores= (100)
To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 25  tid= 1: list_cores= (101)
To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 25  tid= 2: list_cores= (102)
To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 25  tid= 3: list_cores= (103)

To affinity and beyond!! nname= x1922c3s0b0n0  rnk= 26  tid= 0: list_cores= (0)
To affinity and beyond!! nname= x1922c3s0b0n0  rnk= 26  tid= 1: list_cores= (1)
To affinity and beyond!! nname= x1922c3s0b0n0  rnk= 26  tid= 2: list_cores= (2)
To affinity and beyond!! nname= x1922c3s0b0n0  rnk= 26  tid= 3: list_cores= (3)
...
To affinity and beyond!! nname= x1922c3s0b0n0  rnk= 51  tid= 0: list_cores= (100)
To affinity and beyond!! nname= x1922c3s0b0n0  rnk= 51  tid= 1: list_cores= (101)
To affinity and beyond!! nname= x1922c3s0b0n0  rnk= 51  tid= 2: list_cores= (102)
To affinity and beyond!! nname= x1922c3s0b0n0  rnk= 51  tid= 3: list_cores= (103)
```
