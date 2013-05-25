/*
 * Generic Convex Polygon Scan Conversion and Clipping
 * by Paul Heckbert
 * from "Graphics Gems", Academic Press, 1990
 */

/*
 * poly_clip.c: homogeneous 3-D convex polygon clipper
 *
 * Paul Heckbert	1985, Dec 1989
 */

#include <stdio.h>
#include "poly.h"

#define SWAP(a, b, temp)	{temp = a; a = b; b = temp;}
#define COORD(vert, i) ((double *)(vert))[i]

#define CLIP_AND_SWAP(elem, sign, k, p, q, r) { \
    poly_clip_to_halfspace(p, q, &v->elem-(double *)v, sign, sign*k); \
    if (q->n==0) {p1->n = 0; return POLY_CLIP_OUT;} \
    SWAP(p, q, r); \
}

/*
 * poly_clip_to_box: Clip the convex polygon p1 to the screen space box
 * using the homogeneous screen coordinates (sx, sy, sz, sw) of each vertex,
 * testing if v->sx/v->sw > box->x0 and v->sx/v->sw < box->x1,
 * and similar tests for y and z, for each vertex v of the polygon.
 * If polygon is entirely inside box, then POLY_CLIP_IN is returned.
 * If polygon is entirely outside box, then POLY_CLIP_OUT is returned.
 * Otherwise, if the polygon is cut by the box, p1 is modified and
 * POLY_CLIP_PARTIAL is returned.
 *
 * Given an n-gon as input, clipping against 6 planes could generate an
 * (n+6)gon, so POLY_NMAX in poly.h must be big enough to allow that.
 */

int poly_clip_to_box(p1, box)
register Poly *p1;
register Poly_box *box;
{
    int x0out = 0, x1out = 0, y0out = 0, y1out = 0, z0out = 0, z1out = 0;
    register int i;
    register Poly_vert *v;
    Poly p2, *p, *q, *r;

    if (p1->n+6>POLY_NMAX) {
	fprintf(stderr, "poly_clip_to_box: too many vertices: %d (max=%d-6)\n",
	    p1->n, POLY_NMAX);
	exit(1);
    }
    if (sizeof(Poly_vert)/sizeof(double) > 32) {
	fprintf(stderr, "Poly_vert structure too big; must be <=32 doubles\n");
	exit(1);
    }

    /* count vertices "outside" with respect to each of the six planes */
    for (v=p1->vert, i=p1->n; i>0; i--, v++) {
	if (v->sx < box->x0*v->sw) x0out++;	/* out on left */
	if (v->sx > box->x1*v->sw) x1out++;	/* out on right */
	if (v->sy < box->y0*v->sw) y0out++;	/* out on top */
	if (v->sy > box->y1*v->sw) y1out++;	/* out on bottom */
	if (v->sz < box->z0*v->sw) z0out++;	/* out on near */
	if (v->sz > box->z1*v->sw) z1out++;	/* out on far */
    }

    /* check if all vertices inside */
    if (x0out+x1out+y0out+y1out+z0out+z1out == 0) return POLY_CLIP_IN;

    /* check if all vertices are "outside" any of the six planes */
    if (x0out==p1->n || x1out==p1->n || y0out==p1->n ||
	y1out==p1->n || z0out==p1->n || z1out==p1->n) {
	    p1->n = 0;
	    return POLY_CLIP_OUT;
	}

    /*
     * now clip against each of the planes that might cut the polygon,
     * at each step toggling between polygons p1 and p2
     */
    p = p1;
    q = &p2;
    if (x0out) CLIP_AND_SWAP(sx, -1., box->x0, p, q, r);
    if (x1out) CLIP_AND_SWAP(sx,  1., box->x1, p, q, r);
    if (y0out) CLIP_AND_SWAP(sy, -1., box->y0, p, q, r);
    if (y1out) CLIP_AND_SWAP(sy,  1., box->y1, p, q, r);
    if (z0out) CLIP_AND_SWAP(sz, -1., box->z0, p, q, r);
    if (z1out) CLIP_AND_SWAP(sz,  1., box->z1, p, q, r);

    /* if result ended up in p2 then copy it to p1 */
    if (p==&p2)
	bcopy(&p2, p1, sizeof(Poly)-(POLY_NMAX-p2.n)*sizeof(Poly_vert));
    return POLY_CLIP_PARTIAL;
}

/*
 * poly_clip_to_halfspace: clip convex polygon p against a plane,
 * copying the portion satisfying sign*s[index] < k*sw into q,
 * where s is a Poly_vert* cast as a double*.
 * index is an index into the array of doubles at each vertex, such that
 * s[index] is sx, sy, or sz (screen space x, y, or z).
 * Thus, to clip against xmin, use
 *	poly_clip_to_halfspace(p, q, XINDEX, -1., -xmin);
 * and to clip against xmax, use
 *	poly_clip_to_halfspace(p, q, XINDEX,  1.,  xmax);
 */

void poly_clip_to_halfspace(p, q, index, sign, k)
Poly *p, *q;
register int index;
double sign, k;
{
    register unsigned long m;
    register double *up, *vp, *wp;
    register Poly_vert *v;
    int i;
    Poly_vert *u;
    double t, tu, tv;

    q->n = 0;
    q->mask = p->mask;

    /* start with u=vert[n-1], v=vert[0] */
    u = &p->vert[p->n-1];
    tu = sign*COORD(u, index) - u->sw*k;
    for (v= &p->vert[0], i=p->n; i>0; i--, u=v, tu=tv, v++) {
	/* on old polygon (p), u is previous vertex, v is current vertex */
	/* tv is negative if vertex v is in */
	tv = sign*COORD(v, index) - v->sw*k;
	if (tu<=0. ^ tv<=0.) {
	    /* edge crosses plane; add intersection point to q */
	    t = tu/(tu-tv);
	    up = (double *)u;
	    vp = (double *)v;
	    wp = (double *)&q->vert[q->n];
	    for (m=p->mask; m!=0; m>>=1, up++, vp++, wp++)
		if (m&1) *wp = *up+t*(*vp-*up);
	    q->n++;
	}
	if (tv<=0.)		/* vertex v is in, copy it to q */
	    q->vert[q->n++] = *v;
    }
}
