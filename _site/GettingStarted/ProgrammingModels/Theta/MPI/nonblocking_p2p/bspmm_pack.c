/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

#include "bspmm.h"

/*
 * Block sparse matrix multiplication using non-blocking send/receive and manual packing.
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
 * The master process distributes each work unit (submatrices of A and B) to
 * worker processes and sums up the corresponding C submatrices that are received
 * from workers. The workers overlap computation of current submatrix with
 * communication of next one. The submatrix is noncontiguous and is packed into
 * a contiguous buffer for communication.
 */

/* translate worker index to rank */
#define worker_rank(w) (w+1)

int is_zero_local(double *local_mat);

void dgemm(double *local_a, double *local_b, double *local_c);

void pack_global_to_local(double *local_mat, double *global_mat, int mat_dim, int global_i,
                          int global_j);

void add_local_to_global(double *global_mat, double *local_mat, int mat_dim, int global_i,
                         int global_j);

int main(int argc, char **argv)
{
    int rank, nprocs;
    int mat_dim, blk_num;
    int work_id, work_id_len;
    double *mat_a, *mat_b, *mat_c;
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
        mat_a = (double *) malloc(3 * mat_dim * mat_dim * sizeof(double));
        mat_b = mat_a + mat_dim * mat_dim;
        mat_c = mat_b + mat_dim * mat_dim;
        init_mats(mat_dim, mat_a, mat_b, mat_c);
    }

    /* allocate local buffer */
    local_a = (double *) malloc(3 * BLK_DIM * BLK_DIM * sizeof(double));
    local_b = local_a + BLK_DIM * BLK_DIM;
    local_c = local_b + BLK_DIM * BLK_DIM;

    MPI_Barrier(MPI_COMM_WORLD);

    t1 = MPI_Wtime();

    work_id_len = blk_num * blk_num;    /* total number of work units */

    if (!rank) {
        /* distribute A and B and receive results. */
        int work_start_id = 0;

        /* iterate through all work units */
        while (work_start_id < work_id_len) {
            int worker, nworkers;
            int global_j;

            /* not all workers work in the last iteration since # of work units is not uniform */
            nworkers = (work_id_len - work_start_id) < nprocs - 1 ?
                work_id_len - work_start_id : nprocs - 1;

            /* send blocks of A to all workers */
            for (worker = 0; worker < nworkers; worker++) {
                int work_id = work_start_id + worker;
                int global_i = work_id / blk_num;
                int global_k = work_id % blk_num;

                pack_global_to_local(local_a, mat_a, mat_dim, global_i, global_k);
                MPI_Send(local_a, BLK_DIM * BLK_DIM, MPI_DOUBLE, worker_rank(worker), 0,
                         MPI_COMM_WORLD);
            }

            /* send blocks of B and receive results */
            for (global_j = 0; global_j < blk_num; global_j++) {
                /* send one block of B to each worker */
                for (worker = 0; worker < nworkers; worker++) {
                    int work_id = work_start_id + worker;
                    int global_k = work_id % blk_num;

                    pack_global_to_local(local_b, mat_b, mat_dim, global_k, global_j);
                    MPI_Send(local_b, BLK_DIM * BLK_DIM, MPI_DOUBLE, worker_rank(worker), 0,
                             MPI_COMM_WORLD);
                }

                /* workers are expected to proceed computation while master is communicating with
                 * other workers */

                /* receive results from all workers */
                for (worker = 0; worker < nworkers; worker++) {
                    int work_id = work_start_id + worker;
                    int global_i = work_id / blk_num;

                    MPI_Recv(local_c, BLK_DIM * BLK_DIM, MPI_DOUBLE, worker_rank(worker), 0,
                             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    add_local_to_global(mat_c, local_c, mat_dim, global_i, global_j);
                }
            }
            work_start_id += nworkers;
        }
    } else {
        /* receive A and B from master and return result */

        /* requests to receive prefetched blocks */
        MPI_Request recv_a_req = MPI_REQUEST_NULL, pf_recv_a_req = MPI_REQUEST_NULL;
        MPI_Request *pf_b_reqs, *recv_b_reqs, *pf_recv_b_reqs;

        pf_b_reqs = (MPI_Request *) malloc(sizeof(MPI_Request) * blk_num * 2);
        recv_b_reqs = pf_b_reqs;
        pf_recv_b_reqs = &pf_b_reqs[blk_num];

        /* buffers to keep prefetched blocks */
        double *pf_mem, *pf_local_a, *tmp_local_a, *original_local_a = local_a;
        double *local_bs, *pf_local_bs, *tmp_local_bs;

        pf_mem = (double *) malloc((2 * blk_num + 1) * BLK_DIM * BLK_DIM * sizeof(double));
        local_bs = pf_mem;
        pf_local_bs = local_bs + blk_num * BLK_DIM * BLK_DIM;
        pf_local_a = pf_local_bs + blk_num * BLK_DIM * BLK_DIM;

        int first_work_id = rank - 1;
        int i;

        /* prefetch blocks of A and B that are used in the first iteration */
        if (first_work_id < work_id_len) {
            MPI_Irecv(pf_local_a, BLK_DIM * BLK_DIM, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,
                      &pf_recv_a_req);
            for (i = 0; i < blk_num; i++)
                MPI_Irecv(&pf_local_bs[i * BLK_DIM * BLK_DIM], BLK_DIM * BLK_DIM, MPI_DOUBLE, 0, 0,
                          MPI_COMM_WORLD, &pf_recv_b_reqs[i]);
        }

        for (work_id = first_work_id; work_id < work_id_len; work_id += nprocs - 1) {
            /* swap working local buffers and requests */
            recv_a_req = pf_recv_a_req;
            memcpy(recv_b_reqs, pf_recv_b_reqs, sizeof(MPI_Request) * blk_num);
            tmp_local_a = local_a;
            local_a = pf_local_a;
            pf_local_a = tmp_local_a;
            tmp_local_bs = local_bs;
            local_bs = pf_local_bs;
            pf_local_bs = tmp_local_bs;

            /* prefetch blocks of A and B that will be used in the next iteration */
            if (work_id + nprocs - 1 < work_id_len) {
                MPI_Irecv(pf_local_a, BLK_DIM * BLK_DIM, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,
                          &pf_recv_a_req);
                for (i = 0; i < blk_num; i++)
                    MPI_Irecv(&pf_local_bs[i * BLK_DIM * BLK_DIM], BLK_DIM * BLK_DIM, MPI_DOUBLE, 0,
                              0, MPI_COMM_WORLD, &pf_recv_b_reqs[i]);
            }

            /* wait prefetched blocks */
            MPI_Wait(&recv_a_req, MPI_STATUS_IGNORE);

            for (i = 0; i < blk_num; i++) {
                MPI_Wait(&recv_b_reqs[i], MPI_STATUS_IGNORE);

                /* compute Cij += Aik * Bkj only if both local_a and local_b are nonzero */
                if (is_zero_local(local_a) || is_zero_local(&local_bs[i * BLK_DIM * BLK_DIM])) {
                    memset(local_c, 0, BLK_DIM * BLK_DIM * sizeof(double));
                } else {
                    dgemm(local_a, &local_bs[i * BLK_DIM * BLK_DIM], local_c);
                }

                /* send C */
                MPI_Send(local_c, BLK_DIM * BLK_DIM, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
            }
        }

        free(pf_b_reqs);
        free(pf_mem);
        local_a = original_local_a;
    }

    t2 = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);

    if (!rank) {
        check_mats(mat_a, mat_b, mat_c, mat_dim);
        printf("[%i] time: %f\n", rank, t2 - t1);
    }

    free(local_a);
    if (!rank) {
        free(mat_a);
    }

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
