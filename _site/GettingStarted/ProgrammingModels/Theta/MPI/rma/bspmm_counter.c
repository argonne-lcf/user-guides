/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

#include "bspmm.h"

/*
 * Block sparse matrix multiplication using RMA operations and a global counter for workload
 * distribution.
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
 * The master process allocates entire matrices A, B and C. The worker processes get
 * the submatrices from master using RMA operations. The distribution of work between the
 * workers is dynamic: each worker reads a counter to obtain its work id. The counter is
 * updated atomically each time it is read. Each worker atomically updates the value of
 * the submatrix of C that it computes.
 */

int is_zero_local(double *local_mat);

void dgemm(double *local_a, double *local_b, double *local_c);

int main(int argc, char **argv)
{
    int rank, nprocs;
    int mat_dim, blk_num;
    int work_id, work_id_len;
    double *mat_a, *mat_b, *mat_c;
    double *local_a, *local_b, *local_c;
    MPI_Aint disp_a, disp_b, disp_c;
    MPI_Aint offset_a, offset_b, offset_c;

    double *win_mem;
    int *counter_win_mem;
    MPI_Win win, win_counter;

    const int one = 1;

    double t1, t2;

    MPI_Datatype blk_dtp;

    /* initialize MPI environment */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    /* argument checking and setting */
    if (setup(rank, nprocs, argc, argv, &mat_dim)) {
        MPI_Finalize();
        exit(0);
    }

    /* number of blocks in one dimension */
    blk_num = mat_dim / BLK_DIM;

    if (!rank) {
        /* create RMA windows */
        MPI_Win_allocate(3 * mat_dim * mat_dim * sizeof(double), sizeof(double),
                         MPI_INFO_NULL, MPI_COMM_WORLD, &win_mem, &win);
        MPI_Win_allocate(sizeof(int), sizeof(int),
                         MPI_INFO_NULL, MPI_COMM_WORLD, &counter_win_mem, &win_counter);
        mat_a = win_mem;
        mat_b = mat_a + mat_dim * mat_dim;
        mat_c = mat_b + mat_dim * mat_dim;

        /* initialize matrices and global counter */
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
        init_mats(mat_dim, mat_a, mat_b, mat_c);
        MPI_Win_unlock(0, win); /* MEM_MODE: update to my private window becomes
                                 * visible in public window */

        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win_counter);
        *counter_win_mem = 0;
        MPI_Win_unlock(0, win_counter); /* MEM_MODE: update to my private window becomes
                                         * visible in public window */
    } else {
        MPI_Win_allocate(0, sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &win_mem, &win);
        MPI_Win_allocate(0, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &counter_win_mem,
                         &win_counter);
    }

    /* allocate local buffer */
    local_a = (double *) malloc(3 * BLK_DIM * BLK_DIM * sizeof(double));
    local_b = local_a + BLK_DIM * BLK_DIM;
    local_c = local_b + BLK_DIM * BLK_DIM;

    /* create block datatype */
    int array_sizes[2] = { mat_dim, mat_dim };
    int array_subsizes[2] = { BLK_DIM, BLK_DIM };
    int array_starts[2] = { 0, 0 };
    MPI_Type_create_subarray(2, array_sizes, array_subsizes, array_starts,
                             MPI_ORDER_C, MPI_DOUBLE, &blk_dtp);
    MPI_Type_commit(&blk_dtp);

    disp_a = 0;
    disp_b = disp_a + mat_dim * mat_dim;
    disp_c = disp_b + mat_dim * mat_dim;

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);
    MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win_counter);

    work_id_len = blk_num * blk_num;

    t1 = MPI_Wtime();

    do {
        /* read and increment global counter atomically */
        MPI_Fetch_and_op(&one, &work_id, MPI_INT, 0, 0, MPI_SUM, win_counter);
        MPI_Win_flush(0, win_counter);  /* MEM_MODE: update to target public window */
        if (work_id >= work_id_len)
            break;

        /* calculate global ids from the work_id */
        int global_i = work_id / blk_num;
        int global_k = work_id % blk_num;
        int global_j;

        /* get block from mat_a */
        offset_a = global_i * BLK_DIM * mat_dim + global_k * BLK_DIM;
        MPI_Get(local_a, BLK_DIM * BLK_DIM, MPI_DOUBLE, 0, disp_a + offset_a, 1, blk_dtp, win);
        MPI_Win_flush(0, win);  /* MEM_MODE: get from target public window */

        if (is_zero_local(local_a))
            continue;

        for (global_j = 0; global_j < blk_num; global_j++) {
            /* get block from mat_b */
            offset_b = global_k * BLK_DIM * mat_dim + global_j * BLK_DIM;
            MPI_Get(local_b, BLK_DIM * BLK_DIM, MPI_DOUBLE, 0, disp_b + offset_b, 1, blk_dtp, win);
            MPI_Win_flush(0, win);      /* MEM_MODE: get from target public window */

            if (is_zero_local(local_b))
                continue;

            /* compute Cij += Aik * Bkj only if both local_a and local_b are nonzero */
            dgemm(local_a, local_b, local_c);

            /* accumulate block to mat_c */
            offset_c = global_i * BLK_DIM * mat_dim + global_j * BLK_DIM;
            MPI_Accumulate(local_c, BLK_DIM * BLK_DIM, MPI_DOUBLE, 0, disp_c + offset_c, 1, blk_dtp,
                           MPI_SUM, win);
            MPI_Win_flush(0, win);      /* MEM_MODE: update to target public window */
        }
    } while (work_id < work_id_len);

    MPI_Barrier(MPI_COMM_WORLD);
    t2 = MPI_Wtime();

    if (!rank) {
        MPI_Win_sync(win);      /* MEM_MODE: synchronize private and public window copies */
        check_mats(mat_a, mat_b, mat_c, mat_dim);
        printf("[%i] time: %f\n", rank, t2 - t1);
    }

    MPI_Win_unlock(0, win_counter);
    MPI_Win_unlock(0, win);

    MPI_Type_free(&blk_dtp);
    free(local_a);
    MPI_Win_free(&win_counter);
    MPI_Win_free(&win);

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
