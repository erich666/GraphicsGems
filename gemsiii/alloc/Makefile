# -Aa is HPUX's way of invoking ANSI C
CFLAGS = -g -Aa

alloc.o:	alloc.o alloc.h
	cc $(CFLAGS) -c alloc.c -o alloc.o

clean:
	rm -rf alloc.o
