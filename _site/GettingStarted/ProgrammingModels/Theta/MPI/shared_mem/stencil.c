/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

/*
 * 2D stencil code using RMA operations for shared memory.
 *
 * 2D regular grid is divided into px * py blocks of grid points (px * py = # of processes.)
 * In every iteration, each process synchronizes grid points in shared memory.
 */

#include "stencil_par.h"

/* row-major order */
#define ind(i,j) ((j)*(bx)+(i))

int ind_f(int i, int j, int bx)
{
    return ind(i, j);
}

void setup(int rank, int proc, int argc, char **argv,
           int *n_ptr, int *energy_ptr, int *niters_ptr, int *px_ptr, int *py_ptr, int *final_flag);

void init_sources(int bx, int by, int offx, int offy, int n,
                  const int nsources, int sources[][2], int *locnsources_ptr, int locsources[][2]);

void update_inner_grid(int bx, int by, double *aold, double *anew, double *heat_ptr);
void update_outer_grid(int bx, int by, double *aold, double *anew, int north, double *northptr_old,
                       int south, double *southptr_old, int east, double *eastptr_old, int west,
                       double *westptr_old, double *heat_ptr);

void swap_ptr(double **ptr1, double **ptr2);

int main(int argc, char **argv)
{
    int rank, size;
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

    int iter, i;

    double *aold, *anew;
    double heat, rheat;

    int final_flag;

    int grid_size;              /* grid size */
    double *win_mem;            /* window memory */
    MPI_Win win;                /* RMA window */


    /* initialize MPI envrionment */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* create shared memory communicator */
    MPI_Comm shm_comm;
    MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &shm_comm);

    int shm_rank, shm_size;
    MPI_Comm_size(shm_comm, &shm_size);
    MPI_Comm_rank(shm_comm, &shm_rank);

    /* works only when all processes are in the same shared memory region */
    if (shm_size != size)
        MPI_Abort(MPI_COMM_WORLD, 1);

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

    /* printf("%i (%i,%i) - w: %i, e: %i, n: %i, s: %i\n", rank, ry,rx,west,east,north,south); */

    grid_size = bx * by;        /* process-local grid */

    /* create shared RMA window upon working array */
    MPI_Win_allocate_shared(2 * grid_size * sizeof(double), sizeof(double), MPI_INFO_NULL, shm_comm,
                            &win_mem, &win);

    /* check whether the windows are unified memory model */
    int *mem_model = NULL, flag = 0;
    MPI_Win_get_attr(win, MPI_WIN_MODEL, &mem_model, &flag);
    if (*mem_model != MPI_WIN_UNIFIED) {
        fprintf(stderr, "MPI supports accessing shared memory windows only in "
                "UNIFIED memory model.\n");
        fflush(stderr);
        MPI_Abort(MPI_COMM_WORLD, 4);   /* abort if memory model is unsupported */
    }

    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, rank, 0, win);
    memset(win_mem, 0, 2 * grid_size * sizeof(double));
    MPI_Win_unlock(rank, win);

    anew = win_mem;
    aold = win_mem + grid_size; /* second half is aold! */

    double *northptr_new, *southptr_new, *eastptr_new, *westptr_new;
    double *northptr_old, *southptr_old, *eastptr_old, *westptr_old;
    MPI_Aint win_sz;
    int disp_unit;

    /* locate the shared memory region for each neighbor */
    MPI_Win_shared_query(win, north, &win_sz, &disp_unit, &northptr_new);
    MPI_Win_shared_query(win, south, &win_sz, &disp_unit, &southptr_new);
    MPI_Win_shared_query(win, east, &win_sz, &disp_unit, &eastptr_new);
    MPI_Win_shared_query(win, west, &win_sz, &disp_unit, &westptr_new);

    /* second half is aold on each neighbor */
    northptr_old = northptr_new + grid_size;
    southptr_old = southptr_new + grid_size;
    eastptr_old = eastptr_new + grid_size;
    westptr_old = westptr_new + grid_size;

    /* initialize three heat sources */
    init_sources(bx, by, offx, offy, n, nsources, sources, &locnsources, locsources);

    t1 = MPI_Wtime();   /* take time */

    MPI_Win_lock_all(0, win);

    for (iter = 0; iter < niters; ++iter) {
        double inner_heat, outer_heat;

        /* refresh heat sources */
        for (i = 0; i < locnsources; ++i) {
            aold[ind(locsources[i][0], locsources[i][1])] += energy;    /* heat source */
        }

        MPI_Win_sync(win);      /* ensure completion of local updates before MPI barrier */
        MPI_Barrier(shm_comm);  /* ensure neighbors have completed heat source refreshing */
        MPI_Win_sync(win);      /* ensure remote updates are locally visible
                                 * (e.g., the first local sync might perform before remote update) */

        /* update inner grid points */
        update_inner_grid(bx, by, aold, anew, &inner_heat);

        /* update outer grid points */
        update_outer_grid(bx, by, aold, anew, north, northptr_old, south, southptr_old, east,
                          eastptr_old, west, westptr_old, &outer_heat);
        heat = inner_heat + outer_heat;

        /* swap working arrays */
        swap_ptr(&aold, &anew);
        swap_ptr(&northptr_old, &northptr_new);
        swap_ptr(&southptr_old, &southptr_new);
        swap_ptr(&eastptr_old, &eastptr_new);
        swap_ptr(&westptr_old, &westptr_new);

        /* optional - print image */
        if (iter == niters - 1)
            printarr_par(iter, anew, n, px, py, rx, ry, bx, by, offx, offy, ind_f, MPI_COMM_WORLD);
    }

    MPI_Win_unlock_all(win);

    t2 = MPI_Wtime();

    MPI_Win_free(&win);
    MPI_Comm_free(&shm_comm);

    /* get final heat in the system */
    MPI_Allreduce(&heat, &rheat, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    if (!rank)
        printf("[%i] last heat: %f time: %f\n", rank, rheat, t2 - t1);

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
            locsources[locnsources][0] = locx;
            locsources[locnsources][1] = locy;
            locnsources++;
        }
    }

    (*locnsources_ptr) = locnsources;
}


