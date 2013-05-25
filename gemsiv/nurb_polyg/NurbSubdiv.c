/*
 * NurbSubdiv.c - Perform adaptive subdivision on a NURB surface.
 *
 * John Peterson
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "nurbs.h"
#include "drawing.h"

#define EPSILON 0.0000001   /* Used to determine when things are too small. */
#define DIVPT( p, dn ) { ((p).x) /= (dn); ((p).y) /= (dn); ((p).z) /= (dn); }

double SubdivTolerance;	    /* Size, in pixels of facets produced */

#define maxV(surf) ((surf)->numV-1L)
#define maxU(surf) ((surf)->numU-1L)

/*
 * Split a knot vector at the center, by adding multiplicity k knots near
 * the middle of the parameter range.  Tries to start with an existing knot,
 * but will add a new knot value if there's nothing in "the middle" (e.g.,
 * a Bezier curve).
 */
static long
SplitKV( double * srckv,
	 double ** destkv,
	 long * splitPt,    /* Where the knot interval is split */
	 long m, long k )
{
    long i, last;
    long middex, extra, same;	/* "middex" ==> "middle index" */
    double midVal;

    extra = 0L;
    last = (m + k);

    middex = last / 2;
    midVal = srckv[middex];

    /* Search forward and backward to see if multiple knot is already there */

    i = middex+1L;
    same = 1L;
    while ((i < last) && (srckv[i] == midVal)) {
	i++;
	same++;
    }

    i = middex-1L;
    while ((i > 0L) && (srckv[i] == midVal)) {
	i--;
	middex--;	    /* middex is start of multiple knot */
	same++;
    }

    if (i <= 0L)	    /* No knot in middle, must create it */
    {
	midVal = (srckv[0L] + srckv[last]) / 2.0;
	middex = last / 2L;
	while (srckv[middex + 1L] < midVal)
	    middex++;
	same = 0L;
    }

    extra = k - same;
    CHECK( *destkv = (double *) malloc( (long) (sizeof( double )
					* (m+k+extra+1L) ) ) );

    if (same < k)	    /* Must add knots */
    {
	for (i = 0L; i <= middex; i++)
	    (*destkv)[i] = srckv[i];

	for (i = middex+1L; i <= middex+extra; i++)
	    (*destkv)[i] = midVal;

	for (i = middex + k - same + 1L; i <= m + k + extra; i++)
	    (*destkv)[i] = srckv[i - extra];
    }
    else
    {
	for (i = 0L; i <= m + k; i++)
	(*destkv)[i] = srckv[i];
    }

    *splitPt = (extra < k) ? middex - 1L : middex;
    return( extra );
}

/*
 * Given a line defined by firstPt and lastPt, project midPt onto
 * that line.  Used for fixing "cracks".
 */
static void
ProjectToLine( Point3 * firstPt, Point3 * lastPt, Point3 * midPt )
{
    Point3 base, v0, vm;
    double fraction, denom;

    base = *firstPt;

    (void) V3Sub( lastPt, &base, &v0 );
    (void) V3Sub( midPt, &base, &vm );

    denom = V3SquaredLength( &v0 );
    fraction = (denom == 0.0) ? 0.0 : (V3Dot( &v0, &vm ) / denom);

    midPt->x = base.x + fraction * v0.x;
    midPt->y = base.y + fraction * v0.y;
    midPt->z = base.z + fraction * v0.z;
}

/*
 * If a normal has collapsed to zero (normLen == 0.0) then try
 * and fix it by looking at its neighbors.  If all the neighbors
 * are sick, then re-compute them from the plane they form.
 * If that fails too, then we give up...
 */
static void
FixNormals( SurfSample * s0, SurfSample * s1, SurfSample * s2 )
{
    Boolean goodnorm;
    long i, j, ok;
    double dist;
    SurfSample * V[3];
    Point3 norm;

    V[0] = s0; V[1] = s1; V[2] = s2;

    /* Find a reasonable normal */
    for (ok = 0, goodnorm = FALSE;
	(ok < 3L) && !(goodnorm = (V[ok]->normLen > 0.0)); ok++);

    if (! goodnorm)	/* All provided normals are zilch, try and invent one */
    {
	norm.x = 0.0; norm.y = 0.0; norm.z = 0.0;

	for (i = 0; i < 3L; i++)
	{
	    j = (i + 1L) % 3L;
	    norm.x += (V[i]->point.y - V[j]->point.y) * (V[i]->point.z + V[j]->point.z);
	    norm.y += (V[i]->point.z - V[j]->point.z) * (V[i]->point.x + V[j]->point.x);
	    norm.z += (V[i]->point.x - V[j]->point.x) * (V[i]->point.y + V[j]->point.y);
	}
	dist = V3Length( &norm );
	if (dist == 0.0)
	    return;		/* This sucker's hopeless... */

	DIVPT( norm, dist );

	for (i = 0; i < 3; i++)
	{
	    V[i]->normal = norm;
	    V[i]->normLen = dist;
	}
    }
    else	    /* Replace a sick normal with a healthy one nearby */
    {
	for (i = 0; i < 3; i++)
	    if ((i != ok) && (V[i]->normLen == 0.0))
		V[i]->normal = V[ok]->normal;
    }
    return;
}

