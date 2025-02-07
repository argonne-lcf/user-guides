/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

#include "bspmm.h"

/*
 * Block sparse matrix multiplication using send/receive.
 *
 * A, B, and C denote submatrices (BLK_DIM x BLK_DIM) and n is blk_num
 *
 * | C11 ... C1n |   | A11 ... A1n |    | B11 ... B1n |
 * |  . .     .  |   |  . .     .  |    |  . .     .  |
 * |  .  Cij  .  | = |  .  Aik  .  | *  |  .  Bkj  .  |
 * |  .     . .  |   |  .     . .  |    |  .     . .  |
 * | Cn1 ... Cnn |   | An1 ... Ann |    | Bn1 ... Bnn |
 *
 * bspmm parallelizes i and k; there are n^2 parallel computations of Cij += Aik * Bkj
 * Work id (0 <= id < n^2) is associated with each computation as follows
 *   (i, k) = (id / n, id % n)
 *
 * The master process sends entire matrices A and B to worker processes
 * and sums up the calculated C that are received from workers.
 */

int is_zero_local(double *local_mat);

void dgemm(double *local_a, double *local_b, double *local_c);

void pack_global_to_local(double *local_mat, double *global_mat, int mat_dim, int global_i,
                          int global_j);

void unpack_local_to_global(double *global_mat, double *local_mat, int mat_dim, int global_i,
                            int global_j);

void add_local_to_global(double *global_mat, double *local_mat, int mat_dim, int global_i,
                         int global_j);

int main(int argc, char **argv)
{
    int rank, nprocs;
    int mat_dim, blk_num;
    int work_id, work_id_len;
    double *mat_a, *mat_b, *mat_c, *mat_tmp_c;
    double *local_a, *local_b, *local_c;

    double t1, t2;

    /* initialize MPI environment */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    /* argument checking and setting */
    if (setup(rank, nprocs, argc, argv, &mat_dim)) {
        MPI_Finalize();
        exit(0);
    }
    if (nprocs == 1) {
        printf("nprocs must be more than 1.\n");
        MPI_Finalize();
        exit(0);
    }

    /* number of blocks in one dimension */
    blk_num = mat_dim / BLK_DIM;

    /* initialize matrices */
    if (!rank) {
        mat_a = (double *) malloc(4 * mat_dim * mat_dim * sizeof(double));
        mat_b = mat_a + mat_dim * mat_dim;
        mat_c = mat_b + mat_dim * mat_dim;
        mat_tmp_c = mat_c + mat_dim * mat_dim;
        init_mats(mat_dim, mat_a, mat_b, mat_c);
    } else {
        mat_a = (double *) malloc(3 * mat_dim * mat_dim * sizeof(double));
        mat_b = mat_a + mat_dim * mat_dim;
        mat_c = mat_b + mat_dim * mat_dim;
        mat_tmp_c = NULL;
        memset(mat_c, 0, mat_dim * mat_dim * sizeof(double));
    }

    /* allocate local buffer */
    local_a = (double *) malloc(3 * BLK_DIM * BLK_DIM * sizeof(double));
    local_b = local_a + BLK_DIM * BLK_DIM;
    local_c = local_b + BLK_DIM * BLK_DIM;

    MPI_Barrier(MPI_COMM_WORLD);

    t1 = MPI_Wtime();

    work_id_len = blk_num * blk_num;    /* total number of work units */

    /* copy entire matrices to workers */
    if (!rank) {
        /* send A and B to workers */
        int i;
        for (i = 1; i < nprocs; i++) {
            MPI_Send(mat_a, mat_dim * mat_dim, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            MPI_Send(mat_b, mat_dim * mat_dim, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
    } else {
        /* receive A and B from master */
        MPI_Recv(mat_a, mat_dim * mat_dim, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(mat_b, mat_dim * mat_dim, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /* compute Cij += Aik * Bkj */
        for (work_id = rank - 1; work_id < work_id_len; work_id += nprocs - 1) {
            int global_i = work_id / blk_num;
            int global_k = work_id % blk_num;
            int global_j;

            /* copy block from mat_a */
            pack_global_to_local(local_a, mat_a, mat_dim, global_i, global_k);
            if (is_zero_local(local_a))
                continue;

            for (global_j = 0; global_j < blk_num; global_j++) {
                /* copy block from mat_b */
                pack_global_to_local(local_b, mat_b, mat_dim, global_k, global_j);
                if (is_zero_local(local_b))
                    continue;

                /* compute Cij += Aik * Bkj only if both local_a and local_b are nonzero */
                dgemm(local_a, local_b, local_c);

                /* write results to mat_c */
                add_local_to_global(mat_c, local_c, mat_dim, global_i, global_j);
            }
        }
    }

    /* accumulate results */
    if (!rank) {
        int i, j, k;
        /* sum C computed by workers */
        for (i = 1; i < nprocs; i++) {
            MPI_Recv(mat_tmp_c, mat_dim * mat_dim, MPI_DOUBLE, i, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
            for (j = 0; j < mat_dim; j++) {
                for (k = 0; k < mat_dim; k++)
                    mat_c[j + k * mat_dim] += mat_tmp_c[j + k * mat_dim];
            }
        }
    } else {
        /* send C to master */
        MPI_Send(mat_c, mat_dim * mat_dim, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    t2 = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);

    if (!rank) {
        check_mats(mat_a, mat_b, mat_c, mat_dim);
        printf("[%i] time: %f\n", rank, t2 - t1);
    }

    free(local_a);
    free(mat_a);

    MPI_Finalize();
    return 0;
}

int is_zero_local(double *local_mat)
{
    int i, j;

    for (i = 0; i < BLK_DIM; i++) {
        for (j = 0; j < BLK_DIM; j++) {
            if (local_mat[j + i * BLK_DIM] != 0.0)
                return 0;
        }
    }
    return 1;
}

void dgemm(double *local_a, double *local_b, double *local_c)
{
    int i, j, k;

    memset(local_c, 0, BLK_DIM * BLK_DIM * sizeof(double));

    for (j = 0; j < BLK_DIM; j++) {
        for (i = 0; i < BLK_DIM; i++) {
            for (k = 0; k < BLK_DIM; k++)
                local_c[j + i * BLK_DIM] += local_a[k + i * BLK_DIM] * local_b[j + k * BLK_DIM];
        }
    }
}

void pack_global_to_local(double *local_mat, double *global_mat, int mat_dim, int global_i,
                          int global_j)
{
    int i, j;
    int offset = global_i * BLK_DIM * mat_dim + global_j * BLK_DIM;

    for (i = 0; i < BLK_DIM; i++) {
        for (j = 0; j < BLK_DIM; j++)
            local_mat[j + i * BLK_DIM] = global_mat[offset + j + i * mat_dim];
    }
}

void unpack_local_to_global(double *global_mat, double *local_mat, int mat_dim, int global_i,
                            int global_j)
{
    int i, j;
    int offset = global_i * BLK_DIM * mat_dim + global_j * BLK_DIM;

    for (i = 0; i < BLK_DIM; i++) {
        for (j = 0; j < BLK_DIM; j++)
            global_mat[offset + j + i * mat_dim] = local_mat[j + i * BLK_DIM];
    }
}

void add_local_to_global(double *global_mat, double *local_mat, int mat_dim, int global_i,
                         int global_j)
{
    int i, j;
    int offset = global_i * BLK_DIM * mat_dim + global_j * BLK_DIM;

    for (i = 0; i < BLK_DIM; i++) {
        for (j = 0; j < BLK_DIM; j++)
            global_mat[offset + j + i * mat_dim] += local_mat[j + i * BLK_DIM];
    }
}
