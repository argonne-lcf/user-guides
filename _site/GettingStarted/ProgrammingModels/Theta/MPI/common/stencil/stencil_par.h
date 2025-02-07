/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

#ifndef STENCIL_PAR_H_INCLUDED
#define STENCIL_PAR_H_INCLUDED

#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void printarr_par(int iter, double *array, int size, int px, int py, int rx, int ry, int bx, int by,
                  int offx, int offy, int (*ind)(int i, int j, int bx), MPI_Comm comm);

#endif /* STENCIL_PAR_H_INCLUDED */
