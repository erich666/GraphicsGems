# -*- Makefile -*-
# based on gnu's make

.SUFFIXES: .cc .h

GPP     = g++
# Compiler options, such as -g or -O :
COPTS   = -O2
# Linker/loader options, such as -s :
LOPTS   = -s
LIBS    = -lm 

MOBJS = main.o polygon.o dedge.o plane.o list.o
SRCS  = ${MOBJS:.o=.cc}

.cc.o:; $(GPP) -c $(COPTS)  $*.cc

ptest: $(MOBJS)
	$(GPP) $(COPTS) $(LOPTS) -o $@ $(MOBJS) $(LIBS)

# ------------------------------------
clean:; $(RM) Makefile.bak *.o *~ a.out core errs TAGS

depend: 
	/usr/local/X11/makedepend $(SRCS)
# DO NOT DELETE THIS LINE -- make depend depends on it.

main.o: polygon.h list.h plane.h vector.h point.h dedge.h
polygon.o: /usr/include/assert.h polygon.h list.h plane.h vector.h point.h
polygon.o: dedge.h
dedge.o: /usr/include/stdlib.h /usr/include/sys/feature_tests.h
dedge.o: /usr/include/assert.h dedge.h
plane.o: plane.h vector.h point.h /usr/include/assert.h
list.o: list.h
