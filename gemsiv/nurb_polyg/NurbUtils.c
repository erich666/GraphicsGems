/*
 * NurbUtils.c - Code for Allocating, freeing, & copying NURB surfaces.
 *
 * John Peterson
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "nurbs.h"

/*
 * Allocate memory for a NURB (assumes numU, numV, orderU
 * and orderV have been set).  If ukv or vkv are not NIL, they
 * are assumed to be pointers to valid knot vectors.
 */

void
AllocNurb( NurbSurface * n, double * ukv, double * vkv )
{
    long i;

    if (! ukv)
	CHECK( n->kvU = (double *) malloc( (n->numU + n->orderU) * sizeof( double ) ) )
    else
	n->kvU = ukv;
    if (! vkv)
	CHECK( n->kvV = (double *) malloc( (n->numV + n->orderV) * sizeof( double ) ) )
    else
	n->kvV = vkv;

    CHECK( n->points = (Point4 **) malloc( (long) n->numV
					   * (long) sizeof( Point4 * ) ) );
    for (i = 0; i < n->numV; i++)
	CHECK( n->points[i] = (Point4 *) malloc( (long) n->numU
						 * (long) sizeof( Point4 )) );
}

/*
 * Release storage for a patch
 */

void
FreeNurb( NurbSurface * n )
{
    long i;

    if (n->kvU) free( n->kvU );
    n->kvU = NULL;
    if (n->kvV) free( n->kvV );
    n->kvV = NULL;
    for (i = 0; i < n->numV; i++)
	free( n->points[i] );
    free( n->points );
}

/*
 * Clone a nurb (deep copy)
 */

void
CloneNurb( NurbSurface * src, NurbSurface * dst )
{
    long i, j;
    double * srcp, *dstp;

    *dst = *src;	/* Copy fields that don't change */
    dst->kvU = NULL;
    dst->kvV = NULL;	/* So they get allocated */
    dst->points = NULL;

    AllocNurb( dst, NULL, NULL );

    /* Copy kv's */
    srcp = src->kvU;
    dstp = dst->kvU;
    for (i = 0; i < src->numU + src->orderU; i++)
	*dstp++ = *srcp++;

    srcp = src->kvV;
    dstp = dst->kvV;
    for (i = 0; i < src->numV + src->orderV; i++)
	*dstp++ = *srcp++;

    /* Copy control points */
    for (i = 0; i < src->numV; i++)
	for (j = 0; j < src->numU; j++)
	    dst->points[i][j] = src->points[i][j];
}
