CC = gcc

PROG=ZRendv10

GCCFLAGS = -fpcc-struct-return
CFLAGS = -ansi -O2 

INCLUDE = -I/usr/X11R5/include -I./tga

LDLIBS =  -lX11 -lm -ltga

LDFLAGS = -L/usr/X11R5/lib -L./tga

$(PROG): $(PROG).c $(PROG).h
	(cd tga; make)
	(cd sx11; make)
	$(CC) $(GCCFLAGS) $(CFLAGS) $(INCLUDE) $(PROG).c \
	$(LDFLAGS) $(LDLIBS) -o $(PROG)

