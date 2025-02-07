/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

#include "bspmm.h"

/*
 * Block sparse matrix multiplication using shared memory and a global counter for workload
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
 * This version of bspmm parallelizes i and j to eliminate atomic updates to C;
 * there are n^2 parallel computations of Cij += Aik * Bkj
 * Work id (0 <= id < n^2) is associated with each computation as follows
 *   (i, j) = (id / n, id % n)
 *
 * The master process allocates entire matrices A, B, and C. The worker processes
 * read submatrices of A and B directly from master's memory to calculate distinct
 * submatrices of C. Each worker stores the value of the submatrix of C that it
 * computes back into master's memory. The global counter is used for dynamic workload
 * distribution.
 */

int is_zero_block(double *mat, int offset, int mat_dim);

void dgemm_increment_c(double *mat_a, double *mat_b, double *mat_c, int offset_a,
                       int offset_b, int offset_c, int mat_dim);

int main(int argc, char **argv)
{
    int rank, nprocs;
    int mat_dim, blk_num;
    int work_id, work_id_len;
    double *mat_a, *mat_b, *mat_c;
    double *mat_a_ptr, *mat_b_ptr, *mat_c_ptr;

    double *win_mem;
    int *counter_win_mem;
    MPI_Win win, win_counter;

    const int one = 1;

    double t1, t2;

    /* initialize MPI environment */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    /* create shared memory communicator */
    MPI_Comm shm_comm;
    MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &shm_comm);

    int shm_rank, shm_procs;
    MPI_Comm_size(shm_comm, &shm_procs);
    MPI_Comm_rank(shm_comm, &shm_rank);

    /* works only when all processes are in the same shared memory region */
    if (shm_procs != nprocs)
        MPI_Abort(MPI_COMM_WORLD, 1);

    /* argument checking and setting */
    if (setup(rank, nprocs, argc, argv, &mat_dim)) {
        MPI_Finalize();
        exit(0);
    }

    /* number of blocks in one dimension */
    blk_num = mat_dim / BLK_DIM;

    if (!rank) {
        /* create RMA windows */
        MPI_Win_allocate_shared(3 * mat_dim * mat_dim * sizeof(double), sizeof(double),
                                MPI_INFO_NULL, shm_comm, &win_mem, &win);
        MPI_Win_allocate_shared(sizeof(int), sizeof(int),
                                MPI_INFO_NULL, shm_comm, &counter_win_mem, &win_counter);
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
        MPI_Win_allocate_shared(0, sizeof(double), MPI_INFO_NULL, shm_comm, &win_mem, &win);
        MPI_Win_allocate_shared(0, sizeof(int), MPI_INFO_NULL, shm_comm, &counter_win_mem,
                                &win_counter);
    }

    /* check whether the windows are unified memory model */
    int *mem_model = NULL, *mem_model_counter = NULL, flag = 0;
    MPI_Win_get_attr(win, MPI_WIN_MODEL, &mem_model, &flag);
    MPI_Win_get_attr(win, MPI_WIN_MODEL, &mem_model_counter, &flag);
    if (*mem_model != MPI_WIN_UNIFIED || *mem_model_counter != MPI_WIN_UNIFIED) {
        fprintf(stderr, "MPI supports accessing shared memory windows only in "
                "UNIFIED memory model.\n");
        fflush(stderr);
        MPI_Abort(MPI_COMM_WORLD, 1);   /* abort if memory model is unsupported */
    }

    /* acquire rank-0's pointer to all the three matrices */
    MPI_Aint win_sz;
    int disp_unit;
    MPI_Win_shared_query(win, 0, &win_sz, &disp_unit, &mat_a_ptr);
    mat_b_ptr = mat_a_ptr + mat_dim * mat_dim;
    mat_c_ptr = mat_b_ptr + mat_dim * mat_dim;

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
        int global_j = work_id % blk_num;
        int global_k;

        int offset_a, offset_b, offset_c;

        for (global_k = 0; global_k < blk_num; global_k++) {
            /* compute offsets into matrices A, B, and C */
            offset_a = global_i * BLK_DIM * mat_dim + global_k * BLK_DIM;
            offset_b = global_k * BLK_DIM * mat_dim + global_j * BLK_DIM;
            offset_c = global_i * BLK_DIM * mat_dim + global_j * BLK_DIM;

            if (is_zero_block(mat_a_ptr, offset_a, mat_dim))
                continue;

            if (is_zero_block(mat_b_ptr, offset_b, mat_dim))
                continue;

            /* compute Cij += Aik * Bkj only if both blocks of A and B are nonzero */
            dgemm_increment_c(mat_a_ptr, mat_b_ptr, mat_c_ptr, offset_a, offset_b, offset_c,
                              mat_dim);
        }
    } while (work_id < work_id_len);

    /* sync here instead of right-after-store since each rank is updating distinct C-blocks and is not dependent on other C-blocks */
    MPI_Win_sync(win);  /* ensure completion of local updates before MPI_Barrier */
    MPI_Barrier(MPI_COMM_WORLD);
    t2 = MPI_Wtime();

    if (!rank) {
        MPI_Win_sync(win);      /* ensure remote updates are locally visible */
        check_mats(mat_a, mat_b, mat_c, mat_dim);
        printf("[%i] time: %f\n", rank, t2 - t1);
    }

    MPI_Win_unlock(0, win_counter);
    MPI_Win_unlock(0, win);

    MPI_Win_free(&win_counter);
    MPI_Win_free(&win);
    MPI_Comm_free(&shm_comm);

    MPI_Finalize();
    return 0;
}

int is_zero_block(double *mat, int offset, int mat_dim)
{
    int i, j;

    for (i = 0; i < BLK_DIM; i++) {
        for (j = 0; j < BLK_DIM; j++) {
            if (mat[offset + j + i * mat_dim] != 0.0)
                return 0;
        }
    }

    return 1;
}

void dgemm_increment_c(double *mat_a, double *mat_b, double *mat_c, int offset_a,
                       int offset_b, int offset_c, int mat_dim)
{
    int i, j, k;

    for (i = 0; i < BLK_DIM; i++) {
        for (j = 0; j < BLK_DIM; j++) {
            for (k = 0; k < BLK_DIM; k++)
                mat_c[offset_c + j + i * mat_dim] +=
                    mat_a[offset_a + k + i * mat_dim] * mat_b[offset_b + j + k * mat_dim];
        }
    }
}
