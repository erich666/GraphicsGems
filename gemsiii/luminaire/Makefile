# code is C++
CFLAGS =

all:	geometry_object.o sphere_luminaire.o triangle_luminaire.o

geometry_object.o:	geometry_object.C geometry_object.h utility.h
	CC $(CFLAGS) -c geometry_object.C -o geometry_object.o -lm

sphere_luminaire.o:	sphere_luminaire.C sphere_luminaire.h utility.h
	CC $(CFLAGS) -c sphere_luminaire.C -o sphere_luminaire.o -lm

triangle_luminaire.o:	triangle_luminaire.C triangle_luminaire.h utility.h
	CC $(CFLAGS) -c triangle_luminaire.C -o triangle_luminaire.o -lm

clean:
	rm -rf geometry_object.o sphere_luminaire.o triangle_luminaire.o
