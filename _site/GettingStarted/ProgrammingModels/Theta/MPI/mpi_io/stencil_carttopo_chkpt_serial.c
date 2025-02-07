/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

/*
 * 2D stencil code using cartesian topology and nonblocking send/receive.
 *
 * 2D regular grid is divided into px * py blocks of grid points (px * py = # of processes.)
 * In every iteration, each process calls nonblocking operations with derived data types to exchange
 * grid points in a halo region with neighbors. Neighbors are calculated with cartesian topology.
 *
 * Add support for checkpoint/restart using legacy POSIX I/O implementation:
 * - every rank sends its data to rank 0 which writes it to the file
 * - on restart rank 0 reads data from file and distributes it to other ranks
 */

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "stencil_par.h"

#define MAX_FILENAME_LENGTH (128)

/* row-major order */
#define ind(i,j) ((j)*(bx+2)+(i))

int ind_f(int i, int j, int bx)
{
    return ind(i, j);
}

void setup(int rank, int proc, int argc, char **argv,
           int *n_ptr, int *energy_ptr, int *niters_ptr, int *px_ptr, int *py_ptr,
           char **opt_prefix, int *opt_restart_iter, int *final_flag);

void init_sources(int bx, int by, int offx, int offy, int n,
                  const int nsources, int sources[][2], int *locnsources_ptr, int locsources[][2]);

void update_grid(int bx, int by, double *aold, double *anew, double *heat_ptr);

int write_checkpoint_serial(char *prefix, int procs, int n, int *coords, int bx, int by, int iter,
                            double *buf);

int read_checkpoint_serial(char *prefix, int procs, int n, int *coords, int bx, int by, int iter,
                           double *buf);

