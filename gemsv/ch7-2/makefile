
CC=CC	# can be C++ or C compiler

OBJS= \
	pcube.o \
	fpcube.o \
	test.o

.c.o:
	$(CC) -O -c $< -I.

all: test

test: $(OBJS)
	$(CC) -o test $(OBJS) -lm

clean:
	rm -f $(OBJS) test
