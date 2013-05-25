RM = rm -f
COFLAGS= -g

CO=gcc

DEFINES= -DSYSV
CFLAGS= -I/usr/include/X11R4/ -L/usr/lib/X11R4/

OBJECTS = \
	show.o

all: show 

show:	$(OBJECTS)
	cc -g $(OBJECTS) $(CFLAGS) -o show -lX11 

show.o: show.c
	cc -g -c $(CFLAGS) show.c

