# Python

At a future date, we will provide prebuilt `conda` environments containing CPU-optimized builds of `torch`, `tensorflow` (both with `horovod` support for multi-node calculations), `jax`, and many other commonly used Python modules.

In the meantime, users should be able to create their own local environments to begin work on Crux. In this example, `mpi4py` is installed.

```bash
python3 -m venv ~/_test_env
. ~/_test_env/bin/activate
pip install mpi4py
```

This new virtual environment can then be used in a batch job as in this simple hello_world example available in the GettingStarted [repo](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Crux/python).

```bash
$ cat hello_world.py
from mpi4py import MPI
import sys
import socket

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

print("Hello World from rank {} of {} on {}".format(rank, size, socket.gethostname()))

sys.exit()
```

```bash
$ qsub ./submit.sh
12345.crux-pbs-0001.head.cm.crux.alcf.anl.gov

$ cat submit.sh.o12345
NUM_OF_NODES= 2 TOTAL_NUM_RANKS= 8 RANKS_PER_NODE= 4 THREADS_PER_RANK= 1
Hello World from rank 0 of 8 on x1000c0s0b1n1
Hello World from rank 1 of 8 on x1000c0s0b1n1
Hello World from rank 2 of 8 on x1000c0s0b1n1
Hello World from rank 3 of 8 on x1000c0s0b1n1
Hello World from rank 4 of 8 on x1000c0s1b0n0
Hello World from rank 5 of 8 on x1000c0s1b0n0
Hello World from rank 6 of 8 on x1000c0s1b0n0
Hello World from rank 7 of 8 on x1000c0s1b0n0
```