CFLAGS = -g -I..

all:	matrix.o viewcorr.o viewfind.o

matrix.o:	matrix.o matrix.h
		cc $(CFLAGS) -c matrix.c -o matrix.o

viewcorr.o:	viewcorr.o matrix.h viewcorr.h
		cc $(CFLAGS) -c viewcorr.c -o viewcorr.o

viewfind.o:	viewfind.o matrix.h viewcorr.h
		cc $(CFLAGS) -c viewfind.c -o viewfind.o

clean:
		/bin/rm -f matrix.o viewcorr.o viewfind.o
