/* ptinpoly.c - point in polygon inside/outside code.

   by Eric Haines, 3D/Eye Inc, erich@eye.com

   This code contains the following algorithms:
	crossings - count the crossing made by a ray from the test point
	crossings-multiply - as above, but avoids a division; often a bit faster
	angle summation - sum the angle formed by point and vertex pairs
	weiler angle summation - sum the angles using quad movements
	half-plane testing - test triangle fan using half-space planes
	barycentric coordinates - test triangle fan w/barycentric coords
	spackman barycentric - preprocessed barycentric coordinates
	trapezoid testing - bin sorting algorithm
	grid testing - grid imposed on polygon
	exterior test - for convex polygons, check exterior of polygon
	inclusion test - for convex polygons, use binary search for edge.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ptinpoly.h"

#define X	0
#define Y	1

#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

#ifndef HUGE
#define HUGE	1.797693134862315e+308
#endif

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

/* test if a & b are within epsilon.  Favors cases where a < b */
#define Near(a,b,eps)	( ((b)-(eps)<(a)) && ((a)-(eps)<(b)) )

#define MALLOC_CHECK( a )	if ( !(a) ) {				   \
				    fprintf( stderr, "out of memory\n" ) ; \
				    exit(1) ;				   \
				}


/* ======= Crossings algorithm ============================================ */

/* Shoot a test ray along +X axis.  The strategy, from MacMartin, is to
 * compare vertex Y values to the testing point's Y and quickly discard
 * edges which are entirely to one side of the test ray.
 *
 * Input 2D polygon _pgon_ with _numverts_ number of vertices and test point
 * _point_, returns 1 if inside, 0 if outside.	WINDING and CONVEX can be
 * defined for this test.
 */
int CrossingsTest( pgon, numverts, point )
double	pgon[][2] ;
int	numverts ;
double	point[2] ;
{
#ifdef	WINDING
register int	crossings ;
#endif
register int	j, yflag0, yflag1, inside_flag, xflag0 ;
register double ty, tx, *vtx0, *vtx1 ;
#ifdef	CONVEX
register int	line_flag ;
#endif

    tx = point[X] ;
    ty = point[Y] ;

    vtx0 = pgon[numverts-1] ;
    /* get test bit for above/below X axis */
    yflag0 = ( vtx0[Y] >= ty ) ;
    vtx1 = pgon[0] ;

#ifdef	WINDING
    crossings = 0 ;
#else
    inside_flag = 0 ;
#endif
#ifdef	CONVEX
    line_flag = 0 ;
#endif
    for ( j = numverts+1 ; --j ; ) {

	yflag1 = ( vtx1[Y] >= ty ) ;
	/* check if endpoints straddle (are on opposite sides) of X axis
	 * (i.e. the Y's differ); if so, +X ray could intersect this edge.
	 */
	if ( yflag0 != yflag1 ) {
	    xflag0 = ( vtx0[X] >= tx ) ;
	    /* check if endpoints are on same side of the Y axis (i.e. X's
	     * are the same); if so, it's easy to test if edge hits or misses.
	     */
	    if ( xflag0 == ( vtx1[X] >= tx ) ) {

		/* if edge's X values both right of the point, must hit */
#ifdef	WINDING
		if ( xflag0 ) crossings += ( yflag0 ? -1 : 1 ) ;
#else
		if ( xflag0 ) inside_flag = !inside_flag ;
#endif
	    } else {
		/* compute intersection of pgon segment with +X ray, note
		 * if >= point's X; if so, the ray hits it.
		 */
		if ( (vtx1[X] - (vtx1[Y]-ty)*
		     ( vtx0[X]-vtx1[X])/(vtx0[Y]-vtx1[Y])) >= tx ) {
#ifdef	WINDING
		    crossings += ( yflag0 ? -1 : 1 ) ;
#else
		    inside_flag = !inside_flag ;
#endif
		}
	    }
#ifdef	CONVEX
	    /* if this is second edge hit, then done testing */
	    if ( line_flag ) goto Exit ;

	    /* note that one edge has been hit by the ray's line */
	    line_flag = TRUE ;
#endif
	}

	/* move to next pair of vertices, retaining info as possible */
	yflag0 = yflag1 ;
	vtx0 = vtx1 ;
	vtx1 += 2 ;
    }
#ifdef	CONVEX
    Exit: ;
#endif
#ifdef	WINDING
    /* test if crossings is not zero */
    inside_flag = (crossings != 0) ;
#endif

    return( inside_flag ) ;
}

/* ======= Angle summation algorithm ======================================= */

/* Sum angles made by (vtxN to test point to vtxN+1), check if in proper
 * range to be inside.	VERY SLOW, included for tutorial reasons (i.e.
 * to show why one should never use this algorithm).
 *
 * Input 2D polygon _pgon_ with _numverts_ number of vertices and test point
 * _point_, returns 1 if inside, 0 if outside.
 */
int AngleTest( pgon, numverts, point )
double	pgon[][2] ;
int	numverts ;
double	point[2] ;
{
register double *vtx0, *vtx1, angle, len, vec0[2], vec1[2], vec_dot ;
register int	j ;
int	inside_flag ;

    /* sum the angles and see if answer mod 2*PI > PI */
    vtx0 = pgon[numverts-1] ;
    vec0[X] = vtx0[X] - point[X] ;
    vec0[Y] = vtx0[Y] - point[Y] ;
    len = sqrt( vec0[X] * vec0[X] + vec0[Y] * vec0[Y] ) ;
    if ( len <= 0.0 ) {
	/* point and vertex coincide */
	return( 1 ) ;
    }
    vec0[X] /= len ;
    vec0[Y] /= len ;

    angle = 0.0 ;
    for ( j = 0 ; j < numverts ; j++ ) {
	vtx1 = pgon[j] ;
	vec1[X] = vtx1[X] - point[X] ;
	vec1[Y] = vtx1[Y] - point[Y] ;
	len = sqrt( vec1[X] * vec1[X] + vec1[Y] * vec1[Y] ) ;
	if ( len <= 0.0 ) {
	    /* point and vertex coincide */
	    return( 1 ) ;
	}
	vec1[X] /= len ;
	vec1[Y] /= len ;

	/* check if vec1 is to "left" or "right" of vec0 */
	vec_dot = vec0[X] * vec1[X] + vec0[Y] * vec1[Y] ;
	if ( vec_dot < -1.0 ) {
	    /* point is on edge, so always add 180 degrees.  always
	     * adding is not necessarily the right answer for
	     * concave polygons and subtractive triangles.
	     */
	    angle += M_PI ;
	} else if ( vec_dot < 1.0 ) {
	    if ( vec0[X] * vec1[Y] - vec1[X] * vec0[Y] >= 0.0 ) {
		/* add angle due to dot product of vectors */
		angle += acos( vec_dot ) ;
	    } else {
		/* subtract angle due to dot product of vectors */
		angle -= acos( vec_dot ) ;
	    }
	} /* if vec_dot >= 1.0, angle does not change */

	/* get to next point */
	vtx0 = vtx1 ;
	vec0[X] = vec1[X] ;
	vec0[Y] = vec1[Y] ;
    }
    /* test if between PI and 3*PI, 5*PI and 7*PI, etc */
    inside_flag = fmod( fabs(angle) + M_PI, 4.0*M_PI ) > 2.0*M_PI ;

    return( inside_flag ) ;
}

/* ======= Weiler algorithm ============================================ */

/* Track quadrant movements using Weiler's algorithm (elsewhere in Graphics
 * Gems IV).  Algorithm has been optimized for testing purposes, but the
 * crossings algorithm is still faster.	 Included to provide timings.
 *
 * Input 2D polygon _pgon_ with _numverts_ number of vertices and test point
 * _point_, returns 1 if inside, 0 if outside.	WINDING can be defined for
 * this test.
 */

#define QUADRANT( vtx, x, y )	\
	(((vtx)[X]>(x)) ? ( ((vtx)[Y]>(y)) ? 0:3 ) : ( ((vtx)[Y]>(y)) ? 1:2 ))

#define X_INTERCEPT( v0, v1, y )	\
	( (((v1)[X]-(v0)[X])/((v1)[Y]-(v0)[Y])) * ((y)-(v0)[Y]) + (v0)[X] )

int WeilerTest( pgon, numverts, point )
double	pgon[][2] ;
int	numverts ;
double	point[2] ;
{
register int	angle, qd, next_qd, delta, j ;
register double ty, tx, *vtx0, *vtx1 ;
int	inside_flag ;

    tx = point[X] ;
    ty = point[Y] ;

    vtx0 = pgon[numverts-1] ;
    qd = QUADRANT( vtx0, tx, ty ) ;
    angle = 0 ;

    vtx1 = pgon[0] ;

    for ( j = numverts+1 ; --j ; ) {
	/* calculate quadrant and delta from last quadrant */
	next_qd = QUADRANT( vtx1, tx, ty ) ;
	delta = next_qd - qd ;

	/* adjust delta and add it to total angle sum */
	switch ( delta ) {
	    case 0:	/* do nothing */
		break ;
	    case -1:
	    case 3:
		angle-- ;
		qd = next_qd ;
		break ;

	    case 1:
	    case -3:
		angle++ ;
		qd = next_qd ;
		break ;

	    case 2:
	    case -2:
		if (X_INTERCEPT( vtx0, vtx1, ty ) > tx ) {
		    angle -= delta ;
		} else {
		    angle += delta ;
		}
		qd = next_qd ;
		break ;
	}

	/* increment for next step */
	vtx0 = vtx1 ;
	vtx1 += 2 ;
    }

#ifdef	WINDING
    /* simple windings test:  if angle != 0, then point is inside */
    inside_flag = ( angle != 0 ) ;
#else
    /* Jordan test:  if angle is +-4, 12, 20, etc then point is inside */
    inside_flag = ( (angle/4) & 0x1 ) ;
#endif

    return (inside_flag) ;
}

