/* transform, clip test, and project a vertex list */

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
register float xx, xy, xz, xw;		/* transformation matrix */
register float yx, yy, yz, yw;
register float zx, zy, zz, zw;
register float wx, wy, wz, ww;
register float xo, yo, zo, wo;		/* output vertex */
register float xs, xt, ys, yt, zs /*zt*/;	/* projection scale and translate */
register float t0, t1, t2, t3, t4, t5;	/* temps */
register float one = 1.0;

/* load up local projection matrix */
xs = *(prj+0); xt = *(prj+1);
ys = *(prj+2); yt = *(prj+3);
zs = *(prj+4); /* zt = *(prj+5); later */

/* load up local transform matrix */
xx = *(mtx+0 ); xy = *(mtx+1 ); xz = *(mtx+2 ); xw = *(mtx+3 );
yx = *(mtx+4 ); yy = *(mtx+5 ); yz = *(mtx+6 ); yw = *(mtx+7 );
zx = *(mtx+8 ); zy = *(mtx+9 ); zz = *(mtx+10); zw = *(mtx+11);
wx = *(mtx+12); wy = *(mtx+13); wz = *(mtx+14); ww = *(mtx+15);

/* initialize accumulated flags */
flag_or = 0;
flag_and = -1;

do {
#define wi	t4
	wi = vpi[3];	/* wi */
#define xi	t5
	xi = vpi[0];	/* xi */

	/* calculate 4x4 transform, use as few regs as possible */
	wo = wi*ww; xo = wi*wx; yo = wi*wy; zo = wi*wz;
#define yi	t4
	yi = vpi[1];	/* yi */

	t0 = xi*xw; t1 = xi*xx; t2 = xi*xy; t3 = xi*xz;
#define zi	t5
	zi = vpi[2];	/* zi */
	wo += t0; xo += t1; yo += t2; zo += t3;

	t0 = yi*yw; t1 = yi*yx; t2 = yi*yy; t3 = yi*yz;
	wo += t0; xo += t1; yo += t2; zo += t3;

	t0 = zi*zw; t1 = zi*zx; t2 = zi*zy; t3 = zi*zz;
	wo += t0; xo += t1; yo += t2; zo += t3;

#define winv	t0
	winv = one / wo;

#define zt	t1
	/* ready for this now */
	zt =	prj[5];

	/* let's try branches for comparison purposes */
	flag = 0;

#define CXMAX	1
#define CXMIN	2
#define CYMAX	4
#define CYMIN	8
#define CZMAX	16
#define CZMIN 32

	if (xo >  wo) flag |= CXMAX;
	if (xo < -wo) flag |= CXMIN;
	if (yo >  wo) flag |= CYMAX;
	if (yo < -wo) flag |= CYMIN;
	if (zo >  wo) flag |= CZMAX;
	if (zo < -wo) flag |= CZMIN;

	flag_or |= flag;
	flag_and &= flag;

	xo *= xs;
	yo *= ys;
	zo *= zs;

	xo *= winv;
	yo *= winv;
	zo *= winv;

	xo += xt;
	yo += yt;
	zo += zt;

	*(vpo+0) = xo;
	*(vpo+1) = yo;
	*(vpo+2) = zo;
	*pf = flag;

	vpi += visiz;
	vpo += vosiz;
	pf++;
} while (--vc > 0);

*(pa+0) = flag_or;
*(pa+1) = flag_and;

return;
}
