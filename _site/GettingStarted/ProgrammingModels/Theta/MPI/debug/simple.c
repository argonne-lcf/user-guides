/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int rank, size, target, source;
    int sendbuf, recvbuf;
    MPI_Request reqs[2];

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* get communication partners */
    target = (rank + 1) % size;
    source = (rank - 1);
    if (source < 0)
        source += size;

    sendbuf = 42;
    recvbuf = 0;

    MPI_Irecv(&recvbuf, 3, MPI_INT, source, 0, MPI_COMM_WORLD, &reqs[0]);

    /* stop here to view posted recvs */
    printf("recvs posted, recvbuf = %d\n", recvbuf);

    MPI_Isend(&sendbuf, 3, MPI_INT, target, 0, MPI_COMM_WORLD, &reqs[1]);

    /* stop here to view send ops */
    printf("sends issued\n");

    MPI_Waitall(2, reqs, MPI_STATUSES_IGNORE);

    printf("communication complete, recvbuf = %d\n", recvbuf);

    MPI_Finalize();
    return 0;
}