#undef	QUADRANT
#undef	Y_INTERCEPT

/* ======= Triangle half-plane algorithm ================================== */

/* Split the polygon into a fan of triangles and for each triangle test if
 * the point is inside of the three half-planes formed by the triangle's edges.
 *
 * Call setup with 2D polygon _pgon_ with _numverts_ number of vertices,
 * which returns a pointer to a plane set array.
 * Call testing procedure with a pointer to this array, _numverts_, and
 * test point _point_, returns 1 if inside, 0 if outside.
 * Call cleanup with pointer to plane set array to free space.
 *
 * SORT and CONVEX can be defined for this test.
 */

/* split polygons along set of x axes - call preprocess once */
pPlaneSet	PlaneSetup( pgon, numverts )
double	pgon[][2] ;
int	numverts ;
{
int	i, p1, p2 ;
double	tx, ty, vx0, vy0 ;
pPlaneSet	pps, pps_return ;
#ifdef	SORT
double	len[3], len_temp ;
int	j ;
PlaneSet	ps_temp ;
#ifdef	CONVEX
pPlaneSet	pps_new ;
pSizePlanePair p_size_pair ;
#endif
#endif

    pps = pps_return =
	    (pPlaneSet)malloc( 3 * (numverts-2) * sizeof( PlaneSet )) ;
    MALLOC_CHECK( pps ) ;
#ifdef	CONVEX
#ifdef	SORT
    p_size_pair =
	(pSizePlanePair)malloc( (numverts-2) * sizeof( SizePlanePair ) ) ;
    MALLOC_CHECK( p_size_pair ) ;
#endif
#endif

    vx0 = pgon[0][X] ;
    vy0 = pgon[0][Y] ;

    for ( p1 = 1, p2 = 2 ; p2 < numverts ; p1++, p2++ ) {
	pps->vx = vy0 - pgon[p1][Y] ;
	pps->vy = pgon[p1][X] - vx0 ;
	pps->c = pps->vx * vx0 + pps->vy * vy0 ;
#ifdef	SORT
	len[0] = pps->vx * pps->vx + pps->vy * pps->vy ;
#ifdef	CONVEX
#ifdef	HYBRID
	pps->ext_flag = ( p1 == 1 ) ;
#endif
	/* Sort triangles by areas, so compute (twice) the area here */
	p_size_pair[p1-1].pps = pps ;
	p_size_pair[p1-1].size =
			( pgon[0][X] * pgon[p1][Y] ) +
			( pgon[p1][X] * pgon[p2][Y] ) +
			( pgon[p2][X] * pgon[0][Y] ) -
			( pgon[p1][X] * pgon[0][Y] ) -
			( pgon[p2][X] * pgon[p1][Y] ) -
			( pgon[0][X] * pgon[p2][Y] ) ;
#endif
#endif
	pps++ ;
	pps->vx = pgon[p1][Y] - pgon[p2][Y] ;
	pps->vy = pgon[p2][X] - pgon[p1][X] ;
	pps->c = pps->vx * pgon[p1][X] + pps->vy * pgon[p1][Y] ;
#ifdef	SORT
	len[1] = pps->vx * pps->vx + pps->vy * pps->vy ;
#endif
#ifdef	CONVEX
#ifdef	HYBRID
	pps->ext_flag = TRUE ;
#endif
#endif
	pps++ ;
	pps->vx = pgon[p2][Y] - vy0 ;
	pps->vy = vx0 - pgon[p2][X] ;
	pps->c = pps->vx * pgon[p2][X] + pps->vy * pgon[p2][Y] ;
#ifdef	SORT
	len[2] = pps->vx * pps->vx + pps->vy * pps->vy ;
#endif
#ifdef	CONVEX
#ifdef	HYBRID
	pps->ext_flag = ( p2 == numverts-1 ) ;
#endif
#endif

	/* find an average point which must be inside of the triangle */
	tx = ( vx0 + pgon[p1][X] + pgon[p2][X] ) / 3.0 ;
	ty = ( vy0 + pgon[p1][Y] + pgon[p2][Y] ) / 3.0 ;

	/* check sense and reverse if test point is not thought to be inside
	 * first triangle
	 */
	if ( pps->vx * tx + pps->vy * ty >= pps->c ) {
	    /* back up to start of plane set */
	    pps -= 2 ;
	    /* point is thought to be outside, so reverse sense of edge
	     * normals so that it is correctly considered inside.
	     */
	    for ( i = 0 ; i < 3 ; i++ ) {
		pps->vx = -pps->vx ;
		pps->vy = -pps->vy ;
		pps->c	= -pps->c ;
		pps++ ;
	    }
	} else {
	    pps++ ;
	}

#ifdef	SORT
	/* sort the planes based on the edge lengths */
	pps -= 3 ;
	for ( i = 0 ; i < 2 ; i++ ) {
	    for ( j = i+1 ; j < 3 ; j++ ) {
		if ( len[i] < len[j] ) {
		    ps_temp = pps[i] ;
		    pps[i] = pps[j] ;
		    pps[j] = ps_temp ;
		    len_temp = len[i] ;
		    len[i] = len[j] ;
		    len[j] = len_temp ;
		}
	    }
	}
	pps += 3 ;
#endif
    }

#ifdef	CONVEX
#ifdef	SORT
    /* sort the triangles based on their areas */
    qsort( p_size_pair, numverts-2,
	    sizeof( SizePlanePair ), CompareSizePlanePairs ) ;

    /* make the plane sets match the sorted order */
    for ( i = 0, pps = pps_return
	; i < numverts-2
	; i++ ) {

	pps_new = p_size_pair[i].pps ;
	for ( j = 0 ; j < 3 ; j++, pps++, pps_new++ ) {
	    ps_temp = *pps ;
	    *pps = *pps_new ;
	    *pps_new = ps_temp ;
	}
    }
    free( p_size_pair ) ;
#endif
#endif

    return( pps_return ) ;
}

#ifdef	CONVEX
#ifdef	SORT
int CompareSizePlanePairs( p_sp0, p_sp1 )
pSizePlanePair	p_sp0, p_sp1 ;
{
    if ( p_sp0->size == p_sp1->size ) {
	return( 0 ) ;
    } else {
	return( p_sp0->size > p_sp1->size ? -1 : 1 ) ;
    }
}
#endif
#endif


/* check point for inside of three "planes" formed by triangle edges */
int PlaneTest( p_plane_set, numverts, point )
pPlaneSet	p_plane_set ;
int	numverts ;
double	point[2] ;
{
register pPlaneSet	ps ;
register int	p2 ;
#ifndef CONVEX
register int	inside_flag ;
#endif
register double tx, ty ;

    tx = point[X] ;
    ty = point[Y] ;

#ifndef CONVEX
    inside_flag = 0 ;
#endif

    for ( ps = p_plane_set, p2 = numverts-1 ; --p2 ; ) {

	if ( ps->vx * tx + ps->vy * ty < ps->c ) {
	    ps++ ;
	    if ( ps->vx * tx + ps->vy * ty < ps->c ) {
		ps++ ;
		/* note: we make the third edge have a slightly different
		 * equality condition, since this third edge is in fact
		 * the next triangle's first edge.  Not fool-proof, but
		 * it doesn't hurt (better would be to keep track of the
		 * triangle's area sign so we would know which kind of
		 * triangle this is).  Note that edge sorting nullifies
		 * this special inequality, too.
		 */
		if ( ps->vx * tx + ps->vy * ty <= ps->c ) {
		    /* point is inside polygon */
#ifdef CONVEX
		    return( 1 ) ;
#else
		    inside_flag = !inside_flag ;
#endif
		}
#ifdef	CONVEX
#ifdef	HYBRID
		/* check if outside exterior edge */
		else if ( ps->ext_flag ) return( 0 ) ;
#endif
#endif
		ps++ ;
	    } else {
#ifdef	CONVEX
#ifdef	HYBRID
		/* check if outside exterior edge */
		if ( ps->ext_flag ) return( 0 ) ;
#endif
#endif
		/* get past last two plane tests */
		ps += 2 ;
	    }
	} else {
#ifdef	CONVEX
#ifdef	HYBRID
	    /* check if outside exterior edge */
	    if ( ps->ext_flag ) return( 0 ) ;
#endif
#endif
	    /* get past all three plane tests */
	    ps += 3 ;
	}
    }

#ifdef CONVEX
    /* for convex, if we make it to here, all triangles were missed */
    return( 0 ) ;
#else
    return( inside_flag ) ;
#endif
}

void PlaneCleanup( p_plane_set )
pPlaneSet	p_plane_set ;
{
    free( p_plane_set ) ;
}

/* ======= Barycentric algorithm ========================================== */

/* Split the polygon into a fan of triangles and for each triangle test if
 * the point has barycentric coordinates which are inside the triangle.
 * Similar to Badouel's code in Graphics Gems, with a little more efficient
 * coding.
 *
 * Input 2D polygon _pgon_ with _numverts_ number of vertices and test point
 * _point_, returns 1 if inside, 0 if outside.
 */
