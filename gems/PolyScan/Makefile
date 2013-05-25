# Makefile for scantest, test program for generic convex polygon scan conversion
#
# Note: fancytest.c needs a main routine and several auxiliary routines
# in order to be compiled.

CFLAGS = $(GENCFLAGS)

scantest: scantest.o poly_scan.o poly.o
	$(CC) $(CFLAGS) -o scantest scantest.o poly_scan.o poly.o -lm

clean:
	/bin/rm -f scantest.o poly_clip.o poly_scan.o poly.o scantest
