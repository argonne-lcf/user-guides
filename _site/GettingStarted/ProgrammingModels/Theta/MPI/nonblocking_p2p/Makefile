# -*- Mode: Makefile; -*-
#
# See COPYRIGHT in top-level directory.
#

CC=cc
CFLAGS= -g -Wall -Wl,--whole-archive,-ldmapp,--no-whole-archive
STENCIL_COMMON_SRC=../common/stencil/printarr_par.c
STENCIL_CFLAGS=$(CFLAGS) -I../common/stencil 
BSPMM_COMMON_SRC=../common/bspmm/bspmm_common.c
BSPMM_CFLAGS=$(CFLAGS) -I../common/bspmm
BINS=stencil bspmm_pack

all: $(BINS)

stencil: stencil.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

bspmm_pack: bspmm_pack.c $(BSPMM_COMMON_SRC)
	$(CC) $(BSPMM_CFLAGS) $^ -o $@

clean:
	rm -f $(BINS)
	rm -f output*tmp
