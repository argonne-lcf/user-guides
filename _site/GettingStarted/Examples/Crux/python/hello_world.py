from mpi4py import MPI
import sys
import socket

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

print("Hello World from rank {} of {} on {}".format(rank, size, socket.gethostname()))

sys.exit()


