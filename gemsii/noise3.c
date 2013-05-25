/* Copyright (c) 1988 Regents of the University of California */

#ifndef lint
static char SCCSid[] = "@(#)noise3.c 2.1 11/12/91 LBL";
#endif

/*
 *  noise3.c - noise functions for random textures.
 *
 *     Credit for the smooth algorithm goes to Ken Perlin.
 *     (ref. SIGGRAPH Vol 19, No 3, pp 287-96)
 *
 *     4/15/86
 *     5/19/88	Added fractal noise function
 */


#define  A		0
#define  B		1
#define  C		2
#define  D		3

#define  rand3a(x,y,z)	frand(67*(x)+59*(y)+71*(z))
#define  rand3b(x,y,z)	frand(73*(x)+79*(y)+83*(z))
#define  rand3c(x,y,z)	frand(89*(x)+97*(y)+101*(z))
#define  rand3d(x,y,z)	frand(103*(x)+107*(y)+109*(z))

/* hermite */
#define  hpoly1(t)	((2.0*t-3.0)*t*t+1.0)
#define  hpoly2(t)	(-2.0*t+3.0)*t*t
#define  hpoly3(t)	((t-2.0)*t+1.0)*t
#define  hpoly4(t)	(t-1.0)*t*t

double  *noise3(), fnoise3(), frand();

static  interpolate();

static long  xlim[3][2];
static double  xarg[3];

#define  EPSILON	.0001		/* error allowed in fractal */

#define  frand3(x,y,z)	frand(17*(x)+23*(y)+29*(z))


double *
noise3(xnew)			/* compute the noise function */
register double  xnew[3];
{
	extern double  floor();
	static double  x[3] = {-100000.0, -100000.0, -100000.0};
	static double  f[4];

	if (x[0]==xnew[0] && x[1]==xnew[1] && x[2]==xnew[2])
		return(f);
	x[0] = xnew[0]; x[1] = xnew[1]; x[2] = xnew[2];
	xlim[0][0] = floor(x[0]); xlim[0][1] = xlim[0][0] + 1;
	xlim[1][0] = floor(x[1]); xlim[1][1] = xlim[1][0] + 1;
	xlim[2][0] = floor(x[2]); xlim[2][1] = xlim[2][0] + 1;
	xarg[0] = x[0] - xlim[0][0];
	xarg[1] = x[1] - xlim[1][0];
	xarg[2] = x[2] - xlim[2][0];
	interpolate(f, 0, 3);
	return(f);
}


static
interpolate(f, i, n)
double  f[4];
register int  i, n;
{
	double  f0[4], f1[4], hp1, hp2;

	if (n == 0) {
		f[A] = rand3a(xlim[0][i&1],xlim[1][i>>1&1],xlim[2][i>>2]);
		f[B] = rand3b(xlim[0][i&1],xlim[1][i>>1&1],xlim[2][i>>2]);
		f[C] = rand3c(xlim[0][i&1],xlim[1][i>>1&1],xlim[2][i>>2]);
		f[D] = rand3d(xlim[0][i&1],xlim[1][i>>1&1],xlim[2][i>>2]);
	} else {
		n--;
		interpolate(f0, i, n);
		interpolate(f1, i | 1<<n, n);
		hp1 = hpoly1(xarg[n]); hp2 = hpoly2(xarg[n]);
		f[A] = f0[A]*hp1 + f1[A]*hp2;
		f[B] = f0[B]*hp1 + f1[B]*hp2;
		f[C] = f0[C]*hp1 + f1[C]*hp2;
		f[D] = f0[D]*hp1 + f1[D]*hp2 +
				f0[n]*hpoly3(xarg[n]) + f1[n]*hpoly4(xarg[n]);
	}
}


double
frand(s)			/* get random number from seed */
register long  s;
{
	s = s<<13 ^ s;
	return(1.0-((s*(s*s*15731+789221)+1376312589)&0x7fffffff)/1073741824.0);
}


double
fnoise3(p)			/* compute fractal noise function */
double  p[3];
{
	double  floor();
	long  t[3], v[3], beg[3];
	double  fval[8], fc;
	int  branch;
	register long  s;
	register int  i, j;
						/* get starting cube */
	s = (long)(1.0/EPSILON);
	for (i = 0; i < 3; i++) {
		t[i] = s*p[i];
		beg[i] = s*floor(p[i]);
	}
	for (j = 0; j < 8; j++) {
		for (i = 0; i < 3; i++) {
			v[i] = beg[i];
			if (j & 1<<i)
				v[i] += s;
		}
		fval[j] = frand3(v[0],v[1],v[2]);
	}
						/* compute fractal */
	for ( ; ; ) {
		fc = 0.0;
		for (j = 0; j < 8; j++)
			fc += fval[j];
		fc *= 0.125;
		if ((s >>= 1) == 0)
			return(fc);		/* close enough */
		branch = 0;
		for (i = 0; i < 3; i++) {	/* do center */
			v[i] = beg[i] + s;
			if (t[i] > v[i]) {
				branch |= 1<<i;
			}
		}
		fc += s*EPSILON*frand3(v[0],v[1],v[2]);
		fval[~branch & 7] = fc;
		for (i = 0; i < 3; i++) {	/* do faces */
			if (branch & 1<<i)
				v[i] += s;
			else
				v[i] -= s;
			fc = 0.0;
			for (j = 0; j < 8; j++)
				if (~(j^branch) & 1<<i)
					fc += fval[j];
			fc = 0.25*fc + s*EPSILON*frand3(v[0],v[1],v[2]);
			fval[~(branch^1<<i) & 7] = fc;
			v[i] = beg[i] + s;
		}
		for (i = 0; i < 3; i++) {	/* do edges */
			j = (i+1)%3;
			if (branch & 1<<j)
				v[j] += s;
			else
				v[j] -= s;
			j = (i+2)%3;
			if (branch & 1<<j)
				v[j] += s;
			else
				v[j] -= s;
			fc = fval[branch & ~(1<<i)];
			fc += fval[branch | 1<<i];
			fc = 0.5*fc + s*EPSILON*frand3(v[0],v[1],v[2]);
			fval[branch^1<<i] = fc;
			j = (i+1)%3;
			v[j] = beg[j] + s;
			j = (i+2)%3;
			v[j] = beg[j] + s;
		}
		for (i = 0; i < 3; i++)		/* new cube */
			if (branch & 1<<i)
				beg[i] += s;
	}
}

