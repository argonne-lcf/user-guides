# -*- Mode: Makefile; -*-
#
# See COPYRIGHT in top-level directory.
#

CC=mpicc
CFLAGS= -g -Wall -fopenmp
STENCIL_COMMON_SRC=../common/stencil/printarr_par.c
STENCIL_CFLAGS=$(CFLAGS) -I../common/stencil
BSPMM_COMMON_SRC=../common/bspmm/bspmm_common.c
BSPMM_CFLAGS=$(CFLAGS) -I../common/bspmm
BINS=stencil_funneled stencil_multiple stencil_multiple_ncomms
BINS+=bspmm_funneled bspmm_multiple bspmm_multiple_nwins

all: $(BINS)

stencil_funneled: stencil_funneled.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

stencil_multiple: stencil_multiple.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

stencil_multiple_ncomms: stencil_multiple_ncomms.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

bspmm_funneled: bspmm_funneled.c $(BSPMM_COMMON_SRC)
	$(CC) $(BSPMM_CFLAGS) $^ -o $@

bspmm_multiple: bspmm_multiple.c $(BSPMM_COMMON_SRC)
	$(CC) $(BSPMM_CFLAGS) $^ -o $@

bspmm_multiple_nwins: bspmm_multiple_nwins.c $(BSPMM_COMMON_SRC)
	$(CC) $(BSPMM_CFLAGS) $^ -o $@

clean:
	rm -f $(BINS)
	rm -f output*.bmp