/*
 * Normalize the normal in a sample.  If it's degenerate,
 * flag it as such by setting the normLen to 0.0
 */
static void
AdjustNormal( SurfSample * samp )
{
    /* If it's not degenerate, do the normalization now */
    samp->normLen = V3Length( &(samp->normal) );

    if (samp->normLen < EPSILON)
	samp->normLen = 0.0;
    else
	DIVPT( (samp->normal), samp->normLen );
}

/*
 * Compute the normal of a corner point of a mesh.  The
 * base is the value of the point at the corner, indU and indV
 * are the mesh indices of that point (either 0 or numU|numV).
 */
static void
GetNormal( NurbSurface * n, long indV, long indU )
{
    Point3 tmpL, tmpR;	/* "Left" and "Right" of the base point */
    SurfSample * crnr;

    if ( (indU && indV) || ((! indU) && (!indV)) )
    {
	if (indU)
	    crnr = &(n->cnn);
	else
	    crnr = &(n->c00);
	DIVW( &(n->points[indV][(indU ? (indU-1L) : 1L)]), &tmpL );
	DIVW( &(n->points[(indV ? (indV-1L) : 1L)][indU]), &tmpR );
    }
    else
    {
	if (indU)
	    crnr = &(n->c0n);
	else
	    crnr = &(n->cn0);
	DIVW( &(n->points[indV][(indU ? (indU-1L) : 1L)]), &tmpR );
	DIVW( &(n->points[(indV ? (indV-1L) : 1L)][indU]), &tmpL );
    }

    (void) V3Sub( &tmpL, &(crnr->point), &tmpL );
    (void) V3Sub( &tmpR, &(crnr->point), &tmpR );
    (void) V3Cross( &tmpL, &tmpR, &(crnr->normal) );
    AdjustNormal( crnr );
}

/*
 * Build the new corners in the two new surfaces, computing both
 * point on the surface along with the normal.	Prevent cracks that may occur.
 */
static void
MakeNewCorners( NurbSurface * parent,
		NurbSurface * kid0,
		NurbSurface * kid1,
		Boolean dirflag )
{
    DIVW( &(kid0->points[maxV(kid0)][maxU(kid0)]), &(kid0->cnn.point) );
    GetNormal( kid0, maxV(kid0), maxU(kid0) );

    if (dirflag)
    {
	kid0->strUn = FALSE;	/* Must re-test new edge straightness */

	DIVW( &(kid0->points[0L][maxU(kid0)]), &(kid0->c0n.point) );
	GetNormal( kid0, 0L, maxU(kid0) );
	/*
	 * Normals must be re-calculated for kid1 in case the surface
	 * was split at a c1 (or c0!) discontinutiy
	 */
	kid1->c00.point = kid0->c0n.point;
	GetNormal( kid1, 0L, 0L );
	kid1->cn0.point = kid0->cnn.point;
	GetNormal( kid1, maxV(kid1), 0L );

	/*
	 * Prevent cracks from forming by forcing the points on the seam to
	 * lie along any straight edges.  (Must do this BEFORE finding normals)
	 */
	if (parent->strV0)
	    ProjectToLine( &(parent->c00.point),
			   &(parent->c0n.point),
			   &(kid0->c0n.point) );
	if (parent->strVn)
	    ProjectToLine( &(parent->cn0.point),
			   &(parent->cnn.point),
			   &(kid0->cnn.point) );

	kid1->c00.point = kid0->c0n.point;
	kid1->cn0.point = kid0->cnn.point;
	kid1->strU0 = FALSE;
    }
    else
    {
	kid0->strVn = FALSE;

	DIVW( &(kid0->points[maxV(kid0)][0]), &(kid0->cn0.point) );
	GetNormal( kid0, maxV(kid0), 0L );
	kid1->c00.point = kid0->cn0.point;
	GetNormal( kid1, 0L, 0L );
	kid1->c0n.point = kid0->cnn.point;
	GetNormal( kid1, 0L, maxU(kid1) );

	if (parent->strU0)
	    ProjectToLine( &(parent->c00.point),
			   &(parent->cn0.point),
			   &(kid0->cn0.point) );
	if (parent->strUn)
	    ProjectToLine( &(parent->c0n.point),
			   &(parent->cnn.point),
			   &(kid0->cnn.point) );

	kid1->c00.point = kid0->cn0.point;
	kid1->c0n.point = kid0->cnn.point;
	kid1->strV0 = FALSE;
    }
}

