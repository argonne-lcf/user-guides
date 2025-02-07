# -*- Mode: Makefile; -*-
#
# See COPYRIGHT in top-level directory.
#

CC=mpicc
CFLAGS= -g -Wall
STENCIL_COMMON_SRC=../common/stencil/printarr_par.c
STENCIL_CFLAGS=$(CFLAGS) -I../common/stencil
BSPMM_COMMON_SRC=../common/bspmm/bspmm_common.c
BSPMM_CFLAGS=$(CFLAGS) -I../common/bspmm
BINS=stencil_fence_put stencil_fence_get stencil_pscw_put stencil_lock_put
BINS+=bspmm bspmm_counter

all: $(BINS)

stencil_fence_put: stencil_fence_put.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

stencil_fence_get: stencil_fence_get.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

stencil_pscw_put: stencil_pscw_put.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

stencil_lock_put: stencil_lock_put.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

bspmm: bspmm.c $(BSPMM_COMMON_SRC)
	$(CC) $(BSPMM_CFLAGS) $^ -o $@

bspmm_counter: bspmm_counter.c $(BSPMM_COMMON_SRC)
	$(CC) $(BSPMM_CFLAGS) $^ -o $@

clean:
	rm -f $(BINS)
	rm -f output*.bmp
