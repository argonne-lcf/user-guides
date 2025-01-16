# ADIOS2

The Adaptable Input/Output (I/O) System (ADIOS2)
is a framework for I/O and streaming of scientific data developed as part of the U.S. DOE Exascale Computing Project.
ADIOS2 conveniently provides C, C++, Fortran, and Python API for traditional file system I/O, 
as well as API for transporting data between applications running concurrently on HPC systems. 
Data transport with ADIOS2 can be performed via the file system, wide-area-networks (WAN), remote direct memory access (RDMA), 
or MPI to construct a variety of workflows, such as in-situ (or in-transit) visualization, data analysis, and ML training and inference 
from ongoing simulations.

Users are invited to find more information about ADIOS2 on their [GitHub page](https://github.com/ornladios/ADIOS2) and their [documentation](https://adios2.readthedocs.io/en/v2.10.2/).

## Accessing ADIOS2 on Aurora

Pre-built modules are available to all users which enable access to the latest version of ADIOS2 (v2.10).
These modules can be displayed with `module avail adios2` and comprise of a CPU only build and a SYCL build of the library, with the SYCL build being the default.
Note that both ADIOS2 modules also load a Spack installation of Python 3.10 with the `numpy`, `mpi4py`, and `adios2` packages.
For instance, the default SYCL build can be loaded by executing

```bash
module load adios2
```

A custom build of ADIOS2 is also possible on Aurora.
In this case, we recommend the users start with the following install script for a base build

```bash linenums="1" title="install_adios2.sh
export CRAYPE_LINK_TYPE=dynamic
git clone https://github.com/ornladios/ADIOS2.git ADIOS2
mkdir adios2-build && cd adios2-build
cmake \
    -DCMAKE_INSTALL_PREFIX=${PWD}/install \
    -DADIOS2_BUILD_EXAMPLES=ON \
    -DADIOS2_USE_MPI=ON \
    -DADIOS2_HAVE_MPI_CLIENT_SERVER=ON \
    -DADIOS2_USE_SST=ON \
    -DADIOS2_USE_SSC=ON \
    -DADIOS2_USE_Python=OFF \
    -DADIOS2_USE_HDF5=OFF \
    -DADIOS2_USE_BZip2=OFF \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    ../ADIOS2 2>&1 | tee adios2_config.log
make -j 8 2>&1 | tee adios2_build.log
make install 2>&1 | tee adios2_install.log
cd ..
```

!!! info "Building the Python bindings"

	The Python bindings for ADIOS2 can be built by setting `ADIOS2_USE_Python=ON`, however this requires a Python 3 installation to be found. We recommend users load the Python AI/ML module with `module load frameworks` and build ADIOS2 under this environment. This will require users to augemnt their Python path with `export PYTHONPATH=$PYTHONPATH:/path/to/adios2-build/install/lib/python3.10/site-packages` in order to use the `adios2` package. Alternatively, users can use a custom Python installation, but note that ADIOS2 requires `numpy` and `mpi4py` as well. 

A full list of CMake options is available at the [documentation](https://adios2.readthedocs.io/en/latest/setting_up/setting_up.html#install-from-source).

## Python Hello World Example

Here we show a basic example of using ADIOS2 to stream data between two Python `mpi4py` applications.
This simple workflow sets up a data producer and a data consumer, each looping over a workflow iteration loop.
At every iteration, the producer writes data to the stream and the consumer reads the data.
The ADIOS2 IO engine is set to SST for data streaming, and the engine parameters are set to force the producer to halt util the consumer has read the data for a given step.
This is not a requirement, and can be modified with the `RendezvousReaderCount`, `QueueFullPolicy` and `QueueLimit` parameters.
More information on the SST engine parameters can be found at the [documentation](https://adios2.readthedocs.io/en/latest/engines/engines.html#sst-sustainable-staging-transport).

```python linenums="1" title="producer.py"
from mpi4py import MPI
import numpy as np
from adios2 import Stream, Adios, bindings
from time import sleep

# MPI Init
COMM = MPI.COMM_WORLD
RANK = COMM.Get_rank()
SIZE = COMM.Get_size()

if __name__ == '__main__':
    # Create new communicator (needed for launch on MPMD mode)
    color = 3231
    app_comm = COMM.Split(color, RANK)
    arank = app_comm.Get_rank()
    asize = app_comm.Get_size()
    adios = Adios(app_comm)

    # ADIOS IO
    io = adios.declare_io("simai")
    io.set_engine("SST")
    parameters = {
        'RendezvousReaderCount': '1', # options: 1 for sync, 0 for async
        'QueueFullPolicy': 'Block', # options: Block, Discard
        'QueueLimit': '1', # options: 0 for no limit
        'DataTransport': 'RDMA', # options: MPI, WAN, UCX, RDMA
        'OpenTimeoutSecs': '600', # number of seconds SST is to wait for a peer connection on Open()
    }
    io.set_parameters(parameters)

    myArray = np.array([0, 1, 2, 3, 4], dtype=np.float32)
    nx = len(myArray)
    myArray = nx * arank + myArray
    increment = nx * asize * 1.0

    # Loop over workflow steps and send write at each step
    workflow_steps = 2
    with Stream(io, "data_stream", "w", app_comm) as stream:
        for istep in range(workflow_steps):
            sleep(3)
            if arank==0: print(f"\nIteration {istep}:", flush=True)
            app_comm.Barrier()
            stream.begin_step()
            if istep>0: myArray += increment
            stream.write("y", myArray, [asize * nx], [arank * nx], [nx])
            print(f"\tProducer [{arank}]: sent data = {myArray}", flush=True)
            stream.end_step()
```

```python linenums="1" title="consumer.py"
from mpi4py import MPI
import numpy as np
from adios2 import Stream, Adios, bindings

# MPI Init
COMM = MPI.COMM_WORLD
RANK = COMM.Get_rank()
SIZE = COMM.Get_size()

if __name__ == '__main__':
    # Create new communicator (needed for launch on MPMD mode)
    color = 3230
    app_comm = COMM.Split(color, RANK)
    asize = app_comm.Get_size()
    arank = app_comm.Get_rank()
    adios = Adios(app_comm)

    # ADIOS IO
    io = adios.declare_io("simai")
    io.set_engine("SST")
    parameters = {
        'RendezvousReaderCount': '1', # options: 1 for sync, 0 for async
        'QueueFullPolicy': 'Block', # options: Block, Discard
        'QueueLimit': '1', # options: 0 for no limit
        'DataTransport': 'RDMA', # options: MPI, WAN, UCX, RDMA
        'OpenTimeoutSecs': '600', # number of seconds SST is to wait for a peer connection on Open()
    }
    io.set_parameters(parameters)

    # Loop over workflow steps and read data at each step
    workflow_steps = 2
    with Stream(io, "data_stream", "r", app_comm) as stream:
        for istep in range(workflow_steps):
            stream.begin_step()
            var = stream.inquire_variable("y")
            shape = var.shape()
            count = int(shape[0] / asize)
            start = count * arank
            if arank == asize - 1:
                count += shape[0] % asize
            data = stream.read("y", [start], [count])
            print(f"\tConsumer [{arank}]: received data {data}",flush=True)
            stream.end_step()
```

The example can be run from an interactive session with the following script, 
which runs the producer and consumer with two ranks per node across `NODES` and places the producer on sockect 0 and the consumer on socket 1 of each node.
The producer and consumer can also be run on separare nodes by specifying the `--hostfile` or `--hostlist` in the `mpiexec` commands.

```bash linenuma="1" name="run_adios_example.sh"
#!/bin/bash

module load adios2

export OMP_PROC_BIND=spread
export OMP_PLACES=threads

NODES=$(cat $PBS_NODEFILE | wc -l)
PROCS_PER_NODE=2
PROCS=$((NODES * PROCS_PER_NODE))

# Run Python example
mpiexec -n $PROCS --ppn $PROCS_PER_NODE --cpu-bind list:1:2 python producer.py &
mpiexec -n $PROCS --ppn $PROCS_PER_NODE --cpu-bind list:53:54 python consumer.py
wait
```

!!! info "Selecting the SST Data Transport Plane"

	The SST data transport plane can be selected with the parameter `DataTransport`. We recommend using RDMA for the moment, however note that it requires running the applications on more than 1 node. The WAN data plane can also be used, but it may result in slower data transfer performance at scale. The MPI data plane is currently not available, but we are working on resolving the issue with the ADIOS2 team.





