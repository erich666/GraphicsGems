CFLAGS = -I..

main:	main.c partition.o partition.h
	cc $(CFLAGS) -o main main.c partition.o -lm

partition.o:	partition.c partition.h
	cc $(CFLAGS) -c partition.c -o partition.o

clean:
	rm -rf main partition.o
