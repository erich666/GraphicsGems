#
# Makefile for Graphics Gems source
#
# Craig Kolb, 8/90
#
# This make file will build "gemslib.a" and a number of executables.
# Gemslib is built solely for debugging purposes -- it is not intended
# to be used as a library.
#
# Note that some of the gems need additional macros, functions, tables
# driving routines, etc. before they will compile or run properly.
# These include:
#
# AALines
#	Needs variables set in the Makefile.
# Dissolve
#	Needs a table filled.
# FitCurves
#	Needs a functioning DrawBezierCurve().
# MatrixOrtho
#	Needs a number of matrix routines.
# PolyScan
#	Needs driving routines and other additional code.
# RayPolygon
#	Needs surrounding function structure, declaration of
#	variable types, etc.

#
# C compiler flags
#
CFLAGS = -g
#
# Location of Graphics Gems library
#
LIBFILE = gemslib.a

#
# Graphics Gems Vector Library
#
VECLIB = GGVecLib.o

MFLAGS = "LIBFILE = ../$(LIBFILE)" "GENCFLAGS = $(CFLAGS)"

FTPDIR = /usr/spool/uucppublic/ftp/pub/GraphicsGems/src
ZOOFILE = Gems.zoo

SHELL = /bin/sh

OFILES =	PntOnLine.o ViewTrans.o AAPolyScan.o Albers.o \
		Interleave.o BoundSphere.o BoxSphere.o CircleRect.o \
		ConcaveScan.o Roots3And4.o Dissolve.o DigitalLine.o \
		FastJitter.o FixedTrig.o HSLtoRGB.o HypotApprox.o LineEdge.o \
		MatrixInvert.o MatrixPost.o Median.o PixelInteger.o \
		TriPoints.o Quaternions.o RGBTo4Bits.o RayBox.o \
		SeedFill.o SquareRoot.o DoubleLine.o TransBox.o

DIRS = 2DClip PolyScan Sturm AALines

ALL =	Hash3D FitCurves Forms NearestPoint Label \
	OrderDither BinRec $(LIBFILE)

all: $(ALL)
	@for d in $(DIRS) ; do \
		(cd $$d ; $(MAKE) $(MFLAGS)) ;\
	done

$(LIBFILE): $(OFILES) $(VECLIB)
	ar rcs $(LIBFILE) $(OFILES) $(VECLIB)

Hash3D: Hash3D.o
	$(CC) $(CFLAGS) -o $@ Hash3D.o

FitCurves: FitCurves.o $(VECLIB)
	$(CC) $(CFLAGS) -o $@ FitCurves.o $(VECLIB) -lm

Forms: Forms.o
	$(CC) $(CFLAGS) -o $@ Forms.o

NearestPoint: NearestPoint.o $(VECLIB)
	$(CC) $(CFLAGS) -o $@ NearestPoint.o $(VECLIB) -lm

Label: Label.o
	$(CC) $(CFLAGS) -o $@ Label.o -lm

OrderDither: OrderDither.o
	$(CC) $(CFLAGS) -o $@ OrderDither.o

BinRec: BinRec.o
	$(CC) $(CFLAGS) -o $@ BinRec.o

ftpreadme:
	/bin/rm -f $(FTPDIR)/README
	sed -e "s/__DATE__/`date`/g" < README.ftp > $(FTPDIR)/README

readme:
	/bin/rm -rf README
	sed -e "s/__DATE__/`date`/g" < README.dist > README
	

clean:
	@for d in $(DIRS) ; do \
		(cd $$d ; $(MAKE) $(MFLAGS) clean) ;\
	done
	/bin/rm -f $(OFILES) $(VECLIB)
	/bin/rm -f Hash3D.o FitCurves.o \
		Forms.o NearestPoint.o Label.o OrderDither.o BinRec.o \
		Hash3D FitCurves Forms NearestPoint Label OrderDither BinRec \
		bugs a.out core Part??.Z Part?? $(ZOOFILE) gemslib.a

kit: readme
	/bin/rm -f Part??.Z Part??
	(makekit -iPACKING_LIST -oMANIFEST)

zoo: readme
	/bin/rm -f Gems.zoo
	cut -d" " -f2 PACKING_LIST | zoo aI $(ZOOFILE)

ftp: kit zoo ftpreadme
	compress Part??
	/bin/cp Part??.Z $(ZOOFILE) $(FTPDIR)

$(ALL): GraphicsGems.h
