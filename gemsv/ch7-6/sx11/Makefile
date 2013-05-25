CC = gcc

PROG=sx11

GCCFLAGS = -fpcc-struct-return
CFLAGS = -O2

INCLUDE = -I/usr/X11R5/include -I../tga

LDLIBS =  -lX11 -ltga -lm 

LDFLAGS = -L/usr/X11R5/lib -L../tga


$(PROG):
	$(CC) $(GCCFLAGS) $(INCLUDE) $(CFLAGS) $(PROG).c \
	    $(LDFLAGS) $(LDLIBS) -o $(PROG) 