/*
 * Split a surface into two halves.  First inserts multiplicity k knots
 * in the center of the parametric range.  After refinement, the two
 * resulting surfaces are copied into separate data structures.	 If the
 * parent surface had straight edges, the points of the children are
 * projected onto those edges.
 */
static void
SplitSurface( NurbSurface * parent,
	      NurbSurface * kid0, NurbSurface * kid1,
	      Boolean dirflag )	    /* If true subdivided in U, else in V */
{
    NurbSurface tmp;
    double * newkv;
    long i, j, splitPt;

    /*
     * Add a multiplicty k knot to the knot vector in the direction
     * specified by dirflag, and refine the surface.  This creates two
     * adjacent surfaces with c0 discontinuity at the seam.
     */

    tmp = *parent;	/* Copy order, # of points, etc. */
    if (dirflag)
    {
	tmp.numU = parent->numU + SplitKV( parent->kvU,
					    &newkv,
					    &splitPt,
					    maxU(parent),
					    parent->orderU );
	AllocNurb( &tmp, newkv, NULL );
	for (i = 0L; i < tmp.numV + tmp.orderV; i++)
	    tmp.kvV[i] = parent->kvV[i];
    }
    else
    {
	tmp.numV = parent->numV + SplitKV( parent->kvV,
					    &newkv,
					    &splitPt,
					    maxV(parent),
					    parent->orderV );
	AllocNurb( &tmp, NULL, newkv );
	for (i = 0L; i < tmp.numU + tmp.orderU; i++)
	    tmp.kvU[i] = parent->kvU[i];
    }
    RefineSurface( parent, &tmp, dirflag );

    /*
     * Build the two child surfaces, and copy the data from the refined
     * version of the parent (tmp) into the two children
     */

    /* First half */

    *kid0 = *parent;	/* copy various edge flags and orders */

    kid0->numU = dirflag ? splitPt+1L : parent->numU;
    kid0->numV = dirflag ? parent->numV : splitPt+1L;
    kid0->kvU = kid0->kvV = NULL;
    kid0->points = NULL;
    AllocNurb( kid0, NULL, NULL );

    for (i = 0L; i < kid0->numV; i++)	/* Copy the point and kv data */
	for (j = 0L; j < kid0->numU; j++)
	    kid0->points[i][j] = tmp.points[i][j];
    for (i = 0L; i < kid0->orderU + kid0->numU; i++)
	kid0->kvU[i] = tmp.kvU[i];
    for (i = 0L; i < kid0->orderV + kid0->numV; i++)
	kid0->kvV[i] = tmp.kvV[i];

    /* Second half */

    splitPt++;
    *kid1 = *parent;

    kid1->numU = dirflag ? tmp.numU - splitPt : parent->numU;
    kid1->numV = dirflag ? parent->numV : tmp.numV - splitPt;
    kid1->kvU = kid1->kvV = NULL;
    kid1->points = NULL;
    AllocNurb( kid1, NULL, NULL );

    for (i = 0L; i < kid1->numV; i++)	/* Copy the point and kv data */
	for (j = 0L; j < kid1->numU; j++)
	    kid1->points[i][j]
		= tmp.points[dirflag ? i: (i + splitPt) ][dirflag ? (j + splitPt) : j];
    for (i = 0L; i < kid1->orderU + kid1->numU; i++)
	kid1->kvU[i] = tmp.kvU[dirflag ? (i + splitPt) : i];
    for (i = 0L; i < kid1->orderV + kid1->numV; i++)
	kid1->kvV[i] = tmp.kvV[dirflag ? i : (i + splitPt)];

    /* Construct new corners on the boundry between the two kids */
    MakeNewCorners( parent, kid0, kid1, dirflag );

    FreeNurb( &tmp );	    /* Get rid of refined parent */
}

/*
 * Test if a particular row or column of control points in a mesh
 * is "straight" with respect to a particular tolerance.  Returns true
 * if it is.
 */

