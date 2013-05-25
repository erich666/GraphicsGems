#
# Makefile
#
#	command file for make to compile the solver.

solve: main.o sturm.o util.o
	cc -o solve main.o sturm.o util.o -lm

clean:
	/bin/rm -f main.o sturm.o util.o solve

main.o sturm.o util.o: solve.h
