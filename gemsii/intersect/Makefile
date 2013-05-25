CFLAGS = -g -I..

all:		intsph.o inttor.o

intsph.o:	intsph.c
		cc $(CFLAGS) -c intsph.c -o intsph.o

inttor.o:	inttor.c
		cc $(CFLAGS) -c inttor.c -o inttor.o

clean:
		/bin/rm -f intsph.o inttor.o