int BarycentricTest( pgon, numverts, point )
double	pgon[][2] ;
int	numverts ;
double	point[2] ;
{
register double *pg1, *pg2, *pgend ;
register double tx, ty, u0, u1, u2, v0, v1, vx0, vy0, alpha, beta, denom ;
int	inside_flag ;

    tx = point[X] ;
    ty = point[Y] ;
    vx0 = pgon[0][X] ;
    vy0 = pgon[0][Y] ;
    u0 = tx - vx0 ;
    v0 = ty - vy0 ;

    inside_flag = 0 ;
    pgend = pgon[numverts-1] ;
    for ( pg1 = pgon[1], pg2 = pgon[2] ; pg1 != pgend ; pg1+=2, pg2+=2 ) {

	u1 = pg1[X] - vx0 ;
	if ( u1 == 0.0 ) {

	    /* 0 and 1 vertices have same X value */

	    /* zero area test - can be removed for convex testing */
	    u2 = pg2[X] - vx0 ;
	    if ( ( u2 == 0.0 ) ||

		/* compute beta and check bounds */
		/* we use "<= 0.0" so that points on the shared interior
		 * edge will (generally) be inside only one polygon.
		 */
		 ( ( beta = u0 / u2 ) <= 0.0 ) ||
		 ( beta > 1.0 ) ||

		 /* zero area test - remove for convex testing */
		 ( ( v1 = pg1[Y] - vy0 ) == 0.0 ) ||

		 /* compute alpha and check bounds */
		 ( ( alpha = ( v0 - beta *
		    ( pg2[Y] - vy0 ) ) / v1 ) < 0.0 ) ) {

		/* whew! missed! */
		goto NextTri ;
	    }

	} else {
	    /* 0 and 1 vertices have different X value */

	    /* compute denom and check for zero area triangle - check
	     * is not needed for convex polygon testing
	     */
	    u2 = pg2[X] - vx0 ;
	    v1 = pg1[Y] - vy0 ;
	    denom = ( pg2[Y] - vy0 ) * u1 - u2 * v1 ;
	    if ( ( denom == 0.0 ) ||

		/* compute beta and check bounds */
		/* we use "<= 0.0" so that points on the shared interior
		 * edge will (generally) be inside only one polygon.
		 */
		 ( ( beta = ( v0 * u1 - u0 * v1 ) / denom ) <= 0.0 ) ||
		 ( beta > 1.0 ) ||

		 /* compute alpha & check bounds */
		 ( ( alpha = ( u0 - beta * u2 ) / u1 ) < 0.0 ) ) {

		/* whew! missed! */
		goto NextTri ;
	    }
	}

	/* check gamma */
	if ( alpha + beta <= 1.0 ) {
	    /* survived */
	    inside_flag = !inside_flag ;
	}

	NextTri: ;
    }
    return( inside_flag ) ;
}

/* ======= Barycentric precompute (Spackman) algorithm ===================== */

/* Split the polygon into a fan of triangles and for each triangle test if
 * the point has barycentric coordinates which are inside the triangle.
 * Use Spackman's normalization method to precompute various parameters.
 *
 * Call setup with 2D polygon _pgon_ with _numverts_ number of vertices,
 * which returns a pointer to the array of the parameters records and the
 * number of parameter records created.
 * Call testing procedure with the first vertex in the polygon _pgon[0]_,
 * a pointer to this array, the number of parameter records, and test point
 * _point_, returns 1 if inside, 0 if outside.
 * Call cleanup with pointer to parameter record array to free space.
 *
 * SORT can be defined for this test.
 * (CONVEX could be added: see PlaneSetup and PlaneTest for method)
 */
pSpackmanSet	SpackmanSetup( pgon, numverts, p_numrec )
double	pgon[][2] ;
int	numverts ;
int	*p_numrec ;
{
int	p1, p2, degen ;
double	denom, u1, v1, *pv[3] ;
pSpackmanSet	pss, pss_return ;
#ifdef	SORT
double	u[2], v[2], len[2], *pv_temp ;
#endif

    pss = pss_return =
	    (pSpackmanSet)malloc( (numverts-2) * sizeof( SpackmanSet )) ;
    MALLOC_CHECK( pss ) ;

    degen = 0 ;

    for ( p1 = 1, p2 = 2 ; p2 < numverts ; p1++, p2++ ) {

	pv[0] = pgon[0] ;
	pv[1] = pgon[p1] ;
	pv[2] = pgon[p2] ;

#ifdef	SORT
	/* Note that sorting can cause a mismatch of alpha/beta inequality
	 * tests.  In other words, test points on an interior line between
	 * test triangles will often then be wrong.
	 */
	u[0] = pv[1][X] - pv[0][X] ;
	u[1] = pv[2][X] - pv[0][X] ;
	v[0] = pv[1][Y] - pv[0][Y] ;
	v[1] = pv[2][Y] - pv[0][Y] ;
	len[0] = u[0] * u[0] + v[0] * v[0] ;
	len[1] = u[1] * u[1] + v[1] * v[1] ;

	/* compare two edges touching anchor point and put longest first */
	/* we don't sort all three edges because the anchor point and
	 * values computed from it gets used for all triangles in the fan.
	 */
	if ( len[0] < len[1] ) {
	    pv_temp = pv[1] ;
	    pv[1] = pv[2] ;
	    pv[2] = pv_temp ;
	}
#endif

	u1 = pv[1][X] - pv[0][X] ;
	pss->u2 = pv[2][X] - pv[0][X] ;
	v1 = pv[1][Y] - pv[0][Y] ;
	pss->v2 = pv[2][Y] - pv[0][Y] ;
	pss->u1_nonzero = !( u1 == 0.0 ) ;
	if ( pss->u1_nonzero ) {
	    /* not zero, so compute inverse */
	    pss->inv_u1 = 1.0 / u1 ;
	    denom = pss->v2 * u1 - pss->u2 * v1 ;
	    if ( denom == 0.0 ) {
		/* degenerate triangle, ignore it */
		degen++ ;
		goto Skip ;
	    } else {
		pss->u1p = u1 / denom ;
		pss->v1p = v1 / denom ;
	    }
	} else {
	    if ( pss->u2 == 0.0 ) {
		/* degenerate triangle, ignore it */
		degen++ ;
		goto Skip ;
	    } else {
		/* not zero, so compute inverse */
		pss->inv_u2 = 1.0 / pss->u2 ;
		if ( v1 == 0.0 ) {
		    /* degenerate triangle, ignore it */
		    degen++ ;
		    goto Skip ;
		} else {
		    pss->inv_v1 = 1.0 / v1 ;
		}
	    }
	}

	pss++ ;
	Skip: ;
    }

    /* number of Spackman records */
    *p_numrec = numverts - degen - 2 ;
    if ( degen ) {
	pss = pss_return =
		(pSpackmanSet)realloc( pss_return,
			(numverts-2-degen) * sizeof( SpackmanSet )) ;
    }

    return( pss_return ) ;
}

/* barycentric, a la Gems I and Spackman's normalization precompute */
int SpackmanTest( anchor, p_spackman_set, numrec, point )
double	anchor[2] ;
pSpackmanSet	p_spackman_set ;
int	numrec ;
double	point[2] ;
{
register pSpackmanSet	pss ;
register int	inside_flag ;
register int	nr ;
register double tx, ty, vx0, vy0, u0, v0, alpha, beta ;

    tx = point[X] ;
    ty = point[Y] ;
    /* note that we really need only the first vertex of the polygon,
     * so do not really need to keep the whole polygon around.
     */
    vx0 = anchor[X] ;
    vy0 = anchor[Y] ;
    u0 = tx - vx0 ;
    v0 = ty - vy0 ;

    inside_flag = 0 ;

    for ( pss = p_spackman_set, nr = numrec+1 ; --nr ; pss++ ) {

	if ( pss->u1_nonzero ) {
	    /* 0 and 2 vertices have different X value */

	    /* compute beta and check bounds */
	    /* we use "<= 0.0" so that points on the shared interior edge
	     * will (generally) be inside only one polygon.
	     */
	    beta = ( v0 * pss->u1p - u0 * pss->v1p ) ;
	    if ( ( beta <= 0.0 ) || ( beta > 1.0 ) ||

		 /* compute alpha & check bounds */
		 ( ( alpha = ( u0 - beta * pss->u2 ) * pss->inv_u1 )
			< 0.0 ) ) {

		/* whew! missed! */
		goto NextTri ;
	    }
	} else {
	    /* 0 and 2 vertices have same X value */

	    /* compute beta and check bounds */
	    /* we use "<= 0.0" so that points on the shared interior edge
	     * will (generally) be inside only one polygon.
	     */
	    beta = u0 * pss->inv_u2 ;
	    if ( ( beta <= 0.0 ) || ( beta >= 1.0 ) ||

		 /* compute alpha and check bounds */
		 ( ( alpha = ( v0 - beta * pss->v2 ) * pss->inv_v1 )
			< 0.0 ) ) {

		/* whew! missed! */
		goto NextTri ;
	    }
	}

	/* check gamma */
	if ( alpha + beta <= 1.0 ) {
	    /* survived */
	    inside_flag = !inside_flag ;
	}

	NextTri: ;
    }

    return( inside_flag ) ;
}

void SpackmanCleanup( p_spackman_set )
pSpackmanSet	p_spackman_set ;
{
    free( p_spackman_set ) ;
}

/* ======= Trapezoid (bin) algorithm ======================================= */

