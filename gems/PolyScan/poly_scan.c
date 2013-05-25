/*
 * Generic Convex Polygon Scan Conversion and Clipping
 * by Paul Heckbert
 * from "Graphics Gems", Academic Press, 1990
 */

/*
 * poly_scan.c: point-sampled scan conversion of convex polygons
 *
 * Paul Heckbert	1985, Dec 1989
 */

#include <stdio.h>
#include <math.h>
#include "poly.h"

/*
 * poly_scan: Scan convert a polygon, calling pixelproc at each pixel with an
 * interpolated Poly_vert structure.  Polygon can be clockwise or ccw.
 * Polygon is clipped in 2-D to win, the screen space window.
 *
 * Scan conversion is done on the basis of Poly_vert fields sx and sy.
 * These two must always be interpolated, and only they have special meaning
 * to this code; any other fields are blindly interpolated regardless of
 * their semantics.
 *
 * The pixelproc subroutine takes the arguments:
 *
 *	pixelproc(x, y, point)
 *	int x, y;
 *	Poly_vert *point;
 *
 * All the fields of point indicated by p->mask will be valid inside pixelproc
 * except sx and sy.  If they were computed, they would have values
 * sx=x+.5 and sy=y+.5, since sampling is done at pixel centers.
 */

void poly_scan(p, win, pixelproc)
register Poly *p;		/* polygon */
Window *win;			/* 2-D screen space clipping window */
void (*pixelproc)();		/* procedure called at each pixel */
{
    register int i, li, ri, y, ly, ry, top, rem;
    register unsigned long mask;
    double ymin;
    Poly_vert l, r, dl, dr;

    if (p->n>POLY_NMAX) {
	fprintf(stderr, "poly_scan: too many vertices: %d\n", p->n);
	return;
    }
    if (sizeof(Poly_vert)/sizeof(double) > 32) {
	fprintf(stderr, "Poly_vert structure too big; must be <=32 doubles\n");
	exit(1);
    }

    ymin = HUGE;
    for (i=0; i<p->n; i++)		/* find top vertex (y points down) */
	if (p->vert[i].sy < ymin) {
	    ymin = p->vert[i].sy;
	    top = i;
	}

    li = ri = top;			/* left and right vertex indices */
    rem = p->n;				/* number of vertices remaining */
    y = ceil(ymin-.5);			/* current scan line */
    ly = ry = y-1;			/* lower end of left & right edges */
    mask = p->mask & ~POLY_MASK(sy);	/* stop interpolating screen y */

    while (rem>0) {	/* scan in y, activating new edges on left & right */
			/* as scan line passes over new vertices */

	while (ly<=y && rem>0) {	/* advance left edge? */
	    rem--;
	    i = li-1;			/* step ccw down left side */
	    if (i<0) i = p->n-1;
	    incrementalize_y(&p->vert[li], &p->vert[i], &l, &dl, y, mask);
	    ly = floor(p->vert[i].sy+.5);
	    li = i;
	}
	while (ry<=y && rem>0) {	/* advance right edge? */
	    rem--;
	    i = ri+1;			/* step cw down right edge */
	    if (i>=p->n) i = 0;
	    incrementalize_y(&p->vert[ri], &p->vert[i], &r, &dr, y, mask);
	    ry = floor(p->vert[i].sy+.5);
	    ri = i;
	}

	while (y<ly && y<ry) {	    /* do scanlines till end of l or r edge */
	    if (y>=win->y0 && y<=win->y1)
		if (l.sx<=r.sx) scanline(y, &l, &r, win, pixelproc, mask);
		else		scanline(y, &r, &l, win, pixelproc, mask);
	    y++;
	    increment(&l, &dl, mask);
	    increment(&r, &dr, mask);
	}
    }
}

/* scanline: output scanline by sampling polygon at Y=y+.5 */

static scanline(y, l, r, win, pixelproc, mask)
int y;
unsigned long mask;
Poly_vert *l, *r;
Window *win;
void (*pixelproc)();
{
    int x, lx, rx;
    Poly_vert p, dp;

    mask &= ~POLY_MASK(sx);		/* stop interpolating screen x */
    lx = ceil(l->sx-.5);
    if (lx<win->x0) lx = win->x0;
    rx = floor(r->sx-.5);
    if (rx>win->x1) rx = win->x1;
    if (lx>rx) return;
    incrementalize_x(l, r, &p, &dp, lx, mask);
    for (x=lx; x<=rx; x++) {		/* scan in x, generating pixels */
	(*pixelproc)(x, y, &p);
	increment(&p, &dp, mask);
    }
}

/*
 * incrementalize_y: put intersection of line Y=y+.5 with edge between points
 * p1 and p2 in p, put change with respect to y in dp
 */

static incrementalize_y(p1, p2, p, dp, y, mask)
register double *p1, *p2, *p, *dp;
register unsigned long mask;
int y;
{
    double dy, frac;

    dy = ((Poly_vert *)p2)->sy - ((Poly_vert *)p1)->sy;
    if (dy==0.) dy = 1.;
    frac = y+.5 - ((Poly_vert *)p1)->sy;

    for (; mask!=0; mask>>=1, p1++, p2++, p++, dp++)
	if (mask&1) {
	    *dp = (*p2-*p1)/dy;
	    *p = *p1+*dp*frac;
	}
}

/*
 * incrementalize_x: put intersection of line X=x+.5 with edge between points
 * p1 and p2 in p, put change with respect to x in dp
 */

static incrementalize_x(p1, p2, p, dp, x, mask)
register double *p1, *p2, *p, *dp;
register unsigned long mask;
int x;
{
    double dx, frac;

    dx = ((Poly_vert *)p2)->sx - ((Poly_vert *)p1)->sx;
    if (dx==0.) dx = 1.;
    frac = x+.5 - ((Poly_vert *)p1)->sx;

    for (; mask!=0; mask>>=1, p1++, p2++, p++, dp++)
	if (mask&1) {
	    *dp = (*p2-*p1)/dx;
	    *p = *p1+*dp*frac;
	}
}

static increment(p, dp, mask)
register double *p, *dp;
register unsigned long mask;
{
    for (; mask!=0; mask>>=1, p++, dp++)
	if (mask&1)
	    *p += *dp;
}