#define GETPT( i )  (( dirflag ? &(n->points[crvInd][i]) : &(n->points[i][crvInd]) ))

static Boolean
IsCurveStraight( NurbSurface * n,
		 double tolerance,
		 long crvInd,
		 Boolean dirflag )  /* If true, test in U direction, else test in V */
{
    Point3 p, vec, prod;
    Point3 cp, e0;
    long i, last;
    double linelen, dist;

    /* Special case: lines are automatically straight. */
    if ((dirflag ? n->numU : n->numV) == 2L)
	return( TRUE );

    last = (dirflag ? n->numU : n->numV) - 1L;
    ScreenProject( GETPT( 0L ), &e0 );

    /* Form an initial line to test the other points against (skiping degen lines) */

    linelen = 0.0;
    for (i = last; (i > 0L) && (linelen < EPSILON); i--)
    {
	ScreenProject( GETPT( i ), &cp );
	(void) V3Sub( &cp, &e0, &vec );

	linelen = sqrt( V3SquaredLength( &vec ) );
    }

    DIVPT( vec, linelen );

    if (linelen > EPSILON)	/* If no non-degenerate lines found, it's all degen */
	for (i = 1L; i <= last; i++)
	{
	    /* The cross product of the vector defining the
	     * initial line with the vector of the current point
	     * gives the distance to the line. */
	    ScreenProject( GETPT( i ), &cp );
	    (void) V3Sub( &cp,&e0,&p );

	    (void) V3Cross( &p, &vec, &prod );
	    dist = V3Length( &prod );

	    if (dist > tolerance)
		return( FALSE );
	}

    return( TRUE );
}

/*
 * Check to see if a surface is flat.  Tests are only performed on edges and
 * directions that aren't already straight.  If an edge is flagged as straight
 * (from the parent surface) it is assumed it will stay that way.
 */
static Boolean
TestFlat( NurbSurface * n, double tolerance )
{
    long i;
    Boolean straight;
    Point3 cp00, cp0n, cpn0, cpnn, planeEqn;
    double dist,d ;

    /* Check edge straightness */

    if (! n->strU0)
	n->strU0 = IsCurveStraight( n, tolerance, 0L, FALSE );
    if (! n->strUn)
	n->strUn = IsCurveStraight( n, tolerance, maxU(n), FALSE );
    if (! n->strV0)
	n->strV0 = IsCurveStraight( n, tolerance, 0L, TRUE );
    if (! n->strVn)
	n->strVn = IsCurveStraight( n, tolerance, maxV(n), TRUE );

    /* Test to make sure control points are straight in U and V */

    straight = TRUE;
    if ( (! n->flatU) && (n->strV0) && (n->strVn) )
	for (i = 1L;
	     (i < maxV(n)) && (straight = IsCurveStraight( n, tolerance, i, TRUE ));
	     i++);

    if (straight && n->strV0 && n->strVn)
	n->flatU = TRUE;

    straight = TRUE;
    if ( (! n->flatV) && (n->strU0) && (n->strUn) )
	for (i = 1L;
	     (i < maxU(n)) && (straight = IsCurveStraight( n, tolerance, i, FALSE ));
	     i++);

    if (straight && n->strU0 && n->strUn)
	n->flatV = TRUE;

    if ( (! n->flatV) || (! n->flatU) )
	return( FALSE );

    /* The surface can pass the above tests but still be twisted. */

    ScreenProject( &(n->points[0L][0L]),	    &cp00 );
    ScreenProject( &(n->points[0L][maxU(n)]),	    &cp0n );
    ScreenProject( &(n->points[maxV(n)][0L]),	    &cpn0 );
    ScreenProject( &(n->points[maxV(n)][maxU(n)]),  &cpnn );

    (void) V3Sub( &cp0n, &cp00, &cp0n );    /* Make edges into vectors */

    (void) V3Sub( &cpn0, &cp00, &cpn0 );

    /*
     * Compute the plane equation from two adjacent sides, and
     * measure the distance from the far point to the plane.  If it's
     * larger than tolerance, the surface is twisted.
     */

    (void) V3Cross( &cpn0, &cp0n, &planeEqn );

    (void) V3Normalize( &planeEqn );	/* Normalize to keep adds in sync w/ mults */

    d = V3Dot( &planeEqn, &cp00 );
    dist = fabs( V3Dot( &planeEqn, &cpnn ) - d );

    if ( dist > tolerance ) /* Surface is twisted */
	return( FALSE );
    else
	return( TRUE );
}