int main(int argc, char **argv)
{
    int rank, size;
    int n, energy, niters, px, py;

    int north, south, west, east;
    int bx, by, offx, offy;

    /* three heat sources */
    const int nsources = 3;
    int sources[nsources][2];
    int locnsources;            /* number of sources in my area */
    int locsources[nsources][2];        /* sources local to my rank */

    double t1, t2;

    int iter, i;

    double *aold, *anew, *tmp;

    double heat, rheat;

    int final_flag;

    int opt_restart_iter;
    char *opt_prefix;

    /* initialize MPI envrionment */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* argument checking and setting */
    setup(rank, size, argc, argv, &n, &energy, &niters,
          &px, &py, &opt_prefix, &opt_restart_iter, &final_flag);

    if (final_flag == 1) {
        MPI_Finalize();
        exit(0);
    }

    /* Create a communicator with a topology */
    MPI_Comm cart_comm;
    int dims[2], coords[2];
    int periods[2] = { 0, 0 };
    dims[0] = px;
    dims[1] = py;

    MPI_Dims_create(size, 2, dims);
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);
    MPI_Cart_coords(cart_comm, rank, 2, coords);

    /* determine my four neighbors */
    MPI_Cart_shift(cart_comm, 0, 1, &west, &east);
    MPI_Cart_shift(cart_comm, 1, 1, &north, &south);

    /* decompose the domain */
    bx = n / px;        /* block size in x */
    by = n / py;        /* block size in y */
    offx = coords[0] * bx;      /* offset in x */
    offy = coords[1] * by;      /* offset in y */

    /* printf("%i (%i,%i) - w: %i, e: %i, n: %i, s: %i\n", rank, ry,rx,west,east,north,south); */

    /* allocate working arrays & communication buffers */
    aold = (double *) malloc((bx + 2) * (by + 2) * sizeof(double));     /* 1-wide halo zones! */
    anew = (double *) malloc((bx + 2) * (by + 2) * sizeof(double));     /* 1-wide halo zones! */

    memset(aold, 0, (bx + 2) * (by + 2) * sizeof(double));
    memset(anew, 0, (bx + 2) * (by + 2) * sizeof(double));

    /* initialize three heat sources */
    init_sources(bx, by, offx, offy, n, nsources, sources, &locnsources, locsources);

    /* create east-west datatype */
    MPI_Datatype east_west_type;
    MPI_Type_vector(by, 1, bx + 2, MPI_DOUBLE, &east_west_type);
    MPI_Type_commit(&east_west_type);

    t1 = MPI_Wtime();   /* take time */

    iter = 0;

    /* check whether restart is needed */
    if (opt_restart_iter > 0 && opt_restart_iter < niters - 1) {
        /* Recover buffers
         *
         * Explanation:
         * let aold*(i) = refresh_heat_sources(aold(i))
         * let anew*(i) = update_grid(aold*(i), anew(i))
         *
         * At the next iteration:
         * aold(i+1) = anew*(i)
         * anew(i+1) = aold*(i) = refresh_heat_sources(aold(i)) = refresh_heat_sources(anew*(i-1))
         *
         * Then to restart from iteration 'i' we need anew*(i) and anew*(i-1). From anew*(i-1) we
         * can recover anew*(i+1) by refreshing the heat sources of anew*(i-1).
         */
        read_checkpoint_serial(opt_prefix, size, n, coords, bx, by, opt_restart_iter, aold);
        read_checkpoint_serial(opt_prefix, size, n, coords, bx, by, opt_restart_iter - 1, anew);

        /* refresh heat sources */
        for (i = 0; i < locnsources; ++i) {
            anew[ind(locsources[i][0], locsources[i][1])] += energy;    /* heat source */
        }

        /* set restart iteration */
        iter = opt_restart_iter + 1;
    }

    for (; iter < niters; ++iter) {
        /* refresh heat sources */
        for (i = 0; i < locnsources; ++i) {
            aold[ind(locsources[i][0], locsources[i][1])] += energy;    /* heat source */
        }

        /* exchange data with neighbors */
        MPI_Request reqs[8];
        MPI_Isend(&aold[ind(1, 1)] /* north */ , bx, MPI_DOUBLE, north, 9, cart_comm, &reqs[0]);
        MPI_Isend(&aold[ind(1, by)] /* south */ , bx, MPI_DOUBLE, south, 9, cart_comm, &reqs[1]);
        MPI_Isend(&aold[ind(bx, 1)] /* east */ , 1, east_west_type, east, 9, cart_comm, &reqs[2]);
        MPI_Isend(&aold[ind(1, 1)] /* west */ , 1, east_west_type, west, 9, cart_comm, &reqs[3]);
        MPI_Irecv(&aold[ind(1, 0)] /* north */ , bx, MPI_DOUBLE, north, 9, cart_comm, &reqs[4]);
        MPI_Irecv(&aold[ind(1, by + 1)] /* south */ , bx, MPI_DOUBLE, south, 9, cart_comm,
                  &reqs[5]);
        MPI_Irecv(&aold[ind(bx + 1, 1)] /* east */ , 1, east_west_type, east, 9, cart_comm,
                  &reqs[6]);
        MPI_Irecv(&aold[ind(0, 1)] /* west */ , 1, east_west_type, west, 9, cart_comm, &reqs[7]);
        MPI_Waitall(8, reqs, MPI_STATUSES_IGNORE);

        /* update grid points */
        update_grid(bx, by, aold, anew, &heat);

        /* checkpoint buffers */
        write_checkpoint_serial(opt_prefix, size, n, coords, bx, by, iter, anew);

        /* swap working arrays */
        tmp = anew;
        anew = aold;
        aold = tmp;

        /* optional - print image */
        if (iter == niters - 1)
            printarr_par(iter, anew, n, px, py, coords[0], coords[1],
                         bx, by, offx, offy, ind_f, MPI_COMM_WORLD);
    }

    t2 = MPI_Wtime();

    /* free working arrays and communication buffers */
    free(aold);
    free(anew);

    MPI_Type_free(&east_west_type);
    MPI_Comm_free(&cart_comm);

    /* get final heat in the system */
    MPI_Allreduce(&heat, &rheat, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    if (!rank)
        printf("[%i] last heat: %f time: %f\n", rank, rheat, t2 - t1);

    MPI_Finalize();
    return 0;
}

