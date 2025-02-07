# -*- Mode: Makefile; -*-
#
# See COPYRIGHT in top-level directory.
#

CC=cc
CFLAGS= -g -Wall
STENCIL_COMMON_SRC=../common/stencil/printarr_par.c
STENCIL_CFLAGS= $(CFLAGS) -I../common/stencil
BINS=stencil_carttopo stencil_carttopo_neighcolls

all: $(BINS)

stencil_carttopo: stencil_carttopo.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

stencil_carttopo_neighcolls: stencil_carttopo_neighcolls.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

clean:
	rm -f $(BINS)
	rm -f output*.bmp
