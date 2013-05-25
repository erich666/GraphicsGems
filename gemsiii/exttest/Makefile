# code is C++
CFLAGS =

ehtest1:	ehtest1.C exthit.o exthit.h
	CC $(CFLAGS) -o ehtest1 ehtest1.C exthit.o

exthit.o:	exthit.C exthit.h
	CC $(CFLAGS) -c exthit.C -o exthit.o

clean:
	rm -rf ehtest1 exthit.o
