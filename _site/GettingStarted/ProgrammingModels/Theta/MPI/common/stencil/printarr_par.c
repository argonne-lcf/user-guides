/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

#include "stencil_par.h"

struct bmpfile_magic {
    unsigned char magic[2];
};

struct bmpfile_header {
    uint32_t filesz;
    uint16_t creator1;
    uint16_t creator2;
    uint32_t bmp_offset;
};

struct bmpinfo_header {
    uint32_t header_sz;
    int32_t width;
    int32_t height;
    uint16_t nplanes;
    uint16_t bitspp;
    uint32_t compress_type;
    uint32_t bmp_bytesz;
    int32_t hres;
    int32_t vres;
    uint32_t ncolors;
    uint32_t nimpcolors;
};

void printarr_par(int iter, double *array, int size, int px, int py, int rx, int ry, int bx, int by,
                  int offx, int offy, int (*ind)(int, int, int), MPI_Comm comm)
{

    int myrank;

    int xcnt, ycnt, my_xcnt, my_ycnt;
    int i;
    int linesize = bx * 3;
    int padding;
    char *myline;

    MPI_File fh;
    char fname[128];
    snprintf(fname, 128, "./output-%i.bmp", iter);

    MPI_File_open(comm, fname, MPI_MODE_SEQUENTIAL | MPI_MODE_CREATE | MPI_MODE_WRONLY,
                  MPI_INFO_NULL, &fh);

    MPI_Comm_rank(comm, &myrank);

    if (myrank == 0) {

        struct bmpfile_magic magic;
        struct bmpfile_header header;
        struct bmpinfo_header bmpinfo;

        magic.magic[0] = 0x42;
        magic.magic[1] = 0x4D;

        MPI_File_write_shared(fh, &magic, sizeof(struct bmpfile_magic), MPI_BYTE,
                              MPI_STATUSES_IGNORE);

        header.filesz =
            sizeof(struct bmpfile_magic) + sizeof(struct bmpfile_header) +
            sizeof(struct bmpinfo_header) + size * (size * 3 + size * 3 % 4);
        header.creator1 = 0xFE;
        header.creator1 = 0xFE;
        header.bmp_offset =
            sizeof(struct bmpfile_magic) + sizeof(struct bmpfile_header) +
            sizeof(struct bmpinfo_header);

        MPI_File_write_shared(fh, &header, sizeof(struct bmpfile_header), MPI_BYTE,
                              MPI_STATUSES_IGNORE);

        bmpinfo.header_sz = sizeof(struct bmpinfo_header);
        bmpinfo.width = size;
        bmpinfo.height = size;
        bmpinfo.nplanes = 1;
        bmpinfo.bitspp = 24;
        bmpinfo.compress_type = 0;
        bmpinfo.bmp_bytesz = size * (size * 3 + size * 3 % 4);
        bmpinfo.hres = size;
        bmpinfo.vres = size;
        bmpinfo.ncolors = 0;
        bmpinfo.nimpcolors = 0;

        MPI_File_write_shared(fh, &bmpinfo, sizeof(struct bmpinfo_header), MPI_BYTE,
                              MPI_STATUSES_IGNORE);

    }

    padding = 0;
    if (((rx + 1) % px) == 0)
        padding = size * 3 % 4;
    myline = (char *) malloc(linesize + padding);

    my_xcnt = 0;
    my_ycnt = 0;
    xcnt = 0;
    ycnt = size;

    while (ycnt >= 0) {
        MPI_Barrier(comm);
        if ((xcnt == offx) && (ycnt >= offy) && (ycnt < offy + by)) {
            for (i = 0; i < linesize; i += 3) {
                int rgb;
                if (i / 3 > bx)
                    rgb = 0;
                else
                    rgb = (int) round(255.0 * array[ind(i / 3, by - my_ycnt, bx)]);
                if ((i == 0) || (i / 3 == bx - 1) || (my_ycnt == 0) || (my_ycnt == by - 1))
                    rgb = 255;
                if (rgb > 255)
                    rgb = 255;
                myline[i + 0] = 0;
                myline[i + 1] = 0;
                myline[i + 2] = rgb;
            }
            my_xcnt += bx;
            my_ycnt++;
            MPI_File_write_shared(fh, myline, linesize + padding, MPI_BYTE, MPI_STATUSES_IGNORE);
        }
        xcnt += bx;
        if (xcnt >= size) {
            xcnt = 0;
            ycnt--;
        }
    }

    free(myline);

    MPI_File_close(&fh);
}