/* Split polygons along set of y bins and sorts the edge fragments.  Testing
 * is done against these fragments.
 *
 * Call setup with 2D polygon _pgon_ with _numverts_ number of vertices, the
 * number of bins desired _bins_, and a pointer to a trapezoid structure
 * _p_trap_set_.
 * Call testing procedure with 2D polygon _pgon_ with _numverts_ number of
 * vertices, _p_trap_set_ pointer to trapezoid structure, and test point
 * _point_, returns 1 if inside, 0 if outside.
 * Call cleanup with pointer to trapezoid structure to free space.
 */
void TrapezoidSetup( pgon, numverts, bins, p_trap_set )
double	pgon[][2] ;
int	numverts ;
int	bins ;
pTrapezoidSet	p_trap_set ;
{
double	*vtx0, *vtx1, *vtxa, *vtxb, slope ;
int	i, j, bin_tot[TOT_BINS], ba, bb, id, full_cross, count ;
double	fba, fbb, vx0, vx1, dy, vy0 ;

    p_trap_set->bins = bins ;
    p_trap_set->trapz = (pTrapezoid)malloc( p_trap_set->bins *
	    sizeof(Trapezoid));
    MALLOC_CHECK( p_trap_set->trapz ) ;

    p_trap_set->minx =
    p_trap_set->maxx = pgon[0][X] ;
    p_trap_set->miny =
    p_trap_set->maxy = pgon[0][Y] ;

    for ( i = 1 ; i < numverts ; i++ ) {
	if ( p_trap_set->minx > (vx0 = pgon[i][X]) ) {
	    p_trap_set->minx = vx0 ;
	} else if ( p_trap_set->maxx < vx0 ) {
	    p_trap_set->maxx = vx0 ;
	}

	if ( p_trap_set->miny > (vy0 = pgon[i][Y]) ) {
	    p_trap_set->miny = vy0 ;
	} else if ( p_trap_set->maxy < vy0 ) {
	    p_trap_set->maxy = vy0 ;
	}
    }

    /* add a little to the bounds to ensure everything falls inside area */
    p_trap_set->miny -= EPSILON * (p_trap_set->maxy-p_trap_set->miny) ;
    p_trap_set->maxy += EPSILON * (p_trap_set->maxy-p_trap_set->miny) ;

    p_trap_set->ydelta =
	    (p_trap_set->maxy-p_trap_set->miny) / (double)p_trap_set->bins ;
    p_trap_set->inv_ydelta = 1.0 / p_trap_set->ydelta ;

    /* find how many locations to allocate for each bin */
    for ( i = 0 ; i < p_trap_set->bins ; i++ ) {
	bin_tot[i] = 0 ;
    }

    vtx0 = pgon[numverts-1] ;
    for ( i = 0 ; i < numverts ; i++ ) {
	vtx1 = pgon[i] ;

	/* skip if Y's identical (edge has no effect) */
	if ( vtx0[Y] != vtx1[Y] ) {

	    if ( vtx0[Y] < vtx1[Y] ) {
		vtxa = vtx0 ;
		vtxb = vtx1 ;
	    } else {
		vtxa = vtx1 ;
		vtxb = vtx0 ;
	    }
	    ba = (int)(( vtxa[Y]-p_trap_set->miny ) * p_trap_set->inv_ydelta) ;
	    fbb = ( vtxb[Y] - p_trap_set->miny ) * p_trap_set->inv_ydelta ;
	    bb = (int)fbb ;
	    /* if high vertex ends on a boundary, don't go into next boundary */
	    if ( fbb - (double)bb == 0.0 ) {
		bb-- ;
	    }

	    /* mark the bins with this edge */
	    for ( j = ba ; j <= bb ; j++ ) {
		bin_tot[j]++ ;
	    }
	}

	vtx0 = vtx1 ;
    }

    /* allocate the bin contents and fill in some basics */
    for ( i = 0 ; i < p_trap_set->bins ; i++ ) {
	p_trap_set->trapz[i].edge_set =
		(pEdge*)malloc( bin_tot[i] * sizeof(pEdge) ) ;
	MALLOC_CHECK( p_trap_set->trapz[i].edge_set ) ;
	for ( j = 0 ; j < bin_tot[i] ; j++ ) {
	    p_trap_set->trapz[i].edge_set[j] =
		(pEdge)malloc( sizeof(Edge) ) ;
	    MALLOC_CHECK( p_trap_set->trapz[i].edge_set[j] ) ;
	}

	/* start these off at some awful values; refined below */
	p_trap_set->trapz[i].minx = p_trap_set->maxx ;
	p_trap_set->trapz[i].maxx = p_trap_set->minx ;
	p_trap_set->trapz[i].count = 0 ;
    }

    /* now go through list yet again, putting edges in bins */
    vtx0 = pgon[numverts-1] ;
    id = numverts-1 ;
    for ( i = 0 ; i < numverts ; i++ ) {
	vtx1 = pgon[i] ;

	/* we can skip edge if Y's are equal */
	if ( vtx0[Y] != vtx1[Y] ) {
	    if ( vtx0[Y] < vtx1[Y] ) {
		vtxa = vtx0 ;
		vtxb = vtx1 ;
	    } else {
		vtxa = vtx1 ;
		vtxb = vtx0 ;
	    }
	    fba = ( vtxa[Y] - p_trap_set->miny ) * p_trap_set->inv_ydelta ;
	    ba = (int)fba ;
	    fbb = ( vtxb[Y] - p_trap_set->miny ) * p_trap_set->inv_ydelta ;
	    bb = (int)fbb ;
	    /* if high vertex ends on a boundary, don't go into it */
	    if ( fbb == (double)bb ) {
		bb-- ;
	    }

	    vx0 = vtxa[X] ;
	    dy = vtxa[Y] - vtxb[Y] ;
	    slope = p_trap_set->ydelta * ( vtxa[X] - vtxb[X] ) / dy ;

	    /* set vx1 in case loop is not entered */
	    vx1 = vx0 ;
	    full_cross = 0 ;

	    for ( j = ba ; j < bb ; j++, vx0 = vx1 ) {
		/* could increment vx1, but for greater accuracy recompute it */
		vx1 = vtxa[X] + ( (double)(j+1) - fba ) * slope ;

		count = p_trap_set->trapz[j].count++ ;
		p_trap_set->trapz[j].edge_set[count]->id = id ;
		p_trap_set->trapz[j].edge_set[count]->full_cross = full_cross;
		TrapBound( j, count, vx0, vx1, p_trap_set ) ;
		full_cross = 1 ;
	    }

	    /* at last bin - fill as above, but with vx1 = vtxb[X] */
	    vx0 = vx1 ;
	    vx1 = vtxb[X] ;
	    count = p_trap_set->trapz[bb].count++ ;
	    p_trap_set->trapz[bb].edge_set[count]->id = id ;
	    /* the last bin is never a full crossing */
	    p_trap_set->trapz[bb].edge_set[count]->full_cross = 0 ;
	    TrapBound( bb, count, vx0, vx1, p_trap_set ) ;
	}

	vtx0 = vtx1 ;
	id = i ;
    }

    /* finally, sort the bins' contents by minx */
    for ( i = 0 ; i < p_trap_set->bins ; i++ ) {
	qsort( p_trap_set->trapz[i].edge_set, p_trap_set->trapz[i].count,
		sizeof(pEdge), CompareEdges ) ;
    }
}

void TrapBound( j, count, vx0, vx1, p_trap_set )
int	j, count ;
double	vx0, vx1 ;
pTrapezoidSet	p_trap_set ;
{
double	xt ;

    if ( vx0 > vx1 ) {
	xt = vx0 ;
	vx0 = vx1 ;
	vx1 = xt ;
    }

    if ( p_trap_set->trapz[j].minx > vx0 ) {
	p_trap_set->trapz[j].minx = vx0 ;
    }
    if ( p_trap_set->trapz[j].maxx < vx1 ) {
	p_trap_set->trapz[j].maxx = vx1 ;
    }
    p_trap_set->trapz[j].edge_set[count]->minx = vx0 ;
    p_trap_set->trapz[j].edge_set[count]->maxx = vx1 ;
}

/* used by qsort to sort */
int CompareEdges( u, v )
pEdge	*u, *v ;
{
    if ( (*u)->minx == (*v)->minx ) {
	return( 0 ) ;
    } else {
	return( (*u)->minx < (*v)->minx ? -1 : 1 ) ;
    }
}

