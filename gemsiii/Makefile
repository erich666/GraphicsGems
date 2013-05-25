# Makefile for Graphics Gems III source
#
# Eric Haines, 10/92
#
# This make file will build "gemslib3.a" and a number of executables.
# Gemslib3 is built solely for debugging purposes -- it is not intended
# to be used as a library.
#
# Some code uses ANSI headers, some doesn't, so you may have to mess with
# CFLAGS, depending.
#
# Note that some of the gems need additional macros, functions, tables
# driving routines, etc. before they will compile or run properly.
# These include:
#
# accurate_scan - the test program uses HP's Starbase graphics API
#
# bsp.c - need routines for FirstOfLinkList, NextOfLinkList, AddToLinkList,
#	and code for RayBoxIntersect, RayObjIntersect
#
# cyclic.c - note that this file is simply a set of macros, no code is compiled
#
# luminaire - need function "hit()"
#
# panorama.c - needs Rayshade include files.
#
# simplex - need function "bitCount()"

#
# C compiler flags
#
CFLAGS =
#
# Location of Graphics Gems library
#
LIBFILE = gemslib3.a

#
# Graphics Gems II Vector Library
#
VECLIB = GraphicsGems.o

MFLAGS = "LIBFILE = ../$(LIBFILE)" "GENCFLAGS = $(CFLAGS)"

SHELL = /bin/sh

OFILES = 3d.o PIR.o Polyintr.o accForm.o bitmap.o \
	bounding_volumes.o bsp.o bzrinter.o circlexc.o con2d.o contour.o \
	edgeCalc.o fastBitmap.o fastLinear.o fastSpan.o fillet.o filter.o \
	forfac.o hemis.o insectc.o intell.o intqdr.o motblur.o ndline.o \
	newell.o parelarc.o pl2plane.o planeSets.o pt2plane.o \
	quatspin.o rand_rotation.o rgbvary.o scallops8.o \
	sqfinal.o sqrt.o triangleCube.o urot.o zdepth.o

DIRS = accurate_scan alloc exttest luminaire partition3d simplex

ALL =	contour filter forfac scallops8 sqfinal $(LIBFILE)

all: $(ALL)
	@for d in $(DIRS) ; do \
		(cd $$d ; $(MAKE) $(MFLAGS)) ;\
	done

$(LIBFILE): $(OFILES) $(VECLIB)
	ar rcs $(LIBFILE) $(OFILES) $(VECLIB)

contour: contour.o
	$(CC) $(CFLAGS) -o $@ contour.o

filter: filter.o
	$(CC) $(CFLAGS) -o $@ filter.o -lm

forfac: forfac.o
	$(CC) $(CFLAGS) -o $@ forfac.o -lm

scallops8: scallops8.o
	$(CC) $(CFLAGS) -o $@ scallops8.o

sqfinal: sqfinal.o
	$(CC) $(CFLAGS) -o $@ sqfinal.o -lm

clean:
	@for d in $(DIRS) ; do \
		(cd $$d ; $(MAKE) $(MFLAGS) clean) ;\
	done
	/bin/rm -f $(OFILES) $(VECLIB)
	/bin/rm -f 3d.o PIR.o Polyintr.o accForm.o bitmap.o \
		bounding_volumes.o bsp.o bzrinter.o circlexc.o con2d.o \
		contour.o edgeCalc.o fastBitmap.o fastLinear.o fastSpan.o \
		fillet.o filter.o forfac.o hemis.o insectc.o intell.o \
		intqdr.o motblur.o ndline.o newell.o parelarc.o \
		pl2plane.o planeSets.o pt2plane.o quatspin.o rand_rotation.o \
		rgbvary.o scallops8.o sqfinal.o sqrt.o \
		triangleCube.o urot.o zdepth.o \
		contour filter forfac scallops8 sqfinal \
		a.out core $(LIBFILE)

$(ALL): GraphicsGems.h
