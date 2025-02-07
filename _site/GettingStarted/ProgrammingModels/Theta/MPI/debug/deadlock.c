/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char **argv)
{
    int rank, size;
    int i, data;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        int *sendbuf;
        MPI_Request *sendreqs;

        /* setup send operations */
        sendreqs = malloc(sizeof(MPI_Request) * size);
        sendbuf = malloc(sizeof(int) * size);
        for (i = 0; i < size; i++) {
            sendbuf[i] = i * 10;
            MPI_Isend(&sendbuf[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &sendreqs[i]);
        }
        MPI_Waitall(size, sendreqs, MPI_STATUSES_IGNORE);
    } else {
        MPI_Recv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Finalize();
    return 0;
}
