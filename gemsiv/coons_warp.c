/*
 * ANSI C code from the article
 * "Bilinear Coons Patch Image Warping"
 * by Paul S. Heckbert, ph@cs.cmu.edu
 * in "Graphics Gems IV", Academic Press, 1994
 *
 *
 * This code has been written in the most portable way possible.
 * It will not compile and run without some modifications.
 * You'll need to:
 *	write pixel_read and pixel_write subroutines
 *	read or compute a source image
 *	read or compute four boundary curves
 *	pick a destination image size
 *	call resample() to resample the boundary curves to the width and
 *	    height of the destination image
 *	call coons_warp() to write the destination image
 *	display the warped destination image
 */

#include <math.h>
#include <assert.h>
#define ALLOC(ptr, type, n)  assert(ptr = (type *)malloc((n)*sizeof(type)))

typedef struct {	/* 2-D POINT OR VECTOR */
    float x, y;
} Point2f;

typedef struct {	/* A CURVE DEFINED BY A SEQUENCE OF POINTS */
    int npt;		/* number of points */
    Point2f *pt;	/* array of npt points */
} Curve;

#define SHIFT 20	/* number of fractional bits in fixed point coords */
#define SCALE (1<<SHIFT)

typedef struct {	/* INTEGER POINT AND VECTOR */
    int px, py;		/* position */
    int dx, dy;		/* incremental displacement */
} Ipoint;


/*
 * coons_warp: warps the picture in source image into a rectangular
 * destination image according to four boundary curves, using a
 * bilinear Coons patch.
 * bound[0] through bound[3] are the top, right, bottom, and left
 * boundary curves, respectively, clockwise from upper left.
 *   (These comments are written assuming that y points down on your
 *   frame buffer. Otherwise, bound should proceed CCW from lower left.)
 * The lengths of bound[0] and bound[2] are assumed to be the width of
 * the destination rectangle, and the lengths of bound[1] and bound[3]
 * are assumed to be the height of the rectangle.
 * The upper left corner of the destination rectangle is (u0,v0).
 *
 * Paul Heckbert	25 Feb 82, 15 Oct 93
 */

void coons_warp(Pic *source, Pic *dest, Curve *bound, int u0, int v0) {
    register Ipoint *pu;
    register int u, x, y, qx, qy, dqx, dqy;
    int nu, nv, v;
    float du, dv, fv;
    Point2f p00, p01, p10, p11, *pu0, *pu1, *p0v, *p1v;
    Ipoint *pua;

    nu = bound[0].npt-1;			/* nu = dest_width-1 */
    nv = bound[1].npt-1;			/* nv = dest_height-1 */
    assert(bound[2].npt==nu+1);
    assert(bound[3].npt==nv+1);

    pu0 = &bound[0].pt[0];			/* top boundary curve */
    p1v = &bound[1].pt[0];			/* right */
    pu1 = &bound[2].pt[nu];			/* bottom */
    p0v = &bound[3].pt[nv];			/* left */
    /* arrays pu1 and p0v are in the reverse of the desired order,
       running from right to left and bottom to top, resp., so we
       index them with negative subscripts from their ends (yeeeha!) */

    p00.x = (p0v[ 0].x + pu0[  0].x)/2.;	/* upper left patch corner */
    p00.y = (p0v[ 0].y + pu0[  0].y)/2.;
    p10.x = (pu0[nu].x + p1v[  0].x)/2.;	/* upper right */
    p10.y = (pu0[nu].y + p1v[  0].y)/2.;
    p11.x = (p1v[nv].x + pu1[-nu].x)/2.;	/* lower right */
    p11.y = (p1v[nv].y + pu1[-nu].y)/2.;
    p01.x = (pu1[ 0].x + p0v[-nv].x)/2.;	/* lower left */
    p01.y = (pu1[ 0].y + p0v[-nv].y)/2.;

    du = 1./nu;
    dv = 1./nv;

    ALLOC(pua, Ipoint, nu+1);
    for (pu=pua, u=0; u<=nu; u++, pu++) {
	pu->dx = (pu1[-u].x - pu0[u].x)*dv*SCALE + .5;
	pu->dy = (pu1[-u].y - pu0[u].y)*dv*SCALE + .5;
	pu->px = pu0[u].x*SCALE + .5;
	pu->py = pu0[u].y*SCALE + .5;
    }

    for (fv=0., v=0; v<=nv; v++, fv+=dv) {
	qx = (p0v[-v].x - (1.-fv)*p00.x - fv*p01.x + .5)*SCALE + .5;
	qy = (p0v[-v].y - (1.-fv)*p00.y - fv*p01.y + .5)*SCALE + .5;
	dqx = (p1v[v].x - p0v[-v].x - (1.-fv)*(p10.x-p00.x) - fv*(p11.x-p01.x))
		*du*SCALE + .5;
	dqy = (p1v[v].y - p0v[-v].y - (1.-fv)*(p10.y-p00.y) - fv*(p11.y-p01.y))
		*du*SCALE + .5;
	for (pu=pua, u=0; u<=nu; u++, pu++) {
	    x = pu->px+qx >> SHIFT;
	    y = pu->py+qy >> SHIFT;
	    pixel_write(dest, u0+u, v0+v, pixel_read(source, x, y));
	    qx += dqx;
	    qy += dqy;
	    pu->px += pu->dx;
	    pu->py += pu->dy;
	}
    }
    free(pua);
}

