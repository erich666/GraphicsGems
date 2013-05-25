#
# makefile to build program graph layout
#

.C.o:
	$(CC) -c $(CFLAGS) $<

#
# Define Objects
#
OBJS= window.o graph.o layout.o vector.o fileio.o 

#
# define build flags
#
CC = gcc
INCLUDES= -I/usr/include/X11R4 
CFLAGS = -g $(INCLUDES) 
LDFLAGS=  -L/usr/lib/X11R4  
LIBS= -lX11 -lm

#
# Dependencies
#
all : graph

window.o: window.C window.hxx 

graph.o: graph.C window.hxx vector.hxx defines.h graph.hxx

layout.o: layout.C window.hxx vector.hxx defines.h graph.hxx

vector.o: vector.C vector.hxx

fileio.o: fileio.C window.hxx vector.hxx defines.h graph.hxx fileio.hxx

graph: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@
