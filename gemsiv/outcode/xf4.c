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

/* flpt registers, total 32 */
register float xx, xy, xt;	/* transformation matrix */
register float yx, yy, yt;
register float xo, yo;		/* output vertex */
register float xi, yi;		/* temps */

/* load up local transform matrix */
xx = *(mtx+0 ); xy = *(mtx+1 ); xt = *(mtx+2 );
yx = *(mtx+4 ); yy = *(mtx+5 ); yt = *(mtx+6 );

/* initialize accumulated flags */
flag_or = 0;
flag_and = -1;

do {
	xi = vpi[0];	/* xi */
	yi = vpi[1];	/* yi */

	xo = xi * xx + yi * xy + xt;
	yo = xi * yx + yi * yy + yt;

	/* let's try branches for comparison purposes */
	flag = 0;

#define CXMIN	1
#define CXMAX	2
#define CYMIN	4
#define CYMAX	8

#define XMIN	0
#define XMAX	1279
#define YMIN	0
#define YMAX	1023

	if (xo < XMIN) flag |= CXMIN;
	if (xo > XMAX) flag |= CXMAX;
	if (yo < YMIN) flag |= CYMIN;
	if (yo > YMAX) flag |= CYMAX;

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
