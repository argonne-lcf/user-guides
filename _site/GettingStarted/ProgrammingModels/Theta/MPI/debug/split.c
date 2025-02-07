/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int rank, size;
    int color, split_rank, split_size;
    int *sendbuf, *recvbuf;
    MPI_Comm split_comm;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* color evens and odds */
    color = (rank % 2 == 0);
    MPI_Comm_split(MPI_COMM_WORLD, color, 0, &split_comm);
    if (color == 1)
        MPI_Comm_set_name(split_comm, "Even Comm");
    else
        MPI_Comm_set_name(split_comm, "Odd Comm");

    MPI_Comm_rank(split_comm, &split_rank);
    MPI_Comm_size(split_comm, &split_size);

    /* setup some comm buffers */
    sendbuf = malloc(sizeof(int) * split_size);
    recvbuf = malloc(sizeof(int) * split_size);

    /* odd comm */
    if (color == 0) {
        int i, curr, num_ops = split_size * 2 + 1;
        MPI_Request *reqs = malloc(sizeof(MPI_Request) * num_ops);

        for (i = 0, curr = 0; i < split_size; i++) {
            MPI_Irecv(&recvbuf[i], 1, MPI_INT, i, 0, split_comm, &reqs[curr++]);
        }

        for (i = 0; i < split_size; i++) {
            sendbuf[i] = rand();
            sleep(1);
            MPI_Isend(&sendbuf[i], 1, MPI_INT, i, 0, split_comm, &reqs[curr++]);
        }

        if (split_rank == 0) {
            MPI_Isend(NULL, 0, MPI_INT, 0, 0, MPI_COMM_WORLD, &reqs[curr++]);
            MPI_Waitall(num_ops, reqs, MPI_STATUSES_IGNORE);
        } else {
            MPI_Waitall(num_ops - 1, reqs, MPI_STATUSES_IGNORE);
        }
        free(reqs);
    }

    /* even comm */
    if (color == 1) {
        int i, curr, num_ops = split_size * 2 + 1;
        MPI_Request *reqs = malloc(sizeof(MPI_Request) * num_ops);

        for (i = 0, curr = 0; i < split_size; i++) {
            MPI_Irecv(&recvbuf[i], 1, MPI_INT, i, 0, split_comm, &reqs[curr++]);
        }

        for (i = 0; i < split_size; i++) {
            sendbuf[i] = rand();
            sleep(2);
            MPI_Isend(&sendbuf[i], 1, MPI_INT, i, 0, split_comm, &reqs[curr++]);
        }

        if (split_rank == 0) {
            MPI_Irecv(NULL, 0, MPI_INT, 1, 0, MPI_COMM_WORLD, &reqs[curr++]);
            MPI_Waitall(num_ops, reqs, MPI_STATUSES_IGNORE);
        } else {
            MPI_Waitall(num_ops - 1, reqs, MPI_STATUSES_IGNORE);
        }
        free(reqs);
    }

    free(sendbuf);
    free(recvbuf);

    MPI_Finalize();
    return 0;
}
