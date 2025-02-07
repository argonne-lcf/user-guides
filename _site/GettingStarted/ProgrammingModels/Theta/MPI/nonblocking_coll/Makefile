# -*- Mode: Makefile; -*-
#
# See COPYRIGHT in top-level directory.
#

CC=cc
CFLAGS= -g -Wall -Wl,--whole-archive,-ldmapp,--no-whole-archive
STENCIL_COMMON_SRC=../common/stencil/printarr_par.c
STENCIL_CFLAGS=$(CFLAGS) -I../common/stencil 
BINS=stencil_alltoallw

all: $(BINS)

stencil_alltoallw: stencil_alltoallw.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

clean:
	rm -f $(BINS)
