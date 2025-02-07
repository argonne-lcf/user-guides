/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

/*
 * 2D stencil code parallelized by multiple threads with MPI_THREAD_MULTIPLE.
 *
 * 2D regular grid is divided into px * py blocks of grid points (px * py = # of processes.)
 * The computation over x-axis is multithreaded. In every iteration, each thread calls nonblocking
 * operations with derived data types to exchange grid points in a halo region with corresponding
 * threads of neighbor nodes.
 */

#include "stencil_par.h"
#include <omp.h>

/* row-major order */
#define ind(i,j) ((j)*(bx+2)+(i))

int ind_f(int i, int j, int bx)
{
    return ind(i, j);
}

#define THX_START (thread_id % nthreads)==0 ? 1 : (thread_id % nthreads) * Thx + 1
#define THX_END (thread_id % nthreads) == nthreads -1 ? bx + 1 : ((thread_id + 1) % nthreads) * Thx + 1

void setup(int rank, int proc, int argc, char **argv,
           int *n_ptr, int *energy_ptr, int *niters_ptr, int *px_ptr, int *py_ptr, int *final_flag);

void init_sources(int bx, int by, int offx, int offy, int n,
                  const int nsources, int sources[][2], int *locnsources_ptr, int locsources[][2]);

int main(int argc, char **argv)
{
    int rank, size, provided;
    int n, energy, niters, px, py;

    int rx, ry;
    int north, south, west, east;
    int bx, by, offx, offy;

    /* three heat sources */
    const int nsources = 3;
    int sources[nsources][2];
    int locnsources;            /* number of sources in my area */
    int locsources[nsources][2];        /* sources local to my rank */

    double t1, t2;

    int iter, i, j;

    double *aold, *anew, *tmp;

    double heat, rheat;

    int final_flag;

    int nthreads, Thx;

    /* initialize MPI envrionment */
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (provided < MPI_THREAD_MULTIPLE)
        MPI_Abort(MPI_COMM_WORLD, 1);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* argument checking and setting */
    setup(rank, size, argc, argv, &n, &energy, &niters, &px, &py, &final_flag);

    if (final_flag == 1) {
        MPI_Finalize();
        exit(0);
    }

    /* determine my coordinates (x,y) -- rank=x*a+y in the 2d processor array */
    rx = rank % px;
    ry = rank / px;

    /* determine my four neighbors */
    north = (ry - 1) * px + rx;
    if (ry - 1 < 0)
        north = MPI_PROC_NULL;
    south = (ry + 1) * px + rx;
    if (ry + 1 >= py)
        south = MPI_PROC_NULL;
    west = ry * px + rx - 1;
    if (rx - 1 < 0)
        west = MPI_PROC_NULL;
    east = ry * px + rx + 1;
    if (rx + 1 >= px)
        east = MPI_PROC_NULL;

    /* decompose the domain */
    bx = n / px;        /* block size in x */
    by = n / py;        /* block size in y */
    offx = rx * bx;     /* offset in x */
    offy = ry * by;     /* offset in y */


    /* divide blocks in x amongst threads */
    nthreads = omp_get_max_threads();
    Thx = bx / nthreads;

    if (Thx == 0) {
        if (rank == 0) {
            fprintf(stderr, "Domain size too small for number of threads\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /* allocate working arrays & communication buffers */
    aold = (double *) malloc((bx + 2) * (by + 2) * sizeof(double));     /* 1-wide halo zones! */
    anew = (double *) malloc((bx + 2) * (by + 2) * sizeof(double));     /* 1-wide halo zones! */

    /* initialize three heat sources */
    init_sources(bx, by, offx, offy, n, nsources, sources, &locnsources, locsources);

    /* create east-west datatype */
    MPI_Datatype east_west_type;
    MPI_Type_vector(by, 1, bx + 2, MPI_DOUBLE, &east_west_type);
    MPI_Type_commit(&east_west_type);

    t1 = MPI_Wtime();   /* take time */

#pragma omp parallel private(iter, i,j)
    {
        int thread_id = omp_get_thread_num();
        int xstart = THX_START;
        int xend = THX_END;
        int xrange = xend - xstart;

        /* Initialize aold and anew using "first touch", *including boundaries* */
        /* Note that MPI_Alloc_mem does not initialize the memory
         * (this is a good thing in this case) */
        /* temporarily update xstart and xend for the initialization
         * and reset them once we are done */
        if (xstart == 1)
            xstart = 0;
        if (xend == bx + 1)
            xend = bx + 2;
        for (j = 0; j <= by + 1; ++j) {
            for (i = xstart; i < xend; ++i) {
                aold[ind(i, j)] = 0.0;
                anew[ind(i, j)] = 0.0;
            }
        }
        xstart = THX_START;
        xend = THX_END;

        for (iter = 0; iter < niters; ++iter) {
#pragma omp master
            {
                /* refresh heat sources */
                for (i = 0; i < locnsources; ++i) {
                    aold[ind(locsources[i][0], locsources[i][1])] += energy;    /* heat source */
                }

                /* reset the total heat */
                heat = 0.0;
            }
#pragma omp barrier

            /* create request arrays */
            MPI_Request reqs[8];

            /* exchange data with neighbors */

            /* each thread uses thread_id as a tag for north-south
             * exchange.  use nthreads as a tag for west-to-east
             * communication and nthreads+1 for east-to-west because
             * the two sides are assigned to threads that have
             * different ids */
            MPI_Isend(&aold[ind(xstart, 1)] /* north */ , xrange, MPI_DOUBLE,
                      north, thread_id, MPI_COMM_WORLD, &reqs[0]);
            MPI_Isend(&aold[ind(xstart, by)] /* south */ , xrange, MPI_DOUBLE,
                      south, thread_id, MPI_COMM_WORLD, &reqs[1]);
            MPI_Isend(&aold[ind(bx, 1)] /* east */ , 1, east_west_type,
                      east, nthreads, MPI_COMM_WORLD, &reqs[2]);
            MPI_Isend(&aold[ind(1, 1)] /* west */ , 1, east_west_type,
                      west, nthreads + 1, MPI_COMM_WORLD, &reqs[3]);
            MPI_Irecv(&aold[ind(xstart, 0)] /* north */ , xrange, MPI_DOUBLE,
                      north, thread_id, MPI_COMM_WORLD, &reqs[4]);
            MPI_Irecv(&aold[ind(xstart, by + 1)] /* south */ , xrange, MPI_DOUBLE,
                      south, thread_id, MPI_COMM_WORLD, &reqs[5]);
            MPI_Irecv(&aold[ind(bx + 1, 1)] /* west */ , 1, east_west_type,
                      east, nthreads + 1, MPI_COMM_WORLD, &reqs[6]);
            MPI_Irecv(&aold[ind(0, 1)] /* east */ , 1, east_west_type,
                      west, nthreads, MPI_COMM_WORLD, &reqs[7]);
            MPI_Waitall(8, reqs, MPI_STATUSES_IGNORE);

            /* update grid */
            double my_heat = 0.0;
            for (i = xstart; i < xend; ++i) {
                for (j = 1; j < by + 1; ++j) {
                    anew[ind(i, j)] =
                        anew[ind(i, j)] / 2.0 + (aold[ind(i - 1, j)] + aold[ind(i + 1, j)] +
                                                 aold[ind(i, j - 1)] +
                                                 aold[ind(i, j + 1)]) / 4.0 / 2.0;
                    my_heat += anew[ind(i, j)];
                }
            }
#pragma omp critical
            {
                heat += my_heat;
            }
#pragma omp barrier

#pragma omp master
            {
                /* swap working arrays */
                tmp = anew;
                anew = aold;
                aold = tmp;
            }

            /* optional - print image */
            if (iter == niters - 1)
                printarr_par(iter, anew, n, px, py, rx, ry, bx, by, offx, offy, ind_f,
                             MPI_COMM_WORLD);
        }
    }

    t2 = MPI_Wtime();

    /* free working arrays and communication buffers */
    free(aold);
    free(anew);

    MPI_Type_free(&east_west_type);

    /* get final heat in the system */
    MPI_Allreduce(&heat, &rheat, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    if (!rank) {
#if !defined(OUTPUT_TOFILE)
        printf("n,nthreads,last_heat,time,flops\n");
        printf("%d,%d,%f,%f,%f\n", n, nthreads, rheat, t2 - t1,
               ((double) n * n * 7 * niters) / (t2 - t1));
#else
        char filename[20];
        sprintf(filename, "stencil_multiple_%d_%d", px * py, n);
        FILE *out = fopen(filename, "w");
        fprintf(out, "n,nthreads,last_heat,time,flops\n");
        fprintf(out, "%d,%d,%f,%f,%f\n", n, nthreads, rheat, t2 - t1,
                ((double) n * n * 7 * niters) / (t2 - t1));
        fclose(out);
#endif
    }

    MPI_Finalize();
    return 0;
}

void setup(int rank, int proc, int argc, char **argv,
           int *n_ptr, int *energy_ptr, int *niters_ptr, int *px_ptr, int *py_ptr, int *final_flag)
{
    int n, energy, niters, px, py;

    (*final_flag) = 0;

    if (argc < 6) {
        if (!rank)
            printf("usage: stencil_mpi <n> <energy> <niters> <px> <py>\n");
        (*final_flag) = 1;
        return;
    }

    n = atoi(argv[1]);  /* nxn grid */
    energy = atoi(argv[2]);     /* energy to be injected per iteration */
    niters = atoi(argv[3]);     /* number of iterations */
    px = atoi(argv[4]); /* 1st dim processes */
    py = atoi(argv[5]); /* 2nd dim processes */

    if (px * py != proc)
        MPI_Abort(MPI_COMM_WORLD, 1);   /* abort if px or py are wrong */
    if (n % py != 0)
        MPI_Abort(MPI_COMM_WORLD, 2);   /* abort px needs to divide n */
    if (n % px != 0)
        MPI_Abort(MPI_COMM_WORLD, 3);   /* abort py needs to divide n */

    (*n_ptr) = n;
    (*energy_ptr) = energy;
    (*niters_ptr) = niters;
    (*px_ptr) = px;
    (*py_ptr) = py;
}

void init_sources(int bx, int by, int offx, int offy, int n,
                  const int nsources, int sources[][2], int *locnsources_ptr, int locsources[][2])
{
    int i, locnsources = 0;

    sources[0][0] = n / 2;
    sources[0][1] = n / 2;
    sources[1][0] = n / 3;
    sources[1][1] = n / 3;
    sources[2][0] = n * 4 / 5;
    sources[2][1] = n * 8 / 9;

    for (i = 0; i < nsources; ++i) {    /* determine which sources are in my patch */
        int locx = sources[i][0] - offx;
        int locy = sources[i][1] - offy;
        if (locx >= 0 && locx < bx && locy >= 0 && locy < by) {
            locsources[locnsources][0] = locx + 1;      /* offset by halo zone */
            locsources[locnsources][1] = locy + 1;      /* offset by halo zone */
            locnsources++;
        }
    }

    (*locnsources_ptr) = locnsources;
}
