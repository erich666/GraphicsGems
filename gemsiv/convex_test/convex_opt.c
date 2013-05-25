/*
 * C code from the article
 * "Testing the Convexity of a Polygon"
 * by Peter Schorn and Frederick Fisher,
 *	(schorn@inf.ethz.ch, fred@kpc.com)
 * in "Graphics Gems IV", Academic Press, 1994
 */

/* Reasonably Optimized Routine to Classify a Polygon's Shape */

/*

.. code omitted which reads polygon, stores in an array, and calls
	classifyPolygon2()
*/

typedef enum { NotConvex, NotConvexDegenerate,
	       ConvexDegenerate, ConvexCCW, ConvexCW } PolygonClass;

typedef double	Number;		/* float or double */

#define ConvexCompare(delta)						\
    ( (delta[0] > 0) ? -1 :	/* x coord diff, second pt > first pt */\
      (delta[0] < 0) ?	1 :	/* x coord diff, second pt < first pt */\
      (delta[1] > 0) ? -1 :	/* x coord same, second pt > first pt */\
      (delta[1] < 0) ?	1 :	/* x coord same, second pt > first pt */\
      0 )			/* second pt equals first point */

#define ConvexGetPointDelta(delta, pprev, pcur )			\
    /* Given a previous point 'pprev', read a new point into 'pcur' */	\
    /* and return delta in 'delta'.				    */	\
    pcur = pVert[iread++];						\
    delta[0] = pcur[0] - pprev[0];					\
    delta[1] = pcur[1] - pprev[1];					\

#define ConvexCross(p, q) p[0] * q[1] - p[1] * q[0];

#define ConvexCheckTriple						\
    if ( (thisDir = ConvexCompare(dcur)) == -curDir ) {			\
	  ++dirChanges;							\
	  /* The following line will optimize for polygons that are  */ \
	  /* not convex because of classification condition 4,	     */ \
	  /* otherwise, this will only slow down the classification. */ \
	  /* if ( dirChanges > 2 ) return NotConvex;		     */ \
    }									\
    curDir = thisDir;							\
    cross = ConvexCross(dprev, dcur);					\
    if ( cross > 0 ) { if ( angleSign == -1 ) return NotConvex;		\
		       angleSign = 1;					\
		     }							\
    else if (cross < 0) { if (angleSign == 1) return NotConvex;		\
			    angleSign = -1;				\
			  }						\
    pSecond = pThird;		/* Remember ptr to current point. */	\
    dprev[0] = dcur[0];		/* Remember current delta.	  */	\
    dprev[1] = dcur[1];							\

classifyPolygon2( nvert, pVert )
int	nvert;
Number	pVert[][2];
/* Determine polygon type. return one of:
 *	NotConvex, NotConvexDegenerate,
 *	ConvexCCW, ConvexCW, ConvexDegenerate
 */
{
    int	     curDir, thisDir, dirChanges = 0,
	     angleSign = 0, iread ;
    Number   *pSecond, *pThird, *pSaveSecond, dprev[2], dcur[2], cross;

    /* if ( nvert <= 0 ) return error;	     if you care */

    /* Get different point, return if less than 3 diff points. */
    if ( nvert < 3 ) return ConvexDegenerate;
    iread = 1;
    while ( 1 ) {
	ConvexGetPointDelta( dprev, pVert[0], pSecond );
	if ( dprev[0] || dprev[1] ) break;
	/* Check if out of points. Check here to avoid slowing down cases
	 * without repeated points.
	 */
	if ( iread >= nvert ) return ConvexDegenerate;
    }

    pSaveSecond = pSecond;

    curDir = ConvexCompare(dprev);	/* Find initial direction */

    while ( iread < nvert ) {
	/* Get different point, break if no more points */
	ConvexGetPointDelta(dcur, pSecond, pThird );
	if ( dcur[0] == 0.0  &&	 dcur[1] == 0.0 ) continue;

	ConvexCheckTriple;		/* Check current three points */
    }

    /* Must check for direction changes from last vertex back to first */
    pThird = pVert[0];			/* Prepare for 'ConvexCheckTriple' */
    dcur[0] = pThird[0] - pSecond[0];
    dcur[1] = pThird[1] - pSecond[1];
    if ( ConvexCompare(dcur) ) {
	ConvexCheckTriple;
    }

    /* and check for direction changes back to second vertex */
    dcur[0] = pSaveSecond[0] - pSecond[0];
    dcur[1] = pSaveSecond[1] - pSecond[1];
    ConvexCheckTriple;			/* Don't care about 'pThird' now */

    /* Decide on polygon type given accumulated status */
    if ( dirChanges > 2 )
	return angleSign ? NotConvex : NotConvexDegenerate;

    if ( angleSign > 0 ) return ConvexCCW;
    if ( angleSign < 0 ) return ConvexCW;
    return ConvexDegenerate;
}
