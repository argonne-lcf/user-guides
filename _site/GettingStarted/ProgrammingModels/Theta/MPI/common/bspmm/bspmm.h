/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

#ifndef BSPMM_H_INCLUDED
#define BSPMM_H_INCLUDED

#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>

#define BLK_DIM 128
#define SPARSITY_A 0.8
#define SPARSITY_B 0.8
#define RAND_RANGE 10

int setup(int rank, int nprocs, int argc, char **argv, int *mat_dim_ptr);
void init_mats(int mat_dim, double *mat_a, double *mat_b, double *mat_c);
void check_mats(double *mat_a, double *mat_b, double *mat_c, int mat_dim);

#endif /* BSPMM_H_INCLUDED */
