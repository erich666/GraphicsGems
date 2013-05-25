#
# NeXT Makefile for test (a program to test the algebra3 C++ routines)
# Requires libg++ and a c++ compiler. See README file
#

CC = cc++
CFLAGS = -g -I/usr/local/lib/g++-include -L/usr/local/lib
LIBS = -lg++
OBJS = algebra3.o Camera.o Light.o Object3D.o Polyhedron.o Primitive.o \
       Scene3D.o solver.o Sphere.o main.o

.c.o:
	$(CC) -c $@ $(CFLAGS) $*.c
ray: $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS) $(LIBS)
