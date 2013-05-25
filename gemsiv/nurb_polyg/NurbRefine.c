/*
 * NurbRefine.c - Given a refined knot vector, add control points to a surface.
 *
 * John Peterson
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "nurbs.h"

/*
 * Given the original knot vector ukv, and a new knotvector vkv, compute
 * the "alpha matrix" used to generate the corresponding new control points.
 * This routines allocates the alpha matrix if it isn't allocated already.
 *
 * This is from Bartels, Beatty & Barsky, p. 407
 */
static void
CalcAlpha( double * ukv, double * wkv, long m, long n, long k, double *** alpha )
{
    register long i, j;
    long brkPoint, r, rm1, last, s;
    double omega;
    double aval[MAXORDER];

    if (! *alpha)	/* Must allocate alpha */
    {
	CHECK( *alpha = (double **) malloc( (long) ((k+1) * sizeof( double * ))) );
	for (i = 0; i <= k; i++)
	    CHECK( (*alpha)[i] = (double *) malloc( (long) ((m + n + 1)
						    * sizeof( double ))) );
    }

    for (j = 0; j <= m + n; j++)
    {
	brkPoint = FindBreakPoint( wkv[j], ukv, m, k );
	aval[0] = 1.0;
	for (r = 2; r <= k; r++)
	{
	    rm1 = r - 1;
	    last = MIN( rm1, brkPoint );
	    i = brkPoint - last;
	    if (last < rm1)
		aval[last] = aval[last] * (wkv[j + r - 1] - ukv[i])
			    / (ukv[i + r - 1] - ukv[i]);
	    else
		aval[last] = 0.0;

	    for (s = last - 1; s >= 0; s-- )
	    {
		i++;
		omega = (wkv[j + r - 1] - ukv[i]) / (ukv[i + r - 1] - ukv[i]);
		aval[s + 1] = aval[s+1] + (1 - omega) * aval[s];
		aval[s] = omega * aval[s];
	    }
	}
	last = MIN( k - 1, brkPoint );
	for (i = 0; i <= k; i++)
	    (*alpha)[i][j] = 0.0;
	for (s = 0; s <= last; s++)
	    (*alpha)[last - s][j] = aval[s];
    }
}

/*
 * Apply the alpha matrix computed above to the rows (or columns)
 * of the surface.  If dirflag is true do the U's (row), else do V's (col).
 */
void
RefineSurface( NurbSurface * src, NurbSurface * dest, Boolean dirflag )
{
    register long i, j, out;
    register Point4 * dp, * sp;
    long i1, brkPoint, maxj, maxout;
    register double tmp;
    double ** alpha = NULL;

    /* Compute the alpha matrix and indexing variables for the requested direction */

    if (dirflag)
    {
	CalcAlpha( src->kvU, dest->kvU, src->numU - 1, dest->numU - src->numU,
		   src->orderU, &alpha );
	maxj = dest->numU;
	maxout = src->numV;
    }
    else
    {
	CalcAlpha( src->kvV, dest->kvV, src->numV - 1, dest->numV - src->numV,
		   src->orderV, &alpha );
	maxj = dest->numV;
	maxout = dest->numU;
    }

    /* Apply the alpha matrix to the original control points, generating new ones */

    for (out = 0; out < maxout; out++)
	for (j = 0; j < maxj; j++)
	{
	    if (dirflag)
	    {
		dp = &(dest->points[out][j]);
		brkPoint = FindBreakPoint( dest->kvU[j], src->kvU,
					   src->numU-1, src->orderU );
		i1 = MAX( brkPoint - src->orderU + 1, 0 );
		sp = &(src->points[out][i1]);
	    } else {
		dp = &(dest->points[j][out]);
		brkPoint = FindBreakPoint( dest->kvV[j], src->kvV,
					   src->numV-1, src->orderV );
		i1 = MAX( brkPoint - src->orderV + 1, 0 );
		sp = &(src->points[i1][out]);
	    }
	    dp->x = 0.0;
	    dp->y = 0.0;
	    dp->z = 0.0;
	    dp->w = 0.0;
	    for (i = i1; i <= brkPoint; i++)
	    {
		tmp = alpha[i - i1][j];
		sp = (dirflag ? &(src->points[out][i]) : &(src->points[i][out]) );
		dp->x += tmp * sp->x;
		dp->y += tmp * sp->y;
		dp->z += tmp * sp->z;
		dp->w += tmp * sp->w;
	    }
	}

    /* Free up the alpha matrix */
    for (i = 0; i <= (dirflag ? src->orderU : src->orderV); i++)
	free( alpha[i] );
    free( alpha );
}
