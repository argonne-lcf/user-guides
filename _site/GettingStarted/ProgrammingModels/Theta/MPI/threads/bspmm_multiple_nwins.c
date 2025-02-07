/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

#include <omp.h>
#include "bspmm.h"

/*
 * Block sparse matrix multiplication using RMA operations, a global counter for workload
 * distribution, MPI_THREAD_MULTIPLE mode, and a separate window for each thread.
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
 * the submatrices from master using RMA operations. Each worker spawns a team of threads
 * with each thread working on a different submatrix. The distribution of work between
 * the threads of all the workers is dynamic: each thread reads a counter to obtain its
 * work id. The counter is updated atomically each time it is read. Each thread updates
 * the submatrix of C that it is working on atomically. Each thread also has its own RMA
 * window of the same memory region of matrices A and B; this separates the flushes of RMA
 * read operations of different threads.
 */

int is_zero_local(double *local_mat);

void dgemm(double *local_a, double *local_b, double *local_c);

int main(int argc, char **argv)
{
    int rank, nprocs, provided;
    int tid, num_threads;
    int mat_dim, blk_num;
    int work_id, work_id_len;
    double *mats_ab, *mat_a, *mat_b, *mat_c;
    double *local_a, *local_b, *local_c;
    MPI_Aint disp_a, disp_b, disp_c;
    MPI_Aint offset_a, offset_b, offset_c;

    double *win_c_mem;
    int *counter_win_mem;
    MPI_Win *wins_ab, win_c, win_counter;

    const int one = 1;

    double t1, t2;

    MPI_Datatype blk_dtp;

    /* initialize MPI environment */
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (provided < MPI_THREAD_MULTIPLE)
        MPI_Abort(MPI_COMM_WORLD, 1);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    num_threads = omp_get_max_threads();

    /* argument checking and setting */
    if (setup(rank, nprocs, argc, argv, &mat_dim)) {
        MPI_Finalize();
        exit(0);
    }

    /* number of blocks in one dimension */
    blk_num = mat_dim / BLK_DIM;

    /* allocate RMA windows for matrices a and b; separate window for each thread */
    wins_ab = (MPI_Win *) malloc(sizeof(MPI_Win) * num_threads);

    if (!rank) {
        /* allocate RMA memory for matrices a and b */
        mats_ab = (double *) malloc(2 * mat_dim * mat_dim * sizeof(double));
        /* allocate and create RMA windows for matrix C and global counter */
        MPI_Win_allocate(mat_dim * mat_dim * sizeof(double), sizeof(double),
                         MPI_INFO_NULL, MPI_COMM_WORLD, &win_c_mem, &win_c);
        MPI_Win_allocate(sizeof(int), sizeof(int),
                         MPI_INFO_NULL, MPI_COMM_WORLD, &counter_win_mem, &win_counter);

        mat_a = mats_ab;
        mat_b = mat_a + mat_dim * mat_dim;
        mat_c = win_c_mem;

        /* initialize matrices */
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win_c);
        init_mats(mat_dim, mat_a, mat_b, mat_c);
        MPI_Win_unlock(0, win_c);       /* MEM_MODE: update to my private window becomes
                                         * visible in public window */

        /* create RMA windows for matrices A and B */
        for (tid = 0; tid < num_threads; tid++)
            MPI_Win_create(mats_ab, 2 * mat_dim * mat_dim * sizeof(double), sizeof(double),
                           MPI_INFO_NULL, MPI_COMM_WORLD, &wins_ab[tid]);

        /* initialize global counter */
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win_counter);
        *counter_win_mem = 0;
        MPI_Win_unlock(0, win_counter); /* MEM_MODE: update to my private window becomes
                                         * visible in public window */
    } else {
        MPI_Win_allocate(0, sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &win_c_mem, &win_c);
        MPI_Win_allocate(0, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &counter_win_mem,
                         &win_counter);
        for (tid = 0; tid < num_threads; tid++)
            MPI_Win_create(NULL, 0, sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &wins_ab[tid]);
    }

    /* allocate local buffer for each thread */
    local_a = (double *) malloc(num_threads * 3 * BLK_DIM * BLK_DIM * sizeof(double));
    local_b = local_a + BLK_DIM * BLK_DIM * num_threads;
    local_c = local_b + BLK_DIM * BLK_DIM * num_threads;

    /* create block datatype */
    int array_sizes[2] = { mat_dim, mat_dim };
    int array_subsizes[2] = { BLK_DIM, BLK_DIM };
    int array_starts[2] = { 0, 0 };
    MPI_Type_create_subarray(2, array_sizes, array_subsizes, array_starts,
                             MPI_ORDER_C, MPI_DOUBLE, &blk_dtp);
    MPI_Type_commit(&blk_dtp);

    disp_a = 0;
    disp_b = disp_a + mat_dim * mat_dim;
    disp_c = 0;

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win_c);
    MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win_counter);
    for (tid = 0; tid < num_threads; tid++)
        MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, wins_ab[tid]);

    work_id_len = blk_num * blk_num;

    t1 = MPI_Wtime();
    /* create a team of threads with each thread working on a different pair of blocks */
