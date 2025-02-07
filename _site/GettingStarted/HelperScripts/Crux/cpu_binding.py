import sys
import math

def convert_int_to_string(iarray):
    return ','.join(map(str, iarray))

def construct_cpu_list(offset, num_ranks, depth, num_threads, stride):
    cpu_list = []
    first = offset;
    for i in range(num_ranks):
        for j in range( math.floor(num_threads/depth) ):
            for k in range(depth):
                cpu_list.append(first + j * stride + k)
        cpu_list.append(":")
        first = first + depth

    # convert to string and clean up
    
    cpu_slist = convert_int_to_string( cpu_list )
    cpu_slist = cpu_slist.replace(",:,", ":") # intermediate separators
    cpu_slist = cpu_slist.replace(",:","") # trailing separator
        
    return cpu_slist

# print some examples if incorrect number of arguments provided

if(len(sys.argv) != 6):

    print("Example: cpu_binding.py [Rank 0 offset] [# MPI Ranks] [# Cores per MPI Rank (depth)] [# Threads per Rank] [Stride between threads]")
    
    offset = 0
    num_ranks = 2
    depth = 1
    num_threads = 4
    stride = 64
    print("\nLaunch 2 MPI ranks with 4 OpenMP threads. Threads for each MPI rank are bound to hyperthreads in single core")
    print("$ cpu_binding.py 0 2 1 4 64")
    print( construct_cpu_list(offset, num_ranks, depth, num_threads, stride) )

    
    offset = 1
    num_ranks = 2
    depth = 1
    num_threads = 4
    stride = 64
    print("\nLaunch 2 MPI ranks with 4 OpenMP threads ignoring the first cpu core. Threads for each MPI rank are bound to hyperthreads in single core")
    print("$ cpu_binding.py 1 2 1 4 64")
    print( construct_cpu_list(offset, num_ranks, depth, num_threads, stride) )


    offset = 0
    num_ranks = 2
    depth = 2
    num_threads = 4
    stride = 64
    print("\nLaunch 2 MPI ranks with 4 OpenMP threads. Threads for each MPI rank are bound to hyperthreads in two adjacent cores")
    print("$ cpu_binding.py 0 2 2 4 64")
    print( construct_cpu_list(offset, num_ranks, depth, num_threads, stride) )

else:
    
    offset = int(sys.argv[1])
    num_ranks = int(sys.argv[2])
    depth = int(sys.argv[3])
    num_threads = int(sys.argv[4])
    stride = int(sys.argv[5])
    print( construct_cpu_list(offset, num_ranks, depth, num_threads, stride) )