void update_inner_grid(int bx, int by, double *aold, double *anew, double *heat_ptr)
{
    int i, j;
    double heat = 0.0;

    for (i = 1; i < bx - 1; ++i) {
        for (j = 1; j < by - 1; ++j) {
            anew[ind(i, j)] =
                anew[ind(i, j)] / 2.0 + (aold[ind(i - 1, j)] + aold[ind(i + 1, j)] +
                                         aold[ind(i, j - 1)] + aold[ind(i, j + 1)]) / 4.0 / 2.0;
            heat += anew[ind(i, j)];
        }
    }

    (*heat_ptr) = heat;
}

void update_outer_grid(int bx, int by, double *aold, double *anew, int north, double *northptr_old,
                       int south, double *southptr_old, int east, double *eastptr_old, int west,
                       double *westptr_old, double *heat_ptr)
{
    int i, j;
    double heat = 0.0;

    for (i = 0; i < bx; ++i) {
        for (j = 0; j < by; ++j) {
            if (i >= 1 && j >= 1 && i < bx - 1 && j < by - 1)
                continue;       /* it has been calculated in update_inner_grid */
            double north_val, south_val, east_val, west_val;
            if (j == 0) {
                north_val = north != MPI_PROC_NULL ? northptr_old[ind(i, by - 1)] : 0.0;
            } else {
                north_val = aold[ind(i, j - 1)];
            }
            if (j == by - 1) {
                south_val = south != MPI_PROC_NULL ? southptr_old[ind(i, 0)] : 0.0;
            } else {
                south_val = aold[ind(i, j + 1)];
            }
            if (i == bx - 1) {
                east_val = east != MPI_PROC_NULL ? eastptr_old[ind(0, j)] : 0.0;
            } else {
                east_val = aold[ind(i + 1, j)];
            }
            if (i == 0) {
                west_val = west != MPI_PROC_NULL ? westptr_old[ind(bx - 1, j)] : 0.0;
            } else {
                west_val = aold[ind(i - 1, j)];
            }
            anew[ind(i, j)] =
                anew[ind(i, j)] / 2.0 + (north_val + south_val + east_val + west_val) / 4.0 / 2.0;
            heat += anew[ind(i, j)];
        }
    }

    (*heat_ptr) = heat;
}

void swap_ptr(double **ptr1, double **ptr2)
{
    double *tmp = NULL;

    tmp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = tmp;
}
