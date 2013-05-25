CFLAGS = -g

tester : test.c bit32.o
	$(CC) $(CFLAGS) test.c -o tester bit32.o

bit32.o : bit32.c
	$(CC) -c $(CFLAGS) bit32.c -o bit32.o

clean:
	/bin/rm -f bit32.o tester
