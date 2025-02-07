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
BINS=stencil bspmm_counter

all: $(BINS)

sort_any_procs: sort_any_procs.c
	$(CC) $(CFLAGS) $^ -o $@ -lm

stencil: stencil.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

bspmm_counter: bspmm_counter.c $(BSPMM_COMMON_SRC)
	$(CC) $(BSPMM_CFLAGS) $^ -o $@

clean:
	rm -f $(BINS)
	rm -f output*.bmp
