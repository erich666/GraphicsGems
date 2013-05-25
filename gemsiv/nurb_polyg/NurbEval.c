/*
 * NurbEval.c - Code for evaluating NURB surfaces.
 *
 * John Peterson
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "nurbs.h"
#include "drawing.h"


/*
 * Return the current knot the parameter u is less than or equal to.
 * Find this "breakpoint" allows the evaluation routines to concentrate on
 * only those control points actually effecting the curve around u.]
 *
 *	m   is the number of points on the curve (or surface direction)
 *	k   is the order of the curve (or surface direction)
 *	kv  is the knot vector ([0..m+k-1]) to find the break point in.
 */

long
FindBreakPoint( double u, double * kv, long m, long k )
{
    long i;

    if (u == kv[m+1])	/* Special case for closed interval */
	return m;

    i = m + k;
    while ((u < kv[i]) && (i > 0))
	i--;
    return( i );
}

/*
 * Compute Bi,k(u), for i = 0..k.
 *  u		is the parameter of the spline to find the basis functions for
 *  brkPoint	is the start of the knot interval ("segment")
 *  kv		is the knot vector
 *  k		is the order of the curve
 *  bvals	is the array of returned basis values.
 *
 * (From Bartels, Beatty & Barsky, p.387)
 */

static void
BasisFunctions( double u, long brkPoint, double * kv, long k, double * bvals )
{
    register long r, s, i;
    register double omega;

    bvals[0] = 1.0;
    for (r = 2; r <= k; r++)
    {
	i = brkPoint - r + 1;
	bvals[r - 1] = 0.0;
	for (s = r-2; s >= 0; s--)
	{
	    i++;
	    if (i < 0)
		omega = 0;
	    else
		omega = (u - kv[i]) / (kv[i + r - 1] - kv[i]);
	    bvals[s + 1] = bvals[s + 1] + (1 - omega) * bvals[s];
	    bvals[s] = omega * bvals[s];
	}
    }
}

/*
 * Compute derivatives of the basis functions Bi,k(u)'
 */
static void
BasisDerivatives( double u, long brkPoint, double * kv, long k, double * dvals )
{
    register long s, i;
    register double omega, knotScale;

    BasisFunctions( u, brkPoint, kv, k - 1, dvals );

    dvals[k-1] = 0.0;	    /* BasisFunctions misses this */

    knotScale = kv[brkPoint + 1L] - kv[brkPoint];

    i = brkPoint - k + 1L;
    for (s = k - 2L; s >= 0L; s--)
    {
	i++;
	omega = knotScale * ((double)(k-1L)) / (kv[i+k-1L] - kv[i]);
	dvals[s + 1L] += -omega * dvals[s];
	dvals[s] *= omega;
    }
}

/*
 * Calculate a point p on NurbSurface n at a specific u, v using the tensor product.
 * If utan and vtan are not nil, compute the u and v tangents as well.
 *
 * Note the valid parameter range for u and v is
 * (kvU[orderU] <= u < kvU[numU), (kvV[orderV] <= v < kvV[numV])
 */

