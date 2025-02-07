

CC=mpicc

hellompi: hellompi.c
	which $(CC)
	$(CC) -g -O0 -o hellompi hellompi.c


destroy: clean
	/bin/rm -f hellompi

clean:
	/bin/rm -f *.error *.output *.cobaltlog hellompi