/* the following routine is the slow way to do a Coons warp, for reference */

void coons_warp1(Pic *source, Pic *dest, Curve *bound, int u0, int v0) {
    int nu, nv, u, v, x, y;
    float fu, fv;
    Point2f p00, p01, p10, p11, *pu0, *pu1, *p0v, *p1v;

    nu = bound[0].npt-1;
    nv = bound[1].npt-1;

    pu0 = &bound[0].pt[0];			/* top boundary curve */
    p1v = &bound[1].pt[0];			/* right */
    pu1 = &bound[2].pt[nu];			/* bottom */
    p0v = &bound[3].pt[nv];			/* left */

    p00.x = (p0v[ 0].x + pu0[  0].x)/2.;	/* upper left patch corner */
    p00.y = (p0v[ 0].y + pu0[  0].y)/2.;
    p10.x = (pu0[nu].x + p1v[  0].x)/2.;	/* upper right */
    p10.y = (pu0[nu].y + p1v[  0].y)/2.;
    p11.x = (p1v[nv].x + pu1[-nu].x)/2.;	/* lower right */
    p11.y = (p1v[nv].y + pu1[-nu].y)/2.;
    p01.x = (pu1[ 0].x + p0v[-nv].x)/2.;	/* lower left */
    p01.y = (pu1[ 0].y + p0v[-nv].y)/2.;

    for (v=0; v<=nv; v++) {
	fv = (float)v/nv;
	for (u=0; u<=nu; u++) {
	    fu = (float)u/nu;
	    x =	 (1.-fv)*pu0[ u].x + fv*pu1[-u].x
	       + (1.-fu)*p0v[-v].x + fu*p1v[ v].x
	       - (1.-fu)*(1.-fv)*p00.x - fu*(1.-fv)*p10.x
	       - (1.-fu)*   fv	*p01.x - fu*   fv  *p11.x + .5;
	    y =	 (1.-fv)*pu0[ u].y + fv*pu1[-u].y
	       + (1.-fu)*p0v[-v].y + fu*p1v[ v].y
	       - (1.-fu)*(1.-fv)*p00.y - fu*(1.-fv)*p10.y
	       - (1.-fu)*   fv	*p01.y - fu*   fv  *p11.y + .5;
	    pixel_write(dest, u0+u, v0+v, pixel_read(source, x, y));
	}
    }
}

/* resample_nonuniform: non-uniformly resample curve a to create curve b,
 * with n points.  Allocates b->pt to have length n */

void resample_nonuniform(Curve *a, Curve *b, int n) {
    int ai, bi;
    double ax, af;
    Point2f *ap, *bp;

    if (n<2) {
	fprintf(stderr, "Only %d point in new curve\n", n);
	exit(1);
    }
    ALLOC(b->pt, Point2f, n);
    b->npt = n;
    for (bp=b->pt, bi=0; bi<n; bi++, bp++) {
	ax = (float)bi*(a->npt-1)/(n-1);
	ai = ax;
	af = ax-ai;
	ap = &a->pt[ai];
	bp->x = ap[0].x + af*(ap[1].x-ap[0].x);
	bp->y = ap[0].y + af*(ap[1].y-ap[0].y);
    }
}

static double len(double x, double y) {return sqrt(x*x+y*y);}

/* resample_uniform: uniformly resample curve a to create curve b,
 * with n points.  Allocates b->pt to have length n */

void resample_uniform(Curve *a, Curve *b, int n) {
    int i;
    double step, l, d;
    Point2f *ap, *bp;

    if (a->npt<2) {
	fprintf(stderr, "Only %d point in curve\n", a->npt);
	exit(1);
    }
    for (step=0., ap=a->pt, i=a->npt-1; i>0; i--, ap++)
	step += len(ap[1].x-ap[0].x, ap[1].y-ap[0].y);
    step /= n-1;		/* length of each output segment (ideally) */
    ALLOC(b->pt, Point2f, n);
    b->npt = n;
    d = .0001;			/* = 0 + tolerance for roundoff error */
    for (ap=a->pt, bp=b->pt, i=a->npt-1; i>0; i--, ap++) {
	l = len(ap[1].x-ap[0].x, ap[1].y-ap[0].y);
	d += l;
	/* d is the remaining length of the line segment from ap[0] to ap[1]
	 * that needs to be subdivided into segments of length step */
	while (d>0.) {
	    bp->x = ap[1].x - d/l*(ap[1].x-ap[0].x);
	    bp->y = ap[1].y - d/l*(ap[1].y-ap[0].y);
	    bp++;
	    d -= step;
	}
    }
    if (bp-b->pt != n)
	printf("WARNING: requested %d points, created %d, d=%g\n",
	    n, bp-b->pt, d);
}

static void resample(Curve *a, Curve *b, int n, int nonuniform) {
    if (nonuniform) resample_nonuniform(a, b, n);
    else resample_uniform(a, b, n);
}

Curve *boundary_resample(Curve *a, int nx, int ny, int nonuniform) {
    /* "nonuniform" is a boolean flag */
    Curve *b;

    ALLOC(b, Curve, 4);
    resample(&a[0], &b[0], nx, nonuniform);
    resample(&a[1], &b[1], ny, nonuniform);
    resample(&a[2], &b[2], nx, nonuniform);
    resample(&a[3], &b[3], ny, nonuniform);
    return b;
}
