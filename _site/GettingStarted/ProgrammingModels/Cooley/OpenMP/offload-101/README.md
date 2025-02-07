
 # OpenMP Offload 101

 This covers the three basic offloading concepts:
 transferring execution control, expressing parallelism, and 
 mapping data.

 There are three sections:

 1. Offloading code to the device and getting device info
 2. Expressing parallelism
 3. Mapping data

 First, cd into either cpp or fortran directory.

 ## Offloading code to the device and getting device info
 ```
 make 01_target_construct
 ./01_target_construct
 nvidia-smi
 nvprof ./01_target_construct
 ```
 ## Expressing parallelism 

 Artificial example showing a loop parallelized with
 `target teams distribute parallel for`, but the
 only thing in the loop is a printf statement
 to show that the iterations of the loop are split
 up over the threads.

 ```
 make 02_target_teams_parallel
 ./02_target_teams_parallel
 # modify to num_teams() thread_limit()
 ./02_target_teams_parallel
 ```

 We checked that we could execute code on the device and
 that we are spawning threads to be able to run in parallel
 on the device. Let's move to a more complicated example
 and talk about data transfer.

 ## Mapping data

 ```
 make 03_map
 # initially just `parallel for`, but then want to run on device,
 # so change to `target teams distribute parallel for`
 # here we actually want to do something on the device
 # so we need to give data to the device to compute on.
 # this is where the map clause comes in.
 # we want to map arrays a and b to the device, compute on the
 # device, and then map the arrays back.
 ./03_map
 nvprof --print-gpu-trace ./03_map

 # slightly more complicated. we have multiple arrays, and
 # want to call daxpy on them. like good programmers, we
 # pull the code into a routine for reuse.
 make 03_map_function
 ./03_map_function
 nvprof --print-gpu-trace ./03_map_function
 # lots of data transfer. do we need this much?

 # unstructured data mapping
 make 03_map_unstructured_function
 ./03_map_unstructured_function
 nvprof --print-gpu-trace ./03_map_unstructured_function
 ```