int TrapezoidTest( pgon, numverts, p_trap_set, point )
double	pgon[][2] ;
int	numverts ;
pTrapezoidSet	p_trap_set ;
double	point[2] ;
{
int	j, b, count, id ;
double	tx, ty, *vtx0, *vtx1 ;
pEdge	*pp_bin ;
pTrapezoid	p_trap ;
int	inside_flag ;

    inside_flag = 0 ;

    /* first, is point inside bounding rectangle? */
    if ( ( ty = point[Y] ) < p_trap_set->miny ||
	 ty >= p_trap_set->maxy ||
	 ( tx = point[X] ) < p_trap_set->minx ||
	 tx >= p_trap_set->maxx ) {

	/* outside of box */
	return( 0 ) ;
    }

    /* what bin are we in? */
    b = ( ty - p_trap_set->miny ) * p_trap_set->inv_ydelta ;

    /* find if we're inside this bin's bounds */
    if ( tx < (p_trap = &p_trap_set->trapz[b])->minx ||
	 tx > p_trap->maxx ) {

	/* outside of box */
	return( 0 ) ;
    }

    /* now search bin for crossings */
    pp_bin = p_trap->edge_set ;
    count = p_trap->count ;
    for ( j = 0 ; j < count ; j++, pp_bin++ ) {
	if ( tx < (*pp_bin)->minx ) {

	    /* all remaining edges are to right of point, so test them */
	    do {
		if ( (*pp_bin)->full_cross ) {
		    inside_flag = !inside_flag ;
		} else {
		    id = (*pp_bin)->id ;
		    if ( ( ty <= pgon[id][Y] ) !=
			    ( ty <= pgon[(id+1)%numverts][Y] ) ) {

			/* point crosses edge in Y, so must cross */
			inside_flag = !inside_flag ;
		    }
		}
		pp_bin++ ;
	    } while ( ++j < count ) ;
	    goto Exit;

	} else if ( tx < (*pp_bin)->maxx ) {
	    /* edge is overlapping point in X, check it */
	    id = (*pp_bin)->id ;
	    vtx0 = pgon[id] ;
	    vtx1 = pgon[(id+1)%numverts] ;

	    if ( (*pp_bin)->full_cross ||
		 ( ty <= vtx0[Y] ) != ( ty <= vtx1[Y] ) ) {

		/* edge crosses in Y, so have to do full crossings test */
		if ( (vtx0[X] -
		    (vtx0[Y] - ty )*
			( vtx1[X]-vtx0[X])/(vtx1[Y]-vtx0[Y])) >= tx ) {
		    inside_flag = !inside_flag ;
		}
	    }

	} /* else edge is to left of point, ignore it */
    }

    Exit:
    return( inside_flag ) ;
}

void TrapezoidCleanup( p_trap_set )
pTrapezoidSet	p_trap_set ;
{
int	i, j, count ;

    for ( i = 0 ; i < p_trap_set->bins ; i++ ) {
	/* all of these should have bin sets, but check just in case */
	if ( p_trap_set->trapz[i].edge_set ) {
	    count = p_trap_set->trapz[i].count ;
	    for ( j = 0 ; j < count ; j++ ) {
		if ( p_trap_set->trapz[i].edge_set[j] ) {
		    free( p_trap_set->trapz[i].edge_set[j] ) ;
		}
	    }
	    free( p_trap_set->trapz[i].edge_set ) ;
	}
    }
    free( p_trap_set->trapz ) ;
}

/* ======= Grid algorithm ================================================= */

/* Impose a grid upon the polygon and test only the local edges against the
 * point.
 *
 * Call setup with 2D polygon _pgon_ with _numverts_ number of vertices,
 * grid resolution _resolution_ and a pointer to a grid structure _p_gs_.
 * Call testing procedure with a pointer to this array and test point _point_,
 * returns 1 if inside, 0 if outside.
 * Call cleanup with pointer to grid structure to free space.
 */

/* Strategy for setup:
 *   Get bounds of polygon, allocate grid.
 *   "Walk" each edge of the polygon and note which edges have been crossed
 *     and what cells are entered (points on a grid edge are always considered
 *     to be above that edge).	Keep a record of the edges overlapping a cell.
 *     For cells with edges, determine if any cell border has no edges passing
 *     through it and so can be used for shooting a test ray.
 *     Keep track of the parity of the x (horizontal) grid cell borders for
 *     use in determining whether the grid corners are inside or outside.
 */
void GridSetup( pgon, numverts, resolution, p_gs )
double	pgon[][2] ;
int	numverts ;
int	resolution ;
pGridSet	p_gs ;
{
double	*vtx0, *vtx1, *vtxa, *vtxb, *p_gl ;
int	i, j, gc_clear_flags ;
double	vx0, vx1, vy0, vy1, gxdiff, gydiff, eps ;
pGridCell	p_gc, p_ngc ;
double	xdiff, ydiff, tmax, inv_x, inv_y, xdir, ydir, t_near, tx, ty ;
double	tgcx, tgcy ;
int	gcx, gcy, sign_x ;
int	y_flag, io_state ;

    p_gs->xres = p_gs->yres = resolution ;
    p_gs->tot_cells = p_gs->xres * p_gs->yres ;
    p_gs->glx = (double *)malloc( (p_gs->xres+1) * sizeof(double));
    MALLOC_CHECK( p_gs->glx ) ;
    p_gs->gly = (double *)malloc( (p_gs->yres+1) * sizeof(double));
    MALLOC_CHECK( p_gs->gly ) ;
    p_gs->gc = (pGridCell)malloc( p_gs->tot_cells * sizeof(GridCell));
    MALLOC_CHECK( p_gs->gc ) ;

    p_gs->minx =
    p_gs->maxx = pgon[0][X] ;
    p_gs->miny =
    p_gs->maxy = pgon[0][Y] ;

    /* find bounds of polygon */
    for ( i = 1 ; i < numverts ; i++ ) {
	vx0 = pgon[i][X] ;
	if ( p_gs->minx > vx0 ) {
	    p_gs->minx = vx0 ;
	} else if ( p_gs->maxx < vx0 ) {
	    p_gs->maxx = vx0 ;
	}

	vy0 = pgon[i][Y] ;
	if ( p_gs->miny > vy0 ) {
	    p_gs->miny = vy0 ;
	} else if ( p_gs->maxy < vy0 ) {
	    p_gs->maxy = vy0 ;
	}
    }

    /* add a little to the bounds to ensure everything falls inside area */
    gxdiff = p_gs->maxx - p_gs->minx ;
    gydiff = p_gs->maxy - p_gs->miny ;
    p_gs->minx -= EPSILON * gxdiff ;
    p_gs->maxx += EPSILON * gxdiff ;
    p_gs->miny -= EPSILON * gydiff ;
    p_gs->maxy += EPSILON * gydiff ;

    /* avoid roundoff problems near corners by not getting too close to them */
    eps = 1e-9 * ( gxdiff + gydiff ) ;

    /* use the new bounds to compute cell widths */
    TryAgain:
    p_gs->xdelta =
	    (p_gs->maxx-p_gs->minx) / (double)p_gs->xres ;
    p_gs->inv_xdelta = 1.0 / p_gs->xdelta ;

    p_gs->ydelta =
	    (p_gs->maxy-p_gs->miny) / (double)p_gs->yres ;
    p_gs->inv_ydelta = 1.0 / p_gs->ydelta ;

    for ( i = 0, p_gl = p_gs->glx ; i < p_gs->xres ; i++ ) {
	*p_gl++ = p_gs->minx + i * p_gs->xdelta ;
    }
    /* make last grid corner precisely correct */
    *p_gl = p_gs->maxx ;

    for ( i = 0, p_gl = p_gs->gly ; i < p_gs->yres ; i++ ) {
	*p_gl++ = p_gs->miny + i * p_gs->ydelta ;
    }
    *p_gl = p_gs->maxy ;

    for ( i = 0, p_gc = p_gs->gc ; i < p_gs->tot_cells ; i++, p_gc++ ) {
	p_gc->tot_edges = 0 ;
	p_gc->gc_flags = 0x0 ;
	p_gc->gr = NULL ;
    }

    /* loop through edges and insert into grid structure */
    vtx0 = pgon[numverts-1] ;
    for ( i = 0 ; i < numverts ; i++ ) {
	vtx1 = pgon[i] ;

	if ( vtx0[Y] < vtx1[Y] ) {
	    vtxa = vtx0 ;
	    vtxb = vtx1 ;
	} else {
	    vtxa = vtx1 ;
	    vtxb = vtx0 ;
	}

	/* Set x variable for the direction of the ray */
	xdiff = vtxb[X] - vtxa[X] ;
	ydiff = vtxb[Y] - vtxa[Y] ;
	tmax = sqrt( xdiff * xdiff + ydiff * ydiff ) ;

	/* if edge is of 0 length, ignore it (useless edge) */
	if ( tmax == 0.0 ) goto NextEdge ;

	xdir = xdiff / tmax ;
	ydir = ydiff / tmax ;

	gcx = (int)(( vtxa[X] - p_gs->minx ) * p_gs->inv_xdelta) ;
	gcy = (int)(( vtxa[Y] - p_gs->miny ) * p_gs->inv_ydelta) ;

	/* get information about slopes of edge, etc */
	if ( vtxa[X] == vtxb[X] ) {
	    sign_x = 0 ;
	    tx = HUGE ;
	} else {
	    inv_x = tmax / xdiff ;
	    tx = p_gs->xdelta * (double)gcx + p_gs->minx - vtxa[X] ;
	    if ( vtxa[X] < vtxb[X] ) {
		sign_x = 1 ;
		tx += p_gs->xdelta ;
		tgcx = p_gs->xdelta * inv_x ;
	    } else {
		sign_x = -1 ;
		tgcx = -p_gs->xdelta * inv_x ;
	    }
	    tx *= inv_x ;
	}

	if ( vtxa[Y] == vtxb[Y] ) {
	    ty = HUGE ;
	} else {
	    inv_y = tmax / ydiff ;
	    ty = (p_gs->ydelta * (double)(gcy+1) + p_gs->miny - vtxa[Y])
		* inv_y ;
	    tgcy = p_gs->ydelta * inv_y ;
	}

	p_gc = &p_gs->gc[gcy*p_gs->xres+gcx] ;

	vx0 = vtxa[X] ;
	vy0 = vtxa[Y] ;

	t_near = 0.0 ;

	do {
	    /* choose the next boundary, but don't move yet */
	    if ( tx <= ty ) {
		gcx += sign_x ;

		ty -= tx ;
		t_near += tx ;
		tx = tgcx ;

		/* note which edge is hit when leaving this cell */
		if ( t_near < tmax ) {
		    if ( sign_x > 0 ) {
			p_gc->gc_flags |= GC_R_EDGE_HIT ;
			vx1 = p_gs->glx[gcx] ;
		    } else {
			p_gc->gc_flags |= GC_L_EDGE_HIT ;
			vx1 = p_gs->glx[gcx+1] ;
		    }

		    /* get new location */
		    vy1 = t_near * ydir + vtxa[Y] ;
		} else {
		    /* end of edge, so get exact value */
		    vx1 = vtxb[X] ;
		    vy1 = vtxb[Y] ;
		}

		y_flag = FALSE ;

	    } else {

		gcy++ ;

		tx -= ty ;
		t_near += ty ;
		ty = tgcy ;

		/* note top edge is hit when leaving this cell */
		if ( t_near < tmax ) {
		    p_gc->gc_flags |= GC_T_EDGE_HIT ;
		    /* this toggles the parity bit */
		    p_gc->gc_flags ^= GC_T_EDGE_PARITY ;

		    /* get new location */
		    vx1 = t_near * xdir + vtxa[X] ;
		    vy1 = p_gs->gly[gcy] ;
		} else {
		    /* end of edge, so get exact value */
		    vx1 = vtxb[X] ;
		    vy1 = vtxb[Y] ;
		}

		y_flag = TRUE ;
	    }

	    /* check for corner crossing, then mark the cell we're in */
	    if ( !AddGridRecAlloc( p_gc, vx0, vy0, vx1, vy1, eps ) ) {
		/* warning, danger - we have just crossed a corner.
		 * There are all kinds of topological messiness we could
		 * do to get around this case, but they're a headache.
		 * The simplest recovery is just to change the extents a bit
		 * and redo the meshing, so that hopefully no edges will
		 * perfectly cross a corner.  Since it's a preprocess, we
		 * don't care too much about the time to do it.
		 */

		/* clean out all grid records */
		for ( i = 0, p_gc = p_gs->gc
		    ; i < p_gs->tot_cells
		    ; i++, p_gc++ ) {

		    if ( p_gc->gr ) {
			free( p_gc->gr ) ;
		    }
		}

		/* make the bounding box ever so slightly larger, hopefully
		 * changing the alignment of the corners.
		 */
		p_gs->minx -= EPSILON * gxdiff * 0.24 ;
		p_gs->miny -= EPSILON * gydiff * 0.10 ;

		/* yes, it's the dreaded goto - run in fear for your lives! */
		goto TryAgain ;
	    }

	    if ( t_near < tmax ) {
		/* note how we're entering the next cell */
		/* TBD: could be done faster by incrementing index in the
		 * incrementing code, above */
		p_gc = &p_gs->gc[gcy*p_gs->xres+gcx] ;

		if ( y_flag ) {
		    p_gc->gc_flags |= GC_B_EDGE_HIT ;
		    /* this toggles the parity bit */
		    p_gc->gc_flags ^= GC_B_EDGE_PARITY ;
		} else {
		    p_gc->gc_flags |=
			( sign_x > 0 ) ? GC_L_EDGE_HIT : GC_R_EDGE_HIT ;
		}
	    }

	    vx0 = vx1 ;
	    vy0 = vy1 ;
	}
	/* have we gone further than the end of the edge? */
	while ( t_near < tmax ) ;

	NextEdge:
	vtx0 = vtx1 ;
    }

    /* the grid is all set up, now set up the inside/outside value of each
     * corner.
     */
    p_gc = p_gs->gc ;
    p_ngc = &p_gs->gc[p_gs->xres] ;

    /* we know the bottom and top rows are all outside, so no flag is set */
    for ( i = 1; i < p_gs->yres ; i++ ) {
	/* start outside */
	io_state = 0x0 ;

	for ( j = 0; j < p_gs->xres ; j++ ) {

	    if ( io_state ) {
		/* change cell left corners to inside */
		p_gc->gc_flags |= GC_TL_IN ;
		p_ngc->gc_flags |= GC_BL_IN ;
	    }

	    if ( p_gc->gc_flags & GC_T_EDGE_PARITY ) {
		io_state = !io_state ;
	    }

	    if ( io_state ) {
		/* change cell right corners to inside */
		p_gc->gc_flags |= GC_TR_IN ;
		p_ngc->gc_flags |= GC_BR_IN ;
	    }

	    p_gc++ ;
	    p_ngc++ ;
	}
    }

    p_gc = p_gs->gc ;
    for ( i = 0; i < p_gs->tot_cells ; i++ ) {

	/* reverse parity of edge clear (1==edge clear) */
	gc_clear_flags = p_gc->gc_flags ^ GC_ALL_EDGE_CLEAR ;
	if ( gc_clear_flags & GC_L_EDGE_CLEAR ) {
	    p_gc->gc_flags |= GC_AIM_L ;
	} else
	if ( gc_clear_flags & GC_B_EDGE_CLEAR ) {
	    p_gc->gc_flags |= GC_AIM_B ;
	} else
	if ( gc_clear_flags & GC_R_EDGE_CLEAR ) {
	    p_gc->gc_flags |= GC_AIM_R ;
	} else
	if ( gc_clear_flags & GC_T_EDGE_CLEAR ) {
	    p_gc->gc_flags |= GC_AIM_T ;
	} else {
	    /* all edges have something on them, do full test */
	    p_gc->gc_flags |= GC_AIM_C ;
	}
	p_gc++ ;
    }
}

