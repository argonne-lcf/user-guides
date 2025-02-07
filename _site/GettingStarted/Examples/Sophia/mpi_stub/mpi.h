typedef int MPI_Comm;

int MPI_COMM_WORLD = 0;
void MPI_Init(int * argc, char *** argv) {};

void MPI_Comm_rank(int comm, int * rnk)
{
  *rnk = 0;	
};

void MPI_Comm_size(int comm, int * nrnk)
{
  *nrnk = 1;	
};

void MPI_Barrier(int comm) {};
void MPI_Finalize() {};

