/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

/*
 * 2D stencil code using a blocking collective.
 *
 * 2D regular grid is divided into px * py blocks of grid points (px * py = # of processes.)
 * In every iteration, each process calls a blocking collective operation with derived data types to
 * exchange a halo with neighbors.
 */

#include "stencil_par.h"

/* row-major order */
#define ind(i,j) ((j)*(bx+2)+(i))

int ind_f(int i, int j, int bx)
{
    return ind(i, j);
}

void setup(int rank, int proc, int argc, char **argv,
           int *n_ptr, int *energy_ptr, int *niters_ptr, int *px_ptr, int *py_ptr, int *final_flag);

void init_sources(int bx, int by, int offx, int offy, int n,
                  const int nsources, int sources[][2], int *locnsources_ptr, int locsources[][2]);

void update_grid(int bx, int by, double *aold, double *anew, double *heat_ptr);

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

    int *send_counts, *recv_counts;
    int *sdispls, *rdispls;
    MPI_Datatype *types;

    double t1, t2;

    int iter, i;

    double *aold, *anew, *tmp;

    double heat, rheat;

    int final_flag;


    /* initialize MPI envrionment */
    MPI_Init(&argc, &argv);
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

    /* printf("%i (%i,%i) - w: %i, e: %i, n: %i, s: %i\n", rank, ry,rx,west,east,north,south); */

    /* allocate working arrays & communication buffers */
    aold = (double *) malloc((bx + 2) * (by + 2) * sizeof(double));     /* 1-wide halo zones! */
    anew = (double *) malloc((bx + 2) * (by + 2) * sizeof(double));     /* 1-wide halo zones! */

    memset(aold, 0, (bx + 2) * (by + 2) * sizeof(double));
    memset(anew, 0, (bx + 2) * (by + 2) * sizeof(double));

    /* initialize three heat sources */
    init_sources(bx, by, offx, offy, n, nsources, sources, &locnsources, locsources);

    /* create north-south datatype */
    MPI_Datatype north_south_type;
    MPI_Type_contiguous(bx, MPI_DOUBLE, &north_south_type);     /* Don't do this */
    MPI_Type_commit(&north_south_type);

    /* create east-west datatype */
    MPI_Datatype east_west_type;
    MPI_Type_vector(by, 1, bx + 2, MPI_DOUBLE, &east_west_type);
    MPI_Type_commit(&east_west_type);

    /* prepare arguments of alltoallw */
    send_counts = (int *) malloc(size * sizeof(int));
    memset(send_counts, 0, size * sizeof(int));

    recv_counts = (int *) malloc(size * sizeof(int));

    sdispls = (int *) malloc(size * sizeof(int));
    memset(sdispls, 0, size * sizeof(int));

    rdispls = (int *) malloc(size * sizeof(int));
    memset(rdispls, 0, size * sizeof(int));

    types = (MPI_Datatype *) malloc(size * sizeof(MPI_Datatype));
    for (i = 0; i < size; i++)
        types[i] = MPI_DATATYPE_NULL;

    if (north != MPI_PROC_NULL) {
        send_counts[north] = 1;
        sdispls[north] = ind(1, 1) * sizeof(double);
        rdispls[north] = ind(1, 0) * sizeof(double);
        types[north] = north_south_type;
    }
    if (south != MPI_PROC_NULL) {
        send_counts[south] = 1;
        sdispls[south] = ind(1, by) * sizeof(double);
        rdispls[south] = ind(1, by + 1) * sizeof(double);
        types[south] = north_south_type;
    }
    if (east != MPI_PROC_NULL) {
        send_counts[east] = 1;
        sdispls[east] = ind(bx, 1) * sizeof(double);
        rdispls[east] = ind(bx + 1, 1) * sizeof(double);
        types[east] = east_west_type;
    }
    if (west != MPI_PROC_NULL) {
        send_counts[west] = 1;
        sdispls[west] = ind(1, 1) * sizeof(double);
        rdispls[west] = ind(0, 1) * sizeof(double);
        types[west] = east_west_type;
    }

    /* use different count parameters because some MPI implementations do not consider displacements
     * in aliasing check */
    memcpy(recv_counts, send_counts, size * sizeof(int));

    t1 = MPI_Wtime();   /* take time */

    for (iter = 0; iter < niters; ++iter) {

        /* refresh heat sources */
        for (i = 0; i < locnsources; ++i) {
            aold[ind(locsources[i][0], locsources[i][1])] += energy;    /* heat source */
        }

        MPI_Alltoallw(aold, send_counts, sdispls, types, aold, recv_counts, rdispls, types,
                      MPI_COMM_WORLD);

        /* update grid points */
        update_grid(bx, by, aold, anew, &heat);

        /* swap working arrays */
        tmp = anew;
        anew = aold;
        aold = tmp;

        /* optional - print image */
        if (iter == niters - 1)
            printarr_par(iter, anew, n, px, py, rx, ry, bx, by, offx, offy, ind_f, MPI_COMM_WORLD);
    }

    t2 = MPI_Wtime();

    /* free working arrays, communication buffers, and alltoallw arguments */
    free(aold);
    free(anew);
    free(send_counts);
    free(recv_counts);
    free(sdispls);
    free(rdispls);
    free(types);

    MPI_Type_free(&east_west_type);
    MPI_Type_free(&north_south_type);

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
            locsources[locnsources][0] = locx + 1;      /* offset by halo zone */
            locsources[locnsources][1] = locy + 1;      /* offset by halo zone */
            locnsources++;
        }
    }

    (*locnsources_ptr) = locnsources;
}


void update_grid(int bx, int by, double *aold, double *anew, double *heat_ptr)
{
    int i, j;
    double heat = 0.0;

    for (i = 1; i < bx + 1; ++i) {
        for (j = 1; j < by + 1; ++j) {
            anew[ind(i, j)] =
                anew[ind(i, j)] / 2.0 + (aold[ind(i - 1, j)] + aold[ind(i + 1, j)] +
                                         aold[ind(i, j - 1)] + aold[ind(i, j + 1)]) / 4.0 / 2.0;
            heat += anew[ind(i, j)];
        }
    }

    (*heat_ptr) = heat;
}
