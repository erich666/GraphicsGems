CFLAGS = -O
LIBES = -lm
PROG = peano

all: $(PROG) mapply

# compile main program
$(PROG): peano.o main.o 
	cc peano.o main.o $(CFLAGS) -o $(PROG) $(LIBES)

# dependencies on #include files
peano.o main.o : types.h

mapply: mapply.c types.h
	cc mapply.c $(CFLAGS) -o mapply $(LIBES)


clean:
	/bin/rm -f peano.o main.o peano mapply
