# -Aa is HPUX's way of invoking ANSI C
CFLAGS = -g -Aa

all:	draw.o rad.o room.o

draw.o:	draw.o rad.h
	cc $(CFLAGS) -c draw.c -o draw.o

rad.o:	rad.o rad.h
	cc $(CFLAGS) -c rad.c -o rad.o

room.o:	room.o rad.h
	cc $(CFLAGS) -c room.c -o room.o

clean:
	/bin/rm -f draw.o rad.o room.o