int AddGridRecAlloc( p_gc, xa, ya, xb, yb, eps )
pGridCell	p_gc ;
double	xa,ya,xb,yb,eps ;
{
pGridRec	p_gr ;
double		slope, inv_slope ;

    if ( Near(ya, yb, eps) ) {
	if ( Near(xa, xb, eps) ) {
	    /* edge is 0 length, so get rid of it */
	    return( FALSE ) ;
	} else {
	    /* horizontal line */
	    slope = HUGE ;
	    inv_slope = 0.0 ;
	}
    } else {
	if ( Near(xa, xb, eps) ) {
	    /* vertical line */
	    slope = 0.0 ;
	    inv_slope = HUGE ;
	} else {
	    slope = (xb-xa)/(yb-ya) ;
	    inv_slope = (yb-ya)/(xb-xa) ;
	}
    }

    p_gc->tot_edges++ ;
    if ( p_gc->tot_edges <= 1 ) {
	p_gc->gr = (pGridRec)malloc( sizeof(GridRec) ) ;
    } else {
	p_gc->gr = (pGridRec)realloc( p_gc->gr,
		p_gc->tot_edges * sizeof(GridRec) ) ;
    }
    MALLOC_CHECK( p_gc->gr ) ;
    p_gr = &p_gc->gr[p_gc->tot_edges-1] ;

    p_gr->slope = slope ;
    p_gr->inv_slope = inv_slope ;

    p_gr->xa = xa ;
    p_gr->ya = ya ;
    if ( xa <= xb ) {
	p_gr->minx = xa ;
	p_gr->maxx = xb ;
    } else {
	p_gr->minx = xb ;
	p_gr->maxx = xa ;
    }
    if ( ya <= yb ) {
	p_gr->miny = ya ;
	p_gr->maxy = yb ;
    } else {
	p_gr->miny = yb ;
	p_gr->maxy = ya ;
    }

    /* P2 - P1 */
    p_gr->ax = xb - xa ;
    p_gr->ay = yb - ya ;

    return( TRUE ) ;
}

/* Test point against grid and edges in the cell (if any).  Algorithm:
 *    Check bounding box; if outside then return.
 *    Check cell point is inside; if simple inside or outside then return.
 *    Find which edge or corner is considered to be the best for testing and
 *	  send a test ray towards it, counting the crossings.  Add in the
 *	  state of the edge or corner the ray went to and so determine the
 *	  state of the point (inside or outside).
 */
