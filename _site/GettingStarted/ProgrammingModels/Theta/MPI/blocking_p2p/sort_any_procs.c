/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

/*
 * Sort array using blocking send/recv between any number of ranks.
 *
 * The master process prepares the data and sends chunks of the array
 * to the other ranks. Each rank sorts it chunk and sends the sorted
 * chunks back to the master. The master then merges all the individually
 * sorted chunks together. The ranks communicate using blocking send/recv.
 */

#define NUM_ELEMENTS 50

static int compare_int(const void *a, const void *b)
{
    return (*(int *) a - *(int *) b);
}

/* Merge sorted arrays a[] and b[] into a[].
 * Length of a[] must be sum of lengths of a[] and b[] */
static void merge(int *a, int numel_a, int *b, int numel_b)
{
    int *sorted = (int *) malloc((numel_a + numel_b) * sizeof *a);
    int i, a_i = 0, b_i = 0;
    /* merge a[] and b[] into sorted[] */
    for (i = 0; i < (numel_a + numel_b); i++) {
        if (a_i < numel_a && b_i < numel_b) {
            if (a[a_i] < b[b_i]) {
                sorted[i] = a[a_i];
                a_i++;
            } else {
                sorted[i] = b[b_i];
                b_i++;
            }
        } else {
            if (a_i < numel_a) {
                sorted[i] = a[a_i];
                a_i++;
            } else {
                sorted[i] = b[b_i];
                b_i++;
            }
        }
    }
    /* copy sorted[] into a[] */
    memcpy(a, sorted, (numel_a + numel_b) * sizeof *sorted);
    free(sorted);
}

int main(int argc, char **argv)
{
    int rank, size, data[NUM_ELEMENTS];
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int chunk_size = (int) ceil((double) NUM_ELEMENTS / size);
    int last_chunk_size = chunk_size - (chunk_size * size - NUM_ELEMENTS);

    srand(0);

    if (rank == 0) {
        /* prepare data and display it */
        int i;
        printf("Unsorted:\t");
        for (i = 0; i < NUM_ELEMENTS; i++) {
            data[i] = rand() % NUM_ELEMENTS;
            printf("%d ", data[i]);
        }
        printf("\n");

        /* send a chunk of the data to each of the other ranks */
        int target;
        for (target = 1; target < size; target++)
            MPI_Send(&data[target * chunk_size],
                     (target == size - 1) ? last_chunk_size : chunk_size, MPI_INT, target, 0,
                     MPI_COMM_WORLD);
        /* sort the first chunk of the data */
        qsort(data, chunk_size, sizeof(int), compare_int);

        /* receive sorted latter chunks of the data */
        for (target = 1; target < size; target++)
            MPI_Recv(&data[target * chunk_size],
                     (target == size - 1) ? last_chunk_size : chunk_size, MPI_INT, target, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /* merge all the sorted chunks */
        int chunk_i;
        int sorted_so_far = chunk_size;
        for (chunk_i = 1; chunk_i < size; chunk_i++) {
            merge(data, sorted_so_far, &data[chunk_i * chunk_size],
                  (chunk_i == size - 1) ? last_chunk_size : chunk_size);
            sorted_so_far += chunk_size;
        }

        /* display sorted array */
        printf("Sorted:\t\t");
        for (i = 0; i < NUM_ELEMENTS; i++)
            printf("%d ", data[i]);
        printf("\n");
    } else {
        if (rank == size - 1)
            chunk_size = last_chunk_size;
        /* receive a chunk of the data */
        MPI_Recv(data, chunk_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        /* sort the received data */
        qsort(data, chunk_size, sizeof(int), compare_int);

        /* send back the sorted data */
        MPI_Send(data, chunk_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
