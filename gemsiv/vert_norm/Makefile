CC = cc
CFLAGS = -I/usr/ph/include -DSTANDALONE_TEST

test: test.o smooth.o
	$(CC) -o test test.o smooth.o -lm

clean:
	rm -f *.o test

smooth.o: smooth.h
test.o: smooth.h
