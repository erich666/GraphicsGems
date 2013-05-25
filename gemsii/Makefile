# Makefile for Graphics Gems II source
#
# Eric Haines, 9/92
#
# This make file will build "gemslib2.a" and a number of executables.
# Gemslib2 is built solely for debugging purposes -- it is not intended
# to be used as a library.
#
# Note that some of the gems need additional macros, functions, tables
# driving routines, etc. before they will compile or run properly.
# These include:
#
# Hilbert assumes GL is available
#
# hot needs write_pixel and read_pixel routines
#
# ran_ramp needs fb_init, fb_setmap and fb_done routines
#
# VoxelCache needs RAY_REC, LIGHT_REC, TRIANGLE_REC, cache, object, and voxel
#	structures.  Code is mostly for illustrative purposes.
#
# radiosity code needs a fleshed out draw.c function

#
# C compiler flags
#
CFLAGS = -g
#
# Location of Graphics Gems library
#
LIBFILE = gemslib2.a

#
# Graphics Gems II Vector Library
#
VECLIB = GGVecLib.o

MFLAGS = "LIBFILE = ../$(LIBFILE)" "GENCFLAGS = $(CFLAGS)"

SHELL = /bin/sh

OFILES = FastUpdate.o GGVecLib.o InterPhong.o RayCPhdron.o \
	c_format.o hot.o inverse.o noise3.o quantizer.o \
	ran_ramp.o rotate.o rotate8x8.o sparse.o unmatrix.o xlines.o \
	Hilbert.o VoxelCache.o

DIRS =	BitCounting Peano RealPixels dither intersect inv_cmap radiosity \
	viewcorr

ALL =	c_format quantizer xlines hot ran_ramp Hilbert $(LIBFILE)

all: $(ALL)
	@for d in $(DIRS) ; do \
		(cd $$d ; $(MAKE) $(MFLAGS)) ;\
	done

$(LIBFILE): $(OFILES) $(VECLIB)
	ar rcs $(LIBFILE) $(OFILES) $(VECLIB)

Hilbert: Hilbert.o
	$(CC) $(CFLAGS) -o $@ Hilbert.o

c_format: c_format.o
	$(CC) $(CFLAGS) -o $@ c_format.o

hot:	hot.o
	$(CC) $(CFLAGS) -o $@ hot.o -lm

quantizer: quantizer.o
	$(CC) $(CFLAGS) -o $@ quantizer.o

ran_ramp: ran_ramp.o
	$(CC) $(CFLAGS) -o $@ ran_ramp.o

xlines: xlines.o
	$(CC) $(CFLAGS) -o $@ xlines.o

clean:
	@for d in $(DIRS) ; do \
		(cd $$d ; $(MAKE) $(MFLAGS) clean) ;\
	done
	/bin/rm -f $(OFILES) $(VECLIB)
	/bin/rm -f FastUpdate.o GGVecLib.o Hilbert.o InterPhong.o \
		RayCPhdron.o VoxelCache.o c_format.o hot.o inverse.o noise3.o \
		quantizer.o ran_ramp.o rotate.o rotate8x8.o sparse.o \
		unmatrix.o xlines.o \
		Hilbert c_format hot quantizer ran_ramp xlines \
		a.out core $(LIBFILE)

$(ALL): GraphicsGems.h
