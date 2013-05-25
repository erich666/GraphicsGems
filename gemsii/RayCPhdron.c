/* Ray-Convex Polyhedron Intersection Test by Eric Haines, erich@eye.com
 *
 * This test checks the ray against each face of a polyhedron, checking whether
 * the set of intersection points found for each ray-plane intersection
 * overlaps the previous intersection results.  If there is no overlap (i.e.
 * no line segment along the ray that is inside the polyhedron), then the
 * ray misses and returns 0; else 1 is returned if the ray is entering the
 * polyhedron, -1 if the ray originates inside the polyhedron.  If there is
 * an intersection, the distance and the normal of the face hit is returned.
 */

#include <math.h>
#include "GraphicsGems.h"

#ifndef	HUGE_VAL
#define	HUGE_VAL	1.7976931348623157e+308
#endif

typedef struct Point4Struct {	/* 4d point */
	double x, y, z, w;
	} Point4;

/* fast macro version of V3Dot, usable with Point4 */
#define DOT3( a, b )	( (a)->x*(b)->x + (a)->y*(b)->y + (a)->z*(b)->z )

/* return codes */
#define	MISSED		 0
#define	FRONTFACE	 1
#define	BACKFACE	-1

int	RayCvxPolyhedronInt( org, dir, tmax, phdrn, ph_num, tresult, norm )
Point3	*org, *dir ;	/* origin and direction of ray */
double	tmax ;		/* maximum useful distance along ray */
Point4	*phdrn ;	/* list of planes in convex polyhedron */
int	ph_num ;	/* number of planes in convex polyhedron */
double	*tresult ;	/* returned: distance of intersection along ray */
Point3	*norm ;		/* returned: normal of face hit */
{
Point4	*pln ;			/* plane equation */
double	tnear, tfar, t, vn, vd ;
int	fnorm_num, bnorm_num ;	/* front/back face # hit */

    tnear = -HUGE_VAL ;
    tfar = tmax ;

    /* Test each plane in polyhedron */
    for ( pln = &phdrn[ph_num-1] ; ph_num-- ; pln-- ) {
	/* Compute intersection point T and sidedness */
	vd = DOT3( dir, pln ) ;
	vn = DOT3( org, pln ) + pln->w ;
	if ( vd == 0.0 ) {
	    /* ray is parallel to plane - check if ray origin is inside plane's
	       half-space */
	    if ( vn > 0.0 )
		/* ray origin is outside half-space */
		return ( MISSED ) ;
	} else {
	    /* ray not parallel - get distance to plane */
	    t = -vn / vd ;
	    if ( vd < 0.0 ) {
		/* front face - T is a near point */
		if ( t > tfar ) return ( MISSED ) ;
		if ( t > tnear ) {
		    /* hit near face, update normal */
		    fnorm_num = ph_num ;
		    tnear = t ;
		}
	    } else {
		/* back face - T is a far point */
		if ( t < tnear ) return ( MISSED ) ;
		if ( t < tfar ) {
		    /* hit far face, update normal */
		    bnorm_num = ph_num ;
		    tfar = t ;
		}
	    }
	}
    }

    /* survived all tests */
    /* Note: if ray originates on polyhedron, may want to change 0.0 to some
     * epsilon to avoid intersecting the originating face.
     */
    if ( tnear >= 0.0 ) {
	/* outside, hitting front face */
	*norm = *(Point3 *)&phdrn[fnorm_num] ;
	*tresult = tnear ;
	return ( FRONTFACE ) ;
    } else {
	if ( tfar < tmax ) {
	    /* inside, hitting back face */
	    *norm = *(Point3 *)&phdrn[bnorm_num] ;
	    *tresult = tfar ;
	    return ( BACKFACE ) ;
	} else {
	    /* inside, but back face beyond tmax */
	    return ( MISSED ) ;
	}
    }
}









