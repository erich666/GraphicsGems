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
register int ti0, s, t, u;
register int sign = 0x80000000;

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

	{
	/* put values on stack so we can get them as integers */
	float sxo, syo, szo, swo;	/* stacked output values */

	swo = wo;
	sxo = xo;
	syo = yo;
	szo = zo;

#define zt	t1
	/* ready for this now */
	zt =	prj[5];

	/* interleave clip checking with projection to DC */
	xo *= xs;
	yo *= ys;
	zo *= zs;

	/* branch-free clip checker*/
	ti0 = FI(swo);
	s = ti0 & ~sign;	  /* abs(w) */

	ti0 = FI(sxo);
	t = ti0 & ~sign;	  /* abs(x) */
	u = s - t;		/* compare to abs(w) */
	u = (unsigned int)u >> 31; /* extract sign, 1 is abs(x) > abs(w) */
	t = (unsigned int)ti0 >> 31; /* x sign */
	flag = u << t;		 /* bit 0: x pos bit 1: x neg */

	xo *= winv;
	yo *= winv;
	zo *= winv;

	ti0 = FI(syo);
	t = ti0 & ~sign;	  /* abs(y) */
	u = s - t;		/* compare to w */
	u = (unsigned int)u >> 31;	/* extract sign, 1 is abs(y) > w */
	t = (unsigned int)ti0 >> 31;	  /* y sign */
	t += 2;
	t = u << t;		/* bit 2: y pos bit 3: y neg */
	flag |= t;

	xo += xt;
	yo += yt;
	zo += zt;

	ti0 = FI(szo);
	t = ti0 & ~sign;	  /* abs(z) */
	u = s - t;		/* compare to w */
	u = (unsigned int)u >> 31;	/* extract sign, 1 is abs(z) > w */
	t = (unsigned int)ti0 >> 31;	  /* z sign */
	t += 4;
	t = u << t;		/* bit 4: z pos bit 5: z neg */
	flag |= t;

	}

	flag_or |= flag;
	flag_and &= flag;

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
