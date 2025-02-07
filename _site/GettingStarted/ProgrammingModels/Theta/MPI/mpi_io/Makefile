# -*- Mode: Makefile; -*-
#
# See COPYRIGHT in top-level directory.
#

CC=mpicc
STENCIL_COMMON_SRC=../common/stencil/printarr_par.c
STENCIL_CFLAGS= -g -I../common/stencil
BINS=stencil_carttopo_chkpt_coll stencil_carttopo_chkpt_icoll stencil_carttopo_chkpt_indep stencil_carttopo_chkpt_serial

all: $(BINS)

stencil_carttopo_chkpt_coll: stencil_carttopo_chkpt_coll.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

stencil_carttopo_chkpt_icoll: stencil_carttopo_chkpt_icoll.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

stencil_carttopo_chkpt_indep: stencil_carttopo_chkpt_indep.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

stencil_carttopo_chkpt_serial: stencil_carttopo_chkpt_serial.c $(STENCIL_COMMON_SRC)
	$(CC) $(STENCIL_CFLAGS) $^ -o $@ -lm

clean:
	rm -f $(BINS)
	rm -fr output*.bmp *.dSYM
	rm -f *.o *.chkpt
