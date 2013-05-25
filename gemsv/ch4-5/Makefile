timing:
	cc -O -pca -c ellipsoid_par_help.c
	cc -O -c ellipsoid.c
	cc -O -c timing.c
	cc -O -pca -o timing ellipsoid_par_help.o ellipsoid.o timing.o -lsphere -lmalloc -lgl_s -lm -s