#pragma omp parallel private(tid, work_id, offset_a, offset_b, offset_c) firstprivate(local_a, local_b, local_c)
    {
        tid = omp_get_thread_num();
        double *local_ta = local_a + BLK_DIM * BLK_DIM * tid;
        double *local_tb = local_b + BLK_DIM * BLK_DIM * tid;
        double *local_tc = local_c + BLK_DIM * BLK_DIM * tid;

        do {
            /* read and increment global counter atomically */
            MPI_Fetch_and_op(&one, &work_id, MPI_INT, 0, 0, MPI_SUM, win_counter);
            MPI_Win_flush(0, win_counter);      /* MEM_MODE: update to target public window */
            if (work_id >= work_id_len)
                break;

            /* calculate global ids from the work_id */
            int global_i = work_id / blk_num;
            int global_k = work_id % blk_num;
            int global_j;

            /* get block from mat_a */
            offset_a = global_i * BLK_DIM * mat_dim + global_k * BLK_DIM;
            MPI_Get(local_ta, BLK_DIM * BLK_DIM, MPI_DOUBLE, 0, disp_a + offset_a, 1, blk_dtp,
                    wins_ab[tid]);
            MPI_Win_flush(0, wins_ab[tid]);     /* MEM_MODE: get from target public window */

            if (is_zero_local(local_ta))
                continue;

            for (global_j = 0; global_j < blk_num; global_j++) {
                /* get block from mat_b */
                offset_b = global_k * BLK_DIM * mat_dim + global_j * BLK_DIM;
                MPI_Get(local_tb, BLK_DIM * BLK_DIM, MPI_DOUBLE, 0, disp_b + offset_b, 1, blk_dtp,
                        wins_ab[tid]);
                MPI_Win_flush(0, wins_ab[tid]); /* MEM_MODE: get from target public window */

                if (is_zero_local(local_tb))
                    continue;

                /* compute Cij += Aik * Bkj using threads only if both local_ta and local_tb are nonzero */
                dgemm(local_ta, local_tb, local_tc);

                /* accumulate block to mat_c */
                offset_c = global_i * BLK_DIM * mat_dim + global_j * BLK_DIM;
                MPI_Accumulate(local_tc, BLK_DIM * BLK_DIM, MPI_DOUBLE, 0, disp_c + offset_c, 1,
                               blk_dtp, MPI_SUM, win_c);
                MPI_Win_flush(0, win_c);        /* MEM_MODE: update to target public window */
            }
        } while (work_id < work_id_len);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t2 = MPI_Wtime();

    if (!rank) {
        MPI_Win_sync(win_c);    /* MEM_MODE: synchronize private and public window copies */
        check_mats(mat_a, mat_b, mat_c, mat_dim);
        int nthreads = omp_get_max_threads();
#if !defined(OUTPUT_TOFILE)
        printf("m,nthreads,time\n");
        printf("%d,%d,%f\n", mat_dim, nthreads, t2 - t1);
#else
        char filename[50];
        sprintf(filename, "bspmm_multiple_nwins_%d_%d", nprocs, mat_dim);
        FILE *out = fopen(filename, "w");
        fprintf(out, "m,nthreads,time\n");
        fprintf(out, "%d,%d,%f\n", mat_dim, nthreads, t2 - t1);
        fclose(out);
#endif
    }

    MPI_Win_unlock(0, win_counter);
    MPI_Win_unlock(0, win_c);
    for (tid = 0; tid < num_threads; tid++)
        MPI_Win_unlock(0, wins_ab[tid]);

    MPI_Type_free(&blk_dtp);
    free(local_a);
    if (!rank)
        free(mats_ab);
    MPI_Win_free(&win_counter);
    MPI_Win_free(&win_c);
    for (tid = 0; tid < num_threads; tid++)
        MPI_Win_free(&wins_ab[tid]);
    free(wins_ab);

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
