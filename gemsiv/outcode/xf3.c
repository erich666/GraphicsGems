/* 2D transform and clip test a vertex list */

/* oflo and division traps should be disabled, by the way */

#define FI(x)	(*(int *)&x)

/********************************************************/
xform_ctp(vc, vpi, vpo, visiz, vosiz, mtx, prj, pf, pa)
int vc	;	/* vertex count */
int visiz;	/* input vertex array stride */
int vosiz;	/* output vertex array stride */
int *pf;	/* flag array */
int *pa;	/* 2 element - global or_flag, and_flag */
float *vpi, *vpo, *mtx, *prj;
{
register int flag, flag_or, flag_and;
register int sign = 0x80000000;

/* flpt registers, total 32 */
register float xx, xy, xt;	/* transformation matrix */
register float yx, yy, yt;
float xo, yo;		/* output vertex */
register float xi, yi;		/* temps */

register int ixmin, ixmax, iymin, iymax;
register int ix, ax;

#define XMIN	0
#define XMAX	1279
#define YMIN	0
#define YMAX	1023

float xmin = XMIN, ymin = YMIN, xmax = XMAX, ymax = YMAX;

/* load up local transform matrix */
xx = *(mtx+0 ); xy = *(mtx+1 ); xt = *(mtx+2 );
yx = *(mtx+4 ); yy = *(mtx+5 ); yt = *(mtx+6 );

/* initialize accumulated flags */
flag_or = 0;
flag_and = -1;

ixmin = FI(xmin); iymin = FI(ymin);
ixmax = FI(xmax); iymax = FI(ymax);

do {
	xi = vpi[0];	/* xi */
	yi = vpi[1];	/* yi */

	xo = xi * xx + yi * xy + xt;
	yo = xi * yx + yi * yy + yt;

	ix = FI(xo); ax = ix & ~sign;
	flag = (ix | (ax - ixmin)) >> 31;
	flag |= ((~ix & (ixmax - ax)) >> 31 ) << 1;

	ix = FI(yo); ax = ix & ~sign;
	flag = ((ix | (ax - iymin)) >> 31) << 2;
	flag |= ((~ix & (iymax - ax)) >> 31 ) << 3;

	flag_or |= flag;
	flag_and &= flag;

	*(vpo+0) = xo;
	*(vpo+1) = yo;
	*pf = flag;

	vpi += visiz;
	vpo += vosiz;
	pf++;
} while (--vc > 0);

*(pa+0) = flag_or;
*(pa+1) = flag_and;

return;
}
