/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

#include "bspmm.h"

int setup(int rank, int nprocs, int argc, char **argv, int *mat_dim_ptr)
{
    int mat_dim;

    if (argc != 2) {
        if (!rank)
            printf("usage: bspmm_mpi <m>\n");
        return 1;
    }

    mat_dim = atoi(argv[1]);    /* matrix dimension */

    if (mat_dim % BLK_DIM != 0) {
        int new_mat_dim = ((mat_dim + BLK_DIM - 1) / BLK_DIM) * BLK_DIM;
        if (!rank)
            printf("mat_dim: %d -> %d\n", mat_dim, new_mat_dim);
        mat_dim = new_mat_dim;
    }

    (*mat_dim_ptr) = mat_dim;
    return 0;
}

void init_mats(int mat_dim, double *mat_a, double *mat_b, double *mat_c)
{
    int i, j, bi, bj;

    srand(0);

    for (bj = 0; bj < mat_dim; bj += BLK_DIM) {
        for (bi = 0; bi < mat_dim; bi += BLK_DIM) {
            /* initialize mat_a */
            if (rand() < SPARSITY_A * RAND_MAX) {
                for (j = bj; j < bj + BLK_DIM; j++)
                    for (i = bi; i < bi + BLK_DIM; i++)
                        mat_a[j + i * mat_dim] = 0.0;
            } else {
                for (j = bj; j < bj + BLK_DIM; j++)
                    for (i = bi; i < bi + BLK_DIM; i++)
                        mat_a[j + i * mat_dim] = (double) rand() / (RAND_MAX / RAND_RANGE + 1);
            }
            /* initialize mat_b */
            if (rand() < SPARSITY_B * RAND_MAX) {
                for (j = bj; j < bj + BLK_DIM; j++)
                    for (i = bi; i < bi + BLK_DIM; i++)
                        mat_b[j + i * mat_dim] = 0.0;
            } else {
                for (j = bj; j < bj + BLK_DIM; j++)
                    for (i = bi; i < bi + BLK_DIM; i++)
                        mat_b[j + i * mat_dim] = (double) rand() / (RAND_MAX / RAND_RANGE + 1);
            }
        }
    }
    /* reset mat_c */
    memset(mat_c, 0, sizeof(double) * mat_dim * mat_dim);
}

void check_mats(double *mat_a, double *mat_b, double *mat_c, int mat_dim)
{
    int i, j, k, r;
    int bogus = 0;
    double temp_c;
    double diff, max_diff = 0.0;

    /* pick up 1000 values to check correctness */
    for (r = 0; r < 1000; r++) {
        i = rand() % mat_dim;
        j = rand() % mat_dim;
        temp_c = 0.0;
        for (k = 0; k < mat_dim; k++)
            temp_c += mat_a[k + i * mat_dim] * mat_b[j + k * mat_dim];
        diff = mat_c[j + i * mat_dim] - temp_c;
        if (fabs(diff) > 0.00001) {
            bogus = 1;
            if (fabs(diff) > fabs(max_diff))
                max_diff = diff;
        }
    }

    if (bogus)
        printf("\nTEST FAILED: (%.5f MAX diff)\n\n", max_diff);
    else
        printf("\nTEST PASSED\n\n");
}
