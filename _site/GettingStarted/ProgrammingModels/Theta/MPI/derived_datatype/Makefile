# -*- Mode: Makefile; -*-
#
# See COPYRIGHT in top-level directory.
#

CC=mpicc
CFLAGS= -g -Wall
STENCIL_COMMON_SRC=../common/stencil/printarr_par.c
STENCIL_CFLAGS= $(CFLAGS) -I../common/stencil
BSPMM_COMMON_SRC=../common/bspmm/bspmm_common.c
BSPMM_CFLAGS=$(CFLAGS) -I../common/bspmm
BINS=stencil stencil_alltoallw bspmm_vector bspmm_subarray

all: $(BINS)

stencil: stencil.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

stencil_alltoallw: stencil_alltoallw.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

bspmm_vector: bspmm_vector.c $(BSPMM_COMMON_SRC)
	$(CC) $(BSPMM_CFLAGS) $^ -o $@

bspmm_subarray: bspmm_subarray.c $(BSPMM_COMMON_SRC)
	$(CC) $(BSPMM_CFLAGS) $^ -o $@

clean:
	rm -f $(BINS)
	rm -f output*.bmp
