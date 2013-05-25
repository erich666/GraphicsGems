# Valid options: -DTIMER -DDISPLAY -DCONVEX -DHYBRID -DSORT -DRANDOM -DWINDING
# You can define $MAKEOPTS outside the program for testing purposes.
#
# for testing purposes we leave all options off and invoke through $MAKEOPTS
CCFLAGS=-O
# good default if not testing:
# CCFLAGS=-O -DTIMER -DDISPLAY -DSORT -DRANDOM


# use this one for making one which will display the tests being done on an HP
# and do:
# export LDOPTS="-a shared"

p_test:		ptinpoly.o ptinpoly.h p_test.c
		cc -o p_test p_test.c $(CCFLAGS) $(MAKEOPTS) ptinpoly.o -lm
# include these lines for linking in HP Starbase, for display version
#			-L /usr/lib/X11R4 \
#			-lXwindow \
#			-lsb \
#			-lXhp11 -lX11 -ldld \
#			-lm

ptinpoly.o:	ptinpoly.c ptinpoly.h
		cc -o ptinpoly.o -c ptinpoly.c $(CCFLAGS) $(MAKEOPTS)

clean:
		rm -f ptinpoly.o p_test
