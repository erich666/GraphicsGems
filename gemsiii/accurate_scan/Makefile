# Accurate Polygon Scan Conversion Using Half-Open Intervals,
#     by Kurt Fleischer and David Salesin
#
# don't put any -g or -O here -- it's taken care of elsewhere
# -Ac
CFLAGS= -Aa

OBJ =	 fixpoint.o dblfixpoint.o tri.o

GRAPHICSLIB = -L /usr/lib/X11R4 -lXwindow -lsb -lXhp11 -lX11 -ldld
# non-shared library, non-X version:
#GRAPHICSLIB = -ldd98721 -ldd98731 -lsb1 -lsb2

OBJO= $(OBJ:.o=.oo)
OBJG= $(OBJ:.o=.og)
OBJP= $(OBJ:.o=.op)

default: tri

depend:	;mkmf && ed - makefile < Make.mkmf.ed 

clean:	;rm -f core tri a.out *.o? *.o *~ *.og *.oo *.op

# debugging version
tri:  $(OBJG) test.og
	cc -g -o $@ $(OBJG) test.og $(GRAPHICSLIB) -lm

# optimized version
trio: $(OBJO) test.oo
	cc -O -o $@ $(OBJO) test.oo $(GRAPHICSLIB) -lm

# exhaustive test program (lores)
exhaust: $(OBJO) exhaust.oo
	cc -O -o $@ $(OBJO) exhaust.oo $(GRAPHICSLIB) -lm

# debugging version
dtri: gemfile.og test.og addendum.og
	cc -g -o $@ gemfile.og test.og addendum.og \
		-l

# exhaustive test program (lores)
dexhaust: gemfile.og exhaust.oo
	cc -O -o $@ gemfile.og exhaust.oo addendum.og \
		$(GRAPHICSLIB) -lm


#
# Compilation rules
#
.SUFFIXES:
.SUFFIXES: .op .og .oo .c

.c.og:
	cc -c -g $(CFLAGS) $< && mv -f $*.o $*.og

.c.oo:
	cc -c -O $(CFLAGS) $< && mv -f $*.o $*.oo

.c.op:
	cc -c -O -G $(CFLAGS) $< && mv -f $*.o $*.op


###
dblfixpoint.og: /usr/include/stdio.h /usr/include/sys/stdsyms.h fixpoint.h
exhaust.og: /usr/include/stdio.h /usr/include/sys/stdsyms.h \
	/usr/include/stdlib.h /usr/include/math.h /usr/include/starbase.c.h \
	fixpoint.h
fixpoint.og: /usr/include/stdio.h /usr/include/sys/stdsyms.h \
	/usr/include/stdlib.h /usr/include/math.h fixpoint.h
formatted.tri.og: /usr/include/stdio.h /usr/include/sys/stdsyms.h \
	/usr/include/math.h
gemfile.og: /usr/include/stdio.h /usr/include/sys/stdsyms.h \
	/usr/include/math.h fixpoint.h
test.og: /usr/include/stdio.h /usr/include/sys/stdsyms.h /usr/include/stdlib.h \
	/usr/include/math.h /usr/include/starbase.c.h
tri.og: /usr/include/stdio.h /usr/include/sys/stdsyms.h /usr/include/math.h \
	fixpoint.h
dblfixpoint.oo: /usr/include/stdio.h /usr/include/sys/stdsyms.h fixpoint.h
exhaust.oo: /usr/include/stdio.h /usr/include/sys/stdsyms.h \
	/usr/include/stdlib.h /usr/include/math.h /usr/include/starbase.c.h \
	fixpoint.h
fixpoint.oo: /usr/include/stdio.h /usr/include/sys/stdsyms.h \
	/usr/include/stdlib.h /usr/include/math.h fixpoint.h
formatted.tri.oo: /usr/include/stdio.h /usr/include/sys/stdsyms.h \
	/usr/include/math.h
gemfile.oo: /usr/include/stdio.h /usr/include/sys/stdsyms.h \
	/usr/include/math.h fixpoint.h
test.oo: /usr/include/stdio.h /usr/include/sys/stdsyms.h /usr/include/stdlib.h \
	/usr/include/math.h /usr/include/starbase.c.h
tri.oo: /usr/include/stdio.h /usr/include/sys/stdsyms.h /usr/include/math.h \
	fixpoint.h
dblfixpoint.op: /usr/include/stdio.h /usr/include/sys/stdsyms.h fixpoint.h
exhaust.op: /usr/include/stdio.h /usr/include/sys/stdsyms.h \
	/usr/include/stdlib.h /usr/include/math.h /usr/include/starbase.c.h \
	fixpoint.h
fixpoint.op: /usr/include/stdio.h /usr/include/sys/stdsyms.h \
	/usr/include/stdlib.h /usr/include/math.h fixpoint.h
formatted.tri.op: /usr/include/stdio.h /usr/include/sys/stdsyms.h \
	/usr/include/math.h
gemfile.op: /usr/include/stdio.h /usr/include/sys/stdsyms.h \
	/usr/include/math.h fixpoint.h
test.op: /usr/include/stdio.h /usr/include/sys/stdsyms.h /usr/include/stdlib.h \
	/usr/include/math.h /usr/include/starbase.c.h
tri.op: /usr/include/stdio.h /usr/include/sys/stdsyms.h /usr/include/math.h \
	fixpoint.h
