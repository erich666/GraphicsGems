delaunay: test.o quadedge.o
	CC -o delaunay test.o quadedge.o -lgl_s -lX11 -lm

test.o: geom2d.h quadedge.h test.C
	CC -I. -c test.C

quadedge.o: geom2d.h quadedge.h quadedge.C
	CC -I. -c quadedge.C
