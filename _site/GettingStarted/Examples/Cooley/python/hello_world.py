from mpi4py import MPI
import sys

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

print "Hello World from rank %s of %s" %(rank,size)

sys.exit()