/*
 * Turn a sufficiently flat surface into triangles.
 */
static void
EmitTriangles( NurbSurface * n )
{
    Point3 vecnn, vec0n;		/* Diagonal vectors */
    double len2nn, len20n;		/* Diagonal lengths squared */
    double u0, un, v0, vn;		/* Texture coords;

    /*
     * Measure the distance along the two diagonals to decide the best
     * way to cut the rectangle into triangles.
     */

    (void) V3Sub( &n->c00.point, &n->cnn.point, &vecnn );
    (void) V3Sub( &n->c0n.point, &n->cn0.point, &vec0n );

    len2nn = V3SquaredLength( &vecnn ); /* Use these to reject triangles */
    len20n = V3SquaredLength( &vec0n ); /* that are too small to render */

    if (MAX(len2nn, len20n) < EPSILON)
	return;				/* Triangles are too small to render */

    /*
     * Assign the texture coordinates
     */
    u0 = n->kvU[n->orderU-1L];
    un = n->kvU[n->numU];
    v0 = n->kvV[n->orderV-1L];
    vn = n->kvV[n->numV];
    n->c00.u = u0; n->c00.v = v0;
    n->c0n.u = un; n->c0n.v = v0;
    n->cn0.u = u0; n->cn0.v = vn;
    n->cnn.u = un; n->cnn.v = vn;

    /*
     * If any normals are sick, fix them now.
     */
    if ((n->c00.normLen == 0.0) || (n->cnn.normLen == 0.0) || (n->cn0.normLen == 0.0))
	FixNormals( &(n->c00), &(n->cnn), &(n->cn0) );
    if (n->c0n.normLen == 0.0)
	FixNormals( &(n->c00), &(n->c0n), &(n->cnn) );

    if ( len2nn < len20n )
    {
	(*DrawTriangle)( &n->c00, &n->cnn, &n->cn0 );
	(*DrawTriangle)( &n->c00, &n->c0n, &n->cnn );
    }
    else
    {
	(*DrawTriangle)( &n->c0n, &n->cnn, &n->cn0 );
	(*DrawTriangle)( &n->c0n, &n->cn0, &n->c00 );
    }
}

/*
 * The recursive subdivision algorithm.	 Test if the surface is flat.
 * If so, split it into triangles.  Otherwise, split it into two halves,
 * and invoke the procedure on each half.
 */
static void
DoSubdivision( NurbSurface * n, double tolerance, Boolean dirflag, long level )
{
    NurbSurface left, right;	/* ...or top or bottom. Whatever spins your wheels. */

    if (TestFlat( n, tolerance ))
    {
	EmitTriangles( n );
    }
    else
    {
	if ( ((! n->flatV) && (! n->flatU)) || ((n->flatV) && (n->flatU)) )
	    dirflag = ! dirflag;    /* If twisted or curved in both directions, */
	else			    /* then alternate subdivision direction */
	{
	    if (n->flatU)	    /* Only split in directions that aren't flat */
		dirflag = FALSE;
	    else
		dirflag = TRUE;
	}
	SplitSurface( n, &left, &right, dirflag );
	DoSubdivision( &left, tolerance, dirflag, level + 1L );
	DoSubdivision( &right, tolerance, dirflag, level + 1L );
	FreeNurb( &left );
	FreeNurb( &right );	    /* Deallocate surfaces made by SplitSurface */
    }
}

/*
 * Main entry point for subdivision */
void
DrawSubdivision( NurbSurface * surf )
{
    surf->flatV = FALSE;
    surf->flatU = FALSE;
    surf->strU0 = FALSE;
    surf->strUn = FALSE;
    surf->strV0 = FALSE;
    surf->strVn = FALSE;

    /*
     * Initialize the projected corners of the surface
     * and the normals.
     */
    DIVW( &(surf->points[0L][0L]),			 &surf->c00.point );
    DIVW( &(surf->points[0L][surf->numU-1L]),		 &surf->c0n.point );
    DIVW( &(surf->points[surf->numV-1L][0L]),		 &surf->cn0.point );
    DIVW( &(surf->points[surf->numV-1L][surf->numU-1L]), &surf->cnn.point );

    GetNormal( surf, 0L, 0L );
    GetNormal( surf, 0L, maxU(surf) );
    GetNormal( surf, maxV(surf), 0L );
    GetNormal( surf, maxV(surf), maxU(surf) );

    DoSubdivision( surf, SubdivTolerance, TRUE, 0L );
    /* Note surf is deallocated by the subdivision process */
}
