import mpi4py
from mpi4py import MPI

mpi4py.rc.initialize = True
mpi4py.rc.threads = True
mpi4py.rc.thread_level = 'multiple'


def main():
    if not MPI.Is_initialized():
        print('Initializing MPI with Init_thread')
        MPI.Init_thread()

    comm = MPI.COMM_WORLD
    size = comm.Get_size()
    rank = comm.Get_rank()
    name = MPI.Get_processor_name()

    print(
        f'Rank {rank} / {size} says hello from node {name}'
    )
    comm.Barrier()

    if rank == 0:
        print('\n'.join([
            'Some info:',
            '----------',
            f'MPI number of threads: {MPI.Query_thread()}',
            f'MPI Vendor: {MPI.get_vendor()}',
            f'MPI Library version: {MPI.Get_library_version()}',
        ]))

    min_loc = comm.allreduce((1.5 * rank, rank), op=MPI.MINLOC)
    max_loc = comm.allreduce((1.5 * rank, rank), op=MPI.MAXLOC)

    if rank == 0:
        print(f'Min val: {min_loc[0]} at rank: {min_loc[1]}')
        print(f'Max val: {max_loc[0]}, at rank: {max_loc[1]}')


if __name__ == '__main__':
    main()
