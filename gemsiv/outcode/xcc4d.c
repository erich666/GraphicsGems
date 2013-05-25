/* Time xform, clip checking, lighting only */

#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <sys/time.h>

/* BSD timer macros -- replace with your own */
struct timeval start, stop;

#define START	gettimeofday(&start, NULL)
#define STOP	gettimeofday(&stop, NULL)
#define DELTAT	((float)(stop.tv_sec - start.tv_sec)+1.0e-6*	\
		 (float)(stop.tv_usec-start.tv_usec))

#define LOOPCOUNT 10000

#define VSIZ 4 /* vertex size */

#define XSIZ 1280
#define YSIZ 1024
#define ZSIZ 32768

/* clip test bit flags */
#define CXMAX 1
#define CXMIN 2
#define CYMAX 4
#define CYMIN 8
#define CZMAX 16
#define CZMIN 32

#define VERTS 12	/* number of points in primitive */

typedef float verttype[VSIZ];

/* 100 pixel triangles in a triangle strip */
#define X	14.0
verttype vtxm[] = {	/* model triangle strip points */
	{ 0.0, 0.0 + X*0, 1.0, 100.0 },
	{ X,   X/2 + X*0, 1.0, 100.0 },
	{ 0.0, 0.0 + X*1, 1.0, 100.0 },
	{ X,   X/2 + X*1, 1.0, 100.0 },
	{ 0.0, 0.0 + X*2, 1.0, 100.0 },
	{ X,   X/2 + X*2, 1.0, 100.0 },
	{ 0.0, 0.0 + X*3, 1.0, 100.0 },
	{ X,   X/2 + X*3, 1.0, 100.0 },
	{ 0.0, 0.0 + X*4, 1.0, 100.0 },
	{ X,   X/2 + X*4, 1.0, 100.0 },
	{ 0.0, 0.0 + X*5, 1.0, 100.0 },
	{ X,   X/2 + X*5, 1.0, 100.0 }, };

/*
 * Transform/clip check test program
 *
 * takes the following command line options:
 *
 *	-m MHz	 : set MHz for clock calculation
 *	-c count : loop counter (defaults to 10000)
 *
 */

main( argc, argv)
int argc; char *argv[];
{
int MHz = 40;
int i;
float mtx[16], proj[6];
float vtxw[VERTS+1][VSIZ-1];	/* output xyz device coords */
int vflg[VERTS];
int tflg[2];
int loopcount = LOOPCOUNT;

/* read command line options */
	while (--argc){
	  if ((*++argv)[0] == '-'){
		switch((*argv)[1]){
		  case 'm': MHz = atoi((*++argv));
			    --argc;
			    break;
		  case 'c': loopcount = atoi((*++argv));
			    --argc;
			    break;
		}
	  }
	}

/* set up transform and projection matrices */

	/* identity matrix */
	for (i=0; i<16; i++) mtx[i] = 0;
	mtx[0] = mtx[5] = mtx[10] = mtx[15] =  1.0;

	/* projection values */
	proj[0] = 100.0;
	proj[1] = 0.0;
	proj[2] = 100.0;
	proj[3] = 0.0;
	proj[4] = 100.0;
	proj[5] = 0.0;

/* start the operation */
	START;

	for (i=0; i < loopcount; i++) {
		xform_ctp(VERTS,vtxm,vtxw,VSIZ,VSIZ-1,mtx,proj,vflg,tflg);
	}

	STOP;

	printf("%d points in %g seconds\n", i*VERTS, DELTAT);
	printf("%g clocks/point at %d MHz\n", DELTAT/(i*VERTS)*MHz*1.0e6, MHz);
}