int GridTest( p_gs, point )
register pGridSet	p_gs ;
double	point[2] ;
{
int	j, count, init_flag ;
pGridCell	p_gc ;
pGridRec	p_gr ;
double	tx, ty, xcell, ycell, bx,by,cx,cy, cornerx, cornery ;
double	alpha, beta, denom ;
unsigned short	gc_flags ;
int	inside_flag ;

    /* first, is point inside bounding rectangle? */
    if ( ( ty = point[Y] ) < p_gs->miny ||
	 ty >= p_gs->maxy ||
	 ( tx = point[X] ) < p_gs->minx ||
	 tx >= p_gs->maxx ) {

	/* outside of box */
	inside_flag = FALSE ;
    } else {

	/* what cell are we in? */
	ycell = ( ty - p_gs->miny ) * p_gs->inv_ydelta ;
	xcell = ( tx - p_gs->minx ) * p_gs->inv_xdelta ;
	p_gc = &p_gs->gc[((int)ycell)*p_gs->xres + (int)xcell] ;

	/* is cell simple? */
	count = p_gc->tot_edges ;
	if ( count ) {
	    /* no, so find an edge which is free. */
	    gc_flags = p_gc->gc_flags ;
	    p_gr = p_gc->gr ;
	    switch( gc_flags & GC_AIM ) {
	    case GC_AIM_L:
		/* left edge is clear, shoot X- ray */
		/* note - this next statement requires that GC_BL_IN is 1 */
		inside_flag = gc_flags & GC_BL_IN ;
		for ( j = count+1 ; --j ; p_gr++ ) {
		    /* test if y is between edges */
		    if ( ty >= p_gr->miny && ty < p_gr->maxy ) {
			if ( tx > p_gr->maxx ) {
			    inside_flag = !inside_flag ;
			} else if ( tx > p_gr->minx ) {
			    /* full computation */
			    if ( ( p_gr->xa -
				( p_gr->ya - ty ) * p_gr->slope ) < tx ) {
				inside_flag = !inside_flag ;
			    }
			}
		    }
		}
		break ;

	    case GC_AIM_B:
		/* bottom edge is clear, shoot Y+ ray */
		/* note - this next statement requires that GC_BL_IN is 1 */
		inside_flag = gc_flags & GC_BL_IN ;
		for ( j = count+1 ; --j ; p_gr++ ) {
		    /* test if x is between edges */
		    if ( tx >= p_gr->minx && tx < p_gr->maxx ) {
			if ( ty > p_gr->maxy ) {
			    inside_flag = !inside_flag ;
			} else if ( ty > p_gr->miny ) {
			    /* full computation */
			    if ( ( p_gr->ya - ( p_gr->xa - tx ) *
				    p_gr->inv_slope ) < ty ) {
				inside_flag = !inside_flag ;
			    }
			}
		    }
		}
		break ;

	    case GC_AIM_R:
		/* right edge is clear, shoot X+ ray */
		inside_flag = (gc_flags & GC_TR_IN) ? 1 : 0 ;

		/* TBD: Note, we could have sorted the edges to be tested
		 * by miny or somesuch, and so be able to cut testing
		 * short when the list's miny > point.y .
		 */
		for ( j = count+1 ; --j ; p_gr++ ) {
		    /* test if y is between edges */
		    if ( ty >= p_gr->miny && ty < p_gr->maxy ) {
			if ( tx <= p_gr->minx ) {
			    inside_flag = !inside_flag ;
			} else if ( tx <= p_gr->maxx ) {
			    /* full computation */
			    if ( ( p_gr->xa -
				( p_gr->ya - ty ) * p_gr->slope ) >= tx ) {
				inside_flag = !inside_flag ;
			    }
			}
		    }
		}
		break ;

	    case GC_AIM_T:
		/* top edge is clear, shoot Y+ ray */
		inside_flag = (gc_flags & GC_TR_IN) ? 1 : 0 ;
		for ( j = count+1 ; --j ; p_gr++ ) {
		    /* test if x is between edges */
		    if ( tx >= p_gr->minx && tx < p_gr->maxx ) {
			if ( ty <= p_gr->miny ) {
			    inside_flag = !inside_flag ;
			} else if ( ty <= p_gr->maxy ) {
			    /* full computation */
			    if ( ( p_gr->ya - ( p_gr->xa - tx ) *
				    p_gr->inv_slope ) >= ty ) {
				inside_flag = !inside_flag ;
			    }
			}
		    }
		}
		break ;

	    case GC_AIM_C:
		/* no edge is clear, bite the bullet and test
		 * against the bottom left corner.
		 * We use Franklin Antonio's algorithm (Graphics Gems III).
		 */
		/* TBD: Faster yet might be to test against the closest
		 * corner to the cell location, but our hope is that we
		 * rarely need to do this testing at all.
		 */
		inside_flag = ((gc_flags & GC_BL_IN) == GC_BL_IN) ;
		init_flag = TRUE ;

		/* get lower left corner coordinate */
		cornerx = p_gs->glx[(int)xcell] ;
		cornery = p_gs->gly[(int)ycell] ;
		for ( j = count+1 ; --j ; p_gr++ ) {

		    /* quick out test: if test point is
		     * less than minx & miny, edge cannot overlap.
		     */
		    if ( tx >= p_gr->minx && ty >= p_gr->miny ) {

			/* quick test failed, now check if test point and
			 * corner are on different sides of edge.
			 */
			if ( init_flag ) {
			    /* Compute these at most once for test */
			    /* P3 - P4 */
			    bx = tx - cornerx ;
			    by = ty - cornery ;
			    init_flag = FALSE ;
			}
			denom = p_gr->ay * bx - p_gr->ax * by ;
			if ( denom != 0.0 ) {
			    /* lines are not collinear, so continue */
			    /* P1 - P3 */
			    cx = p_gr->xa - tx ;
			    cy = p_gr->ya - ty ;
			    alpha = by * cx - bx * cy ;
			    if ( denom > 0.0 ) {
				if ( alpha < 0.0 || alpha >= denom ) {
				    /* test edge not hit */
				    goto NextEdge ;
				}
				beta = p_gr->ax * cy - p_gr->ay * cx ;
				if ( beta < 0.0 || beta >= denom ) {
				    /* polygon edge not hit */
				    goto NextEdge ;
				}
			    } else {
				if ( alpha > 0.0 || alpha <= denom ) {
				    /* test edge not hit */
				    goto NextEdge ;
				}
				beta = p_gr->ax * cy - p_gr->ay * cx ;
				if ( beta > 0.0 || beta <= denom ) {
				    /* polygon edge not hit */
				    goto NextEdge ;
				}
			    }
			    inside_flag = !inside_flag ;
			}

		    }
		    NextEdge: ;
		}
		break ;
	    }
	} else {
	    /* simple cell, so if lower left corner is in,
	     * then cell is inside.
	     */
	    inside_flag = p_gc->gc_flags & GC_BL_IN ;
	}
    }

    return( inside_flag ) ;
}

void GridCleanup( p_gs )
pGridSet	p_gs ;
{
int	i ;
pGridCell	p_gc ;

    for ( i = 0, p_gc = p_gs->gc
	; i < p_gs->tot_cells
	; i++, p_gc++ ) {

	if ( p_gc->gr ) {
	    free( p_gc->gr ) ;
	}
    }
    free( p_gs->glx ) ;
    free( p_gs->gly ) ;
    free( p_gs->gc ) ;
}

/* ======= Exterior (convex only) algorithm =============================== */

/* Test the edges of the convex polygon against the point.  If the point is
 * outside any edge, the point is outside the polygon.
 *
 * Call setup with 2D polygon _pgon_ with _numverts_ number of vertices,
 * which returns a pointer to a plane set array.
 * Call testing procedure with a pointer to this array, _numverts_, and
 * test point _point_, returns 1 if inside, 0 if outside.
 * Call cleanup with pointer to plane set array to free space.
 *
 * RANDOM can be defined for this test.
 * CONVEX must be defined for this test; it is not usable for general polygons.
 */

#ifdef	CONVEX
/* make exterior plane set */
pPlaneSet ExteriorSetup( pgon, numverts )
double	pgon[][2] ;
int	numverts ;
{
int	p1, p2, flip_edge ;
pPlaneSet	pps, pps_return ;
#ifdef	RANDOM
int	i, ind ;
PlaneSet	ps_temp ;
#endif

    pps = pps_return =
	    (pPlaneSet)malloc( numverts * sizeof( PlaneSet )) ;
    MALLOC_CHECK( pps ) ;

    /* take cross product of vertex to find handedness */
    flip_edge = (pgon[0][X] - pgon[1][X]) * (pgon[1][Y] - pgon[2][Y] ) >
		(pgon[0][Y] - pgon[1][Y]) * (pgon[1][X] - pgon[2][X] ) ;

    /* Generate half-plane boundary equations now for faster testing later.
     * vx & vy are the edge's normal, c is the offset from the origin.
     */
    for ( p1 = numverts-1, p2 = 0 ; p2 < numverts ; p1 = p2, p2++, pps++ ) {
	pps->vx = pgon[p1][Y] - pgon[p2][Y] ;
	pps->vy = pgon[p2][X] - pgon[p1][X] ;
	pps->c = pps->vx * pgon[p1][X] + pps->vy * pgon[p1][Y] ;

	/* check sense and reverse plane edge if need be */
	if ( flip_edge ) {
	    pps->vx = -pps->vx ;
	    pps->vy = -pps->vy ;
	    pps->c  = -pps->c ;
	}
    }

#ifdef	RANDOM
    /* Randomize the order of the edges to improve chance of early out */
    /* There are better orders, but the default order is the worst */
    for ( i = 0, pps = pps_return
	; i < numverts
	; i++ ) {

	ind = (int)(RAN01() * numverts ) ;
	if ( ( ind < 0 ) || ( ind >= numverts ) ) {
	    fprintf( stderr,
		"Yikes, the random number generator is returning values\n" ) ;
	    fprintf( stderr,
		"outside the range [0.0,1.0), so please fix the code!\n" ) ;
	    ind = 0 ;
	}

	/* swap edges */
	ps_temp = *pps ;
	*pps = pps_return[ind] ;
	pps_return[ind] = ps_temp ;
    }
#endif

    return( pps_return ) ;
}

/* Check point for outside of all planes */
/* note that we don't need "pgon", since it's been processed into
 * its corresponding PlaneSet.
 */
int ExteriorTest( p_ext_set, numverts, point )
pPlaneSet	p_ext_set ;
int	numverts ;
double	point[2] ;
{
register PlaneSet	*pps ;
register int	p0 ;
register double tx, ty ;
int	inside_flag ;

    tx = point[X] ;
    ty = point[Y] ;

    for ( p0 = numverts+1, pps = p_ext_set ; --p0 ; pps++ ) {

	/* test if the point is outside this edge */
	if ( pps->vx * tx + pps->vy * ty > pps->c ) {
	    return( 0 ) ;
	}
    }
    /* if we make it to here, we were inside all edges */
    return( 1 ) ;
}

