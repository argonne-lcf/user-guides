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
 * Sort array using blocking send/recv between 2 ranks.
 *
 * The master process prepares the data and sends the latter half
 * of the array to the other rank. Each rank sorts it half. The
 * master then merges the sorted halved together. The two ranks
 * communicate using blocking send/recv.
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
    if (size != 2) {
        printf("Please run with exactly 2 ranks\n");
        MPI_Finalize();
        exit(0);
    }

    int first_half = floor((double) NUM_ELEMENTS / 2);
    int second_half = NUM_ELEMENTS - first_half;

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

        /* send second half of the data to the other rank */
        MPI_Send(&data[first_half], second_half, MPI_INT, 1, 0, MPI_COMM_WORLD);
        /* sort the first half of the data */
        qsort(data, first_half, sizeof(int), compare_int);

        /* receive sorted second half of the data */
        MPI_Recv(&data[first_half], second_half, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /* merge the two sorted halves (using sort on the whole array) */
        merge(data, first_half, &data[first_half], second_half);

        /* display sorted array */
        printf("Sorted:\t\t");
        for (i = 0; i < NUM_ELEMENTS; i++)
            printf("%d ", data[i]);
        printf("\n");
    } else if (rank == 1) {
        /* receive half of the data */
        MPI_Recv(data, second_half, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        /* sort the received data */
        qsort(data, second_half, sizeof(int), compare_int);

        /* send back the sorted data */
        MPI_Send(data, second_half, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
