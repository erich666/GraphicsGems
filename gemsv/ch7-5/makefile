inclpath = .

CC=gcc

CFLAGS= -UCHOOSE_MANUAL -UDEBUG -USIMPLE -DSTANDALONE -UCLOCK\
	-I$(inclpath) -L/lib/pa1.1 -g 


# DEBUG: turn on debugging output
# CLOCK: time the triangulation process 
#
# STANDALONE: run as a separate program. read data from file.
#	      If this flag is False, then use the interface procedure 
#	      triangulate_polygon() instead.
#
# SIMPLE: if defined, turn off randomization
#


LDFLAGS= -lm

objects= construct.o misc.o monotone.o tri.o
executable = triangulate

$(executable): $(objects)
	rm -f $(executable)
	$(CC) $(CFLAGS) $(objects) $(LDFLAGS) -o $(executable)

$(objects): $(inclpath)/basic.h

clean:
	rm -f $(objects)