void ExteriorCleanup( p_ext_set )
pPlaneSet	p_ext_set ;
{
    free( p_ext_set ) ;
}
#endif

/* ======= Inclusion (convex only) algorithm ============================== */

/* Create an efficiency structure (see Preparata) for the convex polygon which
 * allows binary searching to find which edge to test the point against.  This
 * algorithm is O(log n).
 *
 * Call setup with 2D polygon _pgon_ with _numverts_ number of vertices,
 * which returns a pointer to an inclusion anchor structure.
 * Call testing procedure with a pointer to this structure and test point
 * _point_, returns 1 if inside, 0 if outside.
 * Call cleanup with pointer to inclusion anchor structure to free space.
 *
 * CONVEX must be defined for this test; it is not usable for general polygons.
 */

#ifdef	CONVEX
/* make inclusion wedge set */
pInclusionAnchor InclusionSetup( pgon, numverts )
double	pgon[][2] ;
int	numverts ;
{
int	pc, p1, p2, flip_edge ;
double	ax,ay, qx,qy, wx,wy, len ;
pInclusionAnchor pia ;
pInclusionSet	pis ;

    /* double the first edge to avoid needing modulo during test search */
    pia = (pInclusionAnchor)malloc( sizeof( InclusionAnchor )) ;
    MALLOC_CHECK( pia ) ;
    pis = pia->pis =
	    (pInclusionSet)malloc( (numverts+1) * sizeof( InclusionSet )) ;
    MALLOC_CHECK( pis ) ;

    pia->hi_start = numverts - 1 ;

    /* get average point to make wedges from */
    qx = qy = 0.0 ;
    for ( p2 = 0 ; p2 < numverts ; p2++ ) {
	qx += pgon[p2][X] ;
	qy += pgon[p2][Y] ;
    }
    pia->qx = qx /= (double)numverts ;
    pia->qy = qy /= (double)numverts ;

    /* take cross product of vertex to find handedness */
    pia->flip_edge = flip_edge =
		(pgon[0][X] - pgon[1][X]) * (pgon[1][Y] - pgon[2][Y] ) >
		(pgon[0][Y] - pgon[1][Y]) * (pgon[1][X] - pgon[2][X] ) ;


    ax = pgon[0][X] - qx ;
    ay = pgon[0][Y] - qy ;
    len = sqrt( ax * ax + ay * ay ) ;
    if ( len == 0.0 ) {
	fprintf( stderr, "sorry, polygon for inclusion test is defective\n" ) ;
	exit(1) ;
    }
    pia->ax = ax /= len ;
    pia->ay = ay /= len ;

    /* loop through edges, and double last edge */
    for ( pc = p1 = 0, p2 = 1
	; pc <= numverts
	; pc++, p1 = p2, p2 = (++p2)%numverts, pis++ ) {

	/* wedge border */
	wx = pgon[p1][X] - qx ;
	wy = pgon[p1][Y] - qy ;
	len = sqrt( wx * wx + wy * wy ) ;
	wx /= len ;
	wy /= len ;

	/* cosine of angle from anchor border to wedge border */
	pis->dot = ax * wx + ay * wy ;
	/* sign from cross product */
	if ( ( ax * wy > ay * wx ) == flip_edge ) {
	    pis->dot = -2.0 - pis->dot ;
	}

	/* edge */
	pis->ex = pgon[p1][Y] - pgon[p2][Y] ;
	pis->ey = pgon[p2][X] - pgon[p1][X] ;
	pis->ec = pis->ex * pgon[p1][X] + pis->ey * pgon[p1][Y] ;

	/* check sense and reverse plane eqns if need be */
	if ( flip_edge ) {
	    pis->ex = -pis->ex ;
	    pis->ey = -pis->ey ;
	    pis->ec = -pis->ec ;
	}
    }
    /* set first angle a little > 1.0 and last < -3.0 just to be safe. */
    pia->pis[0].dot = -3.001 ;
    pia->pis[numverts].dot = 1.001 ;

    return( pia ) ;
}

/* Find wedge point is in by binary search, then test wedge */
int InclusionTest( pia, point )
pInclusionAnchor	pia ;
double	point[2] ;
{
register double tx, ty, len, dot ;
int	inside_flag, lo, hi, ind ;
pInclusionSet	pis ;

    tx = point[X] - pia->qx ;
    ty = point[Y] - pia->qy ;
    len = sqrt( tx * tx + ty * ty ) ;
    /* check if point is exactly at anchor point (which is inside polygon) */
    if ( len == 0.0 ) return( 1 ) ;
    tx /= len ;
    ty /= len ;

    /* get dot product for searching */
    dot = pia->ax * tx + pia->ay * ty ;
    if ( ( pia->ax * ty > pia->ay * tx ) == pia->flip_edge ) {
	dot = -2.0 - dot ;
    }

    /* binary search through angle list and find matching angle pair */
    lo = 0 ;
    hi = pia->hi_start ;
    while ( lo <= hi ) {
	ind = (lo+hi)/ 2 ;
	if ( dot < pia->pis[ind].dot ) {
	    hi = ind - 1 ;
	} else if ( dot > pia->pis[ind+1].dot ) {
	    lo = ind + 1 ;
	} else {
	    goto Foundit ;
	}
    }
    /* should never reach here, but just in case... */
    fprintf( stderr,
	    "Hmmm, something weird happened - bad dot product %lg\n", dot);

    Foundit:

    /* test if the point is outside the wedge's exterior edge */
    pis = &pia->pis[ind] ;
    inside_flag = ( pis->ex * point[X] + pis->ey * point[Y] <= pis->ec ) ;

    return( inside_flag ) ;
}

void InclusionCleanup( p_inc_anchor )
pInclusionAnchor p_inc_anchor ;
{
    free( p_inc_anchor->pis ) ;
    free( p_inc_anchor ) ;
}
#endif


/* ======= Crossings Multiply algorithm =================================== */

/*
 * This version is usually somewhat faster than the original published in
 * Graphics Gems IV; by turning the division for testing the X axis crossing
 * into a tricky multiplication test this part of the test became faster,
 * which had the additional effect of making the test for "both to left or
 * both to right" a bit slower for triangles than simply computing the
 * intersection each time.  The main increase is in triangle testing speed,
 * which was about 15% faster; all other polygon complexities were pretty much
 * the same as before.  On machines where division is very expensive (not the
 * case on the HP 9000 series on which I tested) this test should be much
 * faster overall than the old code.  Your mileage may (in fact, will) vary,
 * depending on the machine and the test data, but in general I believe this
 * code is both shorter and faster.  This test was inspired by unpublished
 * Graphics Gems submitted by Joseph Samosky and Mark Haigh-Hutchinson.
 * Related work by Samosky is in:
 *
 * Samosky, Joseph, "SectionView: A system for interactively specifying and
 * visualizing sections through three-dimensional medical image data",
 * M.S. Thesis, Department of Electrical Engineering and Computer Science,
 * Massachusetts Institute of Technology, 1993.
 *
 */

/* Shoot a test ray along +X axis.  The strategy is to compare vertex Y values
 * to the testing point's Y and quickly discard edges which are entirely to one
 * side of the test ray.  Note that CONVEX and WINDING code can be added as
 * for the CrossingsTest() code; it is left out here for clarity.
 *
 * Input 2D polygon _pgon_ with _numverts_ number of vertices and test point
 * _point_, returns 1 if inside, 0 if outside.
 */
int CrossingsMultiplyTest( pgon, numverts, point )
double	pgon[][2] ;
int	numverts ;
double	point[2] ;
{
register int	j, yflag0, yflag1, inside_flag ;
register double	ty, tx, *vtx0, *vtx1 ;

    tx = point[X] ;
    ty = point[Y] ;

    vtx0 = pgon[numverts-1] ;
    /* get test bit for above/below X axis */
    yflag0 = ( vtx0[Y] >= ty ) ;
    vtx1 = pgon[0] ;

    inside_flag = 0 ;
    for ( j = numverts+1 ; --j ; ) {

	yflag1 = ( vtx1[Y] >= ty ) ;
	/* Check if endpoints straddle (are on opposite sides) of X axis
	 * (i.e. the Y's differ); if so, +X ray could intersect this edge.
	 * The old test also checked whether the endpoints are both to the
	 * right or to the left of the test point.  However, given the faster
	 * intersection point computation used below, this test was found to
	 * be a break-even proposition for most polygons and a loser for
	 * triangles (where 50% or more of the edges which survive this test
	 * will cross quadrants and so have to have the X intersection computed
	 * anyway).  I credit Joseph Samosky with inspiring me to try dropping
	 * the "both left or both right" part of my code.
	 */
	if ( yflag0 != yflag1 ) {
	    /* Check intersection of pgon segment with +X ray.
	     * Note if >= point's X; if so, the ray hits it.
	     * The division operation is avoided for the ">=" test by checking
	     * the sign of the first vertex wrto the test point; idea inspired
	     * by Joseph Samosky's and Mark Haigh-Hutchinson's different
	     * polygon inclusion tests.
	     */
	    if ( ((vtx1[Y]-ty) * (vtx0[X]-vtx1[X]) >=
		    (vtx1[X]-tx) * (vtx0[Y]-vtx1[Y])) == yflag1 ) {
		inside_flag = !inside_flag ;
	    }
	}

	/* Move to the next pair of vertices, retaining info as possible. */
	yflag0 = yflag1 ;
	vtx0 = vtx1 ;
	vtx1 += 2 ;
    }

    return( inside_flag ) ;
}
