#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
  {
    int ret;
    int myrank, nprocs;
    int inbuf, outbuf;
    MPI_Request req;
    MPI_Status status;

    ret=MPI_Init(&argc,&argv);
 
    if (ret!=MPI_SUCCESS)
      {
	fprintf(stderr,"MPI_Init() failed\n");
	abort();
      }


    ret=MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
 
    if (ret!=MPI_SUCCESS)
      {
	fprintf(stderr,"msg_init: MPI_Comm_rank() failed\n");
	abort();
      }

    ret=MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
 
    if (ret!=MPI_SUCCESS)
      {
	fprintf(stderr,"msg_init: MPI_Comm_size() failed\n");
	abort();
      }

    
    outbuf=myrank*10;

    if (myrank==0)
      {
	printf("%d: Hello!\n",myrank);
	MPI_Isend(&outbuf,1,MPI_INT,
		  (myrank+1)%nprocs,0,MPI_COMM_WORLD,&req);
      }
    else
      {
	MPI_Recv(&inbuf,1,MPI_INT,
		 MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);

        /* Only print a message from some ranks */
        if ( (myrank < 8) || (myrank==nprocs-1) )
	  printf("%d: Hello!\n",myrank);

        if (myrank<(nprocs-1))
	  MPI_Isend(&outbuf,1,MPI_INT,
		    (myrank+1)%nprocs,0,MPI_COMM_WORLD,&req);
      }

    MPI_Finalize();

    return(0);
  }