void setup(int rank, int proc, int argc, char **argv,
           int *n_ptr, int *energy_ptr, int *niters_ptr, int *px_ptr, int *py_ptr,
           char **opt_prefix, int *opt_restart_iter, int *final_flag)
{
    int n, energy, niters, px, py;

    (*final_flag) = 0;

    if (argc < 6) {
        if (!rank)
            printf
                ("usage: stencil_mpi <n> <energy> <niters> <px> <py> <ckpt_prefix> <restart_iter>\n");
        (*final_flag) = 1;
        return;
    }

    n = atoi(argv[1]);  /* nxn grid */
    energy = atoi(argv[2]);     /* energy to be injected per iteration */
    niters = atoi(argv[3]);     /* number of iterations */
    px = atoi(argv[4]); /* 1st dim processes */
    py = atoi(argv[5]); /* 2nd dim processes */

    if (argc > 6 && argc <= 7) {
        *opt_prefix = argv[6];  /* checkpoint file prefix */
    } else if (argc > 7) {
        *opt_prefix = argv[6];
        *opt_restart_iter = atoi(argv[7]);      /* restart from iteration */
    }

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

int write_checkpoint_serial(char *prefix, int procs, int n, int *coords, int bx, int by, int iter,
                            double *buf)
{
    int i;
    int rank;
    int header[2];              /* file header metadata */
    int row;
    int fd;                     /* file descriptor */
    off_t stride;               /* stride inside file */
    off_t offset;               /* offset inside file */
    double *iobuf;              /* rank 0 recv buffer */
    MPI_Datatype memtype;
    struct coord_array {
        int xcoord;
        int ycoord;
    } *coord_array;
    char filename[MAX_FILENAME_LENGTH] = { 0 }; /* checkpoint file name */

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* rank 0 receives data from each process and writes it to file
     * NOTE: this is not the most efficient way of writing to a file
     *       but simplifies the code and serves as example of legacy
     *       I/O strategy using POSIX. A better implementation would
     *       write data to the file sequentially instead of strided.*/
    if (rank == 0) {
        /* recv domain partitioning info from other ranks */
        coord_array = (struct coord_array *) malloc(sizeof(struct coord_array) * procs);
        MPI_Gather(coords, 2, MPI_INT, coord_array, 2, MPI_INT, 0, MPI_COMM_WORLD);

        /* allocate space for I/O buffer */
        iobuf = (double *) malloc(sizeof(double) * bx * by);

        /* update checkpoint file name for current iteration */
        snprintf(filename, MAX_FILENAME_LENGTH, "%s-%d.chkpt", prefix, iter);

        /* open checkpoint file in write only mode */
        fd = open(filename, O_CREAT | O_WRONLY, S_IRWXU);

        /* write header metadata to file */
        header[0] = n;
        header[1] = iter;
        pwrite(fd, header, 2 * sizeof(int), 0);

        /* initialize offset and stride */
        offset = 2 * sizeof(int);
        stride = n * sizeof(double);

        /* rank 0 writes its data first (no comm needed) */
        for (row = 1; row <= by; row++) {
            pwrite(fd, buf + (bx + 2) * row + 1, bx * sizeof(double), offset);
            offset += stride;
        }

        /* then it writes data received from other ranks */
        for (i = 1; i < procs; i++) {
            MPI_Recv(iobuf, bx * by, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            offset =
                (coord_array[i].ycoord * by * n + coord_array[i].xcoord * bx) * sizeof(double) +
                2 * sizeof(int);
            for (row = 0; row < by; row++) {
                pwrite(fd, iobuf + (row * bx), bx * sizeof(double), offset);
                offset += stride;
            }
        }

        /* close checkpoint file */
        close(fd);

        /* free I/O buf */
        free(iobuf);

        free(coord_array);
    } else {
        /* send domain partitioning info to rank 0 */
        MPI_Gather(coords, 2, MPI_INT, coord_array, 2, MPI_INT, 0, MPI_COMM_WORLD);

        /* create memory layout */
        MPI_Type_vector(by, bx, bx + 2, MPI_DOUBLE, &memtype);
        MPI_Type_commit(&memtype);

        /* send data to rank 0 */
        MPI_Send(buf + (bx + 2 + 1), 1, memtype, 0, 0, MPI_COMM_WORLD);

        /* free memory layout object */
        MPI_Type_free(&memtype);
    }

    /* return written bytes */
    return (rank == 0) ? (bx * by * sizeof(double) + 2 * sizeof(int)) : (bx * by * sizeof(double));
}

int read_checkpoint_serial(char *prefix, int procs, int n, int *coords, int bx, int by, int iter,
                           double *buf)
{
    int i;
    int rank;
    int header[2];              /* file header metadata */
    int row;
    int fd;                     /* file descriptor */
    off_t stride;               /* stride inside file */
    off_t offset;               /* offset inside file */
    double *iobuf;              /* rank 0 I/O buffer */
    MPI_Datatype memtype;
    struct coord_array {
        int xcoord;
        int ycoord;
    } *coord_array;
    char filename[MAX_FILENAME_LENGTH] = { 0 }; /* checkpoint file name */

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        /* recv domain partitioning info from other ranks */
        coord_array = (struct coord_array *) malloc(sizeof(struct coord_array) * procs);
        MPI_Gather(coords, 2, MPI_INT, coord_array, 2, MPI_INT, 0, MPI_COMM_WORLD);

        /* allocate space for I/O buffer */
        iobuf = (double *) malloc(sizeof(double) * bx * by);

        /* update checkpoint file name for current iteration */
        snprintf(filename, MAX_FILENAME_LENGTH, "%s-%d.chkpt", prefix, iter);

        /* open checkpoint file in read only mode */
        fd = open(filename, O_RDONLY, S_IRWXU);
        if (fd < 0) {
            fprintf(stderr, "Error opening checkpoint file %s: %s.\n", filename, strerror(errno));
            MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
        }

        /* read header metadata from file */
        pread(fd, header, 2 * sizeof(int), 0);
        if (header[1] != iter) {
            fprintf(stderr, "Error restarting iter %d from %s.\n", header[1], filename);
            MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
        }

        /* initialize offset and stride */
        offset = 2 * sizeof(int);
        stride = n * sizeof(double);

        /* rank 0 reads its data first (no comm needed) */
        for (row = 1; row <= by; row++) {
            pread(fd, buf + (bx + 2) * row + 1, bx * sizeof(double), offset);
            offset += stride;
        }

        /* then it reads data on behalf of other ranks and send it to them */
        for (i = 1; i < procs; i++) {
            offset =
                (coord_array[i].ycoord * by * n + coord_array[i].xcoord * bx) * sizeof(double) +
                2 * sizeof(int);
            for (row = 0; row < by; row++) {
                pread(fd, iobuf + (row * bx), bx * sizeof(double), offset);
                offset += stride;
            }
            MPI_Send(iobuf, bx * by, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }

        /* close checkpoint file */
        close(fd);

        /* free I/O buf */
        free(iobuf);

        free(coord_array);
    } else {
        /* send domain partitioning info to rank 0 */
        MPI_Gather(coords, 2, MPI_INT, coord_array, 2, MPI_INT, 0, MPI_COMM_WORLD);

        /* create memory layout */
        MPI_Type_vector(by, bx, bx + 2, MPI_DOUBLE, &memtype);
        MPI_Type_commit(&memtype);

        /* recv data from rank 0 */
        MPI_Recv(buf + (bx + 2 + 1), 1, memtype, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /* free memory layout object */
        MPI_Type_free(&memtype);
    }

    /* return read bytes */
    return (bx * by * sizeof(double));
}
