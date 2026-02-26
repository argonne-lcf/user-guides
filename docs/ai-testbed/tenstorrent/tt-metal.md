# Steps to run tt-metal examples. 

1. Login in to tt-02 node, and clone the tt-metal repository
```console
git clone https://github.com/tenstorrent/tt-metal.git --recurse-submodules 
cd tt-metal
# Set the environment variables 
export TT_METAL_HOME=$(pwd) && export PYTHONPATH=$(pwd) 
```

2. When using the system for the first time, create the virtual environment using the create_venv.sh script, followed by activating the virtual environment. 
```console
./create_venv.sh # skip if virtual environment is already created.
source python_env/bin/activate       
```
```console
2026-02-26 21:56:09.538 | info     |     Distributed | Using auto discovery to generate mesh graph. (metal_context.cpp:936)
2026-02-26 21:56:09.538 | info     |     Distributed | Constructing control plane using auto-discovery (no mesh graph descriptor). (metal_context.cpp:913)
2026-02-26 21:56:09.539 | info     |          Fabric | TopologyMapper mapping start (mesh=0): n_log=32, n_phys=32, log_deg_hist={2:4, 3:16, 4:12}, phys_deg_hist={4:32} (topology_mapper_utils.cpp:171)
2026-02-26 21:56:09.540 | info     |          Fabric | TopologyMapper mapping start (mesh=0): n_log=32, n_phys=32, log_deg_hist={2:4, 3:16, 4:12}, phys_deg_hist={4:32} (topology_mapper_utils.cpp:171)
2026-02-26 21:56:09.540 | info     |          Fabric | TopologyMapper: Using 2 pinning(s) for mesh 0: [fabric_node=(M0, D0) (mesh_id=0, chip_id=0) -> ASIC position (tray=1, loc=1), fabric_node=(M0, D31) (mesh_id=0, chip_id=31) -> ASIC position (tray=4, loc=1)] (topology_mapper_utils.cpp:257)
2026-02-26 21:56:10.248 | warning  |           Metal | Opening subset of mmio devices slows down UMD read/write to remote chips. If opening more devices, consider using CreateDevices API. (device_manager.cpp:301)
2026-02-26 21:56:12.691 | info     |          Always | Output vector of size 409600 (lab_eltwise_binary.cpp:336)
2026-02-26 21:56:12.693 | info     |          Always | Test Passed (lab_eltwise_binary.cpp:365)
2026-02-26 21:56:12.712 | info     |          Device | Closing user mode device drivers (tt_cluster.cpp:471)
2026-02-26 21:56:12.712 | info     |             UMD | Closing devices in cluster (cluster.cpp:980)
```

3. Compile the tt-metal directory. All the dependencies that require root access are already installed on the tt-02 node. 
```console 
./build_metal.sh
```

# tt-nn examples 
```console
./build/ttnn/examples/example_lab_eltwise_binary
TT_METAL_DEVICE_PROFILER=1 ./build/ttnn/examples/example_lab_eltwise_binary # with profiler 
```

# tt-metal examples
To run tt-metal examples, first build the programming-examples directory. 
```console
./build_metal.sh --build-programming-examples
```

Run a Gemm example
```console 
./build/programming_examples/metal_example_matmul_single_core
```
```console

2026-02-26 21:20:05.608 | info     |     Distributed | Using auto discovery to generate mesh graph. (metal_context.cpp:936)
2026-02-26 21:20:05.608 | info     |     Distributed | Constructing control plane using auto-discovery (no mesh graph descriptor). (metal_context.cpp:913)
2026-02-26 21:20:05.609 | info     |          Fabric | TopologyMapper mapping start (mesh=0): n_log=32, n_phys=32, log_deg_hist={2:4, 3:16, 4:12}, phys_deg_hist={4:32} (topology_mapper_utils.cpp:171)
2026-02-26 21:20:05.610 | info     |          Fabric | TopologyMapper mapping start (mesh=0): n_log=32, n_phys=32, log_deg_hist={2:4, 3:16, 4:12}, phys_deg_hist={4:32} (topology_mapper_utils.cpp:171)
2026-02-26 21:20:05.610 | info     |          Fabric | TopologyMapper: Using 2 pinning(s) for mesh 0: [fabric_node=(M0, D0) (mesh_id=0, chip_id=0) -> ASIC position (tray=1, loc=1), fabric_node=(M0, D31) (mesh_id=0, chip_id=31) -> ASIC position (tray=4, loc=1)] (topology_mapper_utils.cpp:257)
2026-02-26 21:20:05.774 | warning  |           Metal | Opening subset of mmio devices slows down UMD read/write to remote chips. If opening more devices, consider using CreateDevices API. (device_manager.cpp:301)
Output vector of size 409600
Metalium vs Golden -- PCC = 0.9787865
Test Passed
2026-02-26 21:20:06.145 | info     |          Device | Closing user mode device drivers (tt_cluster.cpp:471)
2026-02-26 21:20:06.145 | info     |             UMD | Closing devices in cluster (cluster.cpp:980)
```


