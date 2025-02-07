
# c use "cc"
# c++ use "CC"
# fortran use "ftn"

CC=cc

WHICHCC=$(shell which $(CC))
CRAYVERB=$(shell $(CC) -craype-verbose))

hellompi: hellompi.c
	$(info "which ${CC}"=  $(WHICHCC))
	$(CC) -g -O0 -o hellompi hellompi.c


destroy: clean
	/bin/rm -f hellompi

clean:
	/bin/rm -f *.error *.output *.cobaltlog hellompi