void
CalcPoint(double u, double v, NurbSurface * n,
	   Point3 * p, Point3 * utan, Point3 * vtan)
{
    register long i, j, ri, rj;
    register Point4 * cp;
    register double tmp;
    register double wsqrdiv;
    long ubrkPoint, ufirst;
    double bu[MAXORDER], buprime[MAXORDER];
    long vbrkPoint, vfirst;
    double bv[MAXORDER], bvprime[MAXORDER];
    Point4 r, rutan, rvtan;

    r.x = 0.0;
    r.y = 0.0;
    r.z = 0.0;
    r.w = 0.0;

    rutan = r;
    rvtan = r;

    /* Evaluate non-uniform basis functions (and derivatives) */

    ubrkPoint = FindBreakPoint( u, n->kvU, n->numU-1, n->orderU );
    ufirst = ubrkPoint - n->orderU + 1;
    BasisFunctions( u, ubrkPoint, n->kvU, n->orderU, bu );
    if (utan)
	BasisDerivatives( u, ubrkPoint, n->kvU, n->orderU, buprime );

    vbrkPoint = FindBreakPoint( v, n->kvV, n->numV-1, n->orderV );
    vfirst = vbrkPoint - n->orderV + 1;
    BasisFunctions( v, vbrkPoint, n->kvV, n->orderV, bv );
    if (vtan)
	BasisDerivatives( v, vbrkPoint, n->kvV, n->orderV, bvprime );

    /* Weight control points against the basis functions */

    for (i = 0; i < n->orderV; i++)
	for (j = 0; j < n->orderU; j++)
	{
	    ri = n->orderV - 1L - i;
	    rj = n->orderU - 1L - j;

	    tmp = bu[rj] * bv[ri];
	    cp = &( n->points[i+vfirst][j+ufirst] );
	    r.x += cp->x * tmp;
	    r.y += cp->y * tmp;
	    r.z += cp->z * tmp;
	    r.w += cp->w * tmp;

	    if (utan)
	    {
		tmp = buprime[rj] * bv[ri];
		rutan.x += cp->x * tmp;
		rutan.y += cp->y * tmp;
		rutan.z += cp->z * tmp;
		rutan.w += cp->w * tmp;
	    }
	    if (vtan)
	    {
		tmp = bu[rj] * bvprime[ri];
		rvtan.x += cp->x * tmp;
		rvtan.y += cp->y * tmp;
		rvtan.z += cp->z * tmp;
		rvtan.w += cp->w * tmp;
	    }
	}

    /* Project tangents, using the quotient rule for differentiation */

    wsqrdiv = 1.0 / (r.w * r.w);
    if (utan)
    {
	utan->x = (r.w * rutan.x - rutan.w * r.x) * wsqrdiv;
	utan->y = (r.w * rutan.y - rutan.w * r.y) * wsqrdiv;
	utan->z = (r.w * rutan.z - rutan.w * r.z) * wsqrdiv;
    }
    if (vtan)
    {
	vtan->x = (r.w * rvtan.x - rvtan.w * r.x) * wsqrdiv;
	vtan->y = (r.w * rvtan.y - rvtan.w * r.y) * wsqrdiv;
	vtan->z = (r.w * rvtan.z - rvtan.w * r.z) * wsqrdiv;
    }

    p->x = r.x / r.w;
    p->y = r.y / r.w;
    p->z = r.z / r.w;
}

/*
 * Draw a mesh of points by evaluating the surface at evenly spaced
 * points.
 */
void
DrawEvaluation( NurbSurface * n )
{
    Point3 p, utan, vtan;
    register long i, j;
    register double u, v, d;
    SurfSample ** pts ;

    long Granularity = 10;  /* Controls the number of steps in u and v */

    /* Allocate storage for the grid of points generated */

    CHECK( pts = (SurfSample**) malloc( (Granularity+1L) * sizeof( SurfSample* )));
    CHECK( pts[0] = (SurfSample*) malloc( (Granularity+1L)*(Granularity+1L)
		    * sizeof( SurfSample )));
    for (i = 1; i <= Granularity; i++)
	pts[i] = &(pts[0][(Granularity+1L) * i]);

    /* Compute points on curve */

    for (i = 0; i <= Granularity; i++)
    {
	v = ((double) i / (double) Granularity)
	    * (n->kvV[n->numV] - n->kvV[n->orderV-1])
	    + n->kvV[n->orderV-1];

	for (j = 0; j <= Granularity; j++)
	{
	    u = ((double) j / (double) Granularity)
		* (n->kvU[n->numU] - n->kvU[n->orderU-1])
		+ n->kvU[n->orderU-1];

	    CalcPoint( u, v, n, &(pts[i][j].point), &utan, &vtan );
	    (void) V3Cross( &utan, &vtan, &p );
	    d = V3Length( &p );
	    if (d != 0.0)
	    {
		p.x /= d;
		p.y /= d;
		p.z /= d;
	    }
	    else
	    {
		p.x = 0;
		p.y = 0;
		p.z = 0;
	    }
	    pts[i][j].normLen = d;
	    pts[i][j].normal = p;
	    pts[i][j].u = u;
	    pts[i][j].v = v;
	}
    }

    /* Draw the grid */

    for (i = 0; i < Granularity; i++)
	for (j = 0; j < Granularity; j++)
	{
	    (*DrawTriangle)( &pts[i][j], &pts[i+1][j+1], &pts[i+1][j] );
	    (*DrawTriangle)( &pts[i][j], &pts[i][j+1],	 &pts[i+1][j+1] );
	}

    free( pts[0] );
    free( pts );
}

