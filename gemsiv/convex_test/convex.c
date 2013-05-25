/*
 * C code from the article
 * "Testing the Convexity of a Polygon"
 * by Peter Schorn and Frederick Fisher,
 *	(schorn@inf.ethz.ch, fred@kpc.com)
 * in "Graphics Gems IV", Academic Press, 1994
 */

/* Program to Classify a Polygon's Shape */

#include <stdio.h>

typedef enum { NotConvex, NotConvexDegenerate,
	       ConvexDegenerate, ConvexCCW, ConvexCW } PolygonClass;

typedef struct { double x, y; } Point2d;

int WhichSide(p, q, r)		/* Given a directed line pq, determine	*/
Point2d	      p, q, r;		/* whether qr turns CW or CCW.		*/
{
    double result;
    result = (p.x - q.x) * (q.y - r.y) - (p.y - q.y) * (q.x - r.x);
    if (result < 0) return -1;	/* q lies to the left  (qr turns CW).	*/
    if (result > 0) return  1;	/* q lies to the right (qr turns CCW).	*/
    return 0;			/* q lies on the line from p to r.	*/
}

int Compare(p, q)		/* Lexicographic comparison of p and q	*/
Point2d	    p, q;
{
    if (p.x < q.x) return -1;	/* p is less than q.			*/
    if (p.x > q.x) return  1;	/* p is greater than q.			*/
    if (p.y < q.y) return -1;	/* p is less than q.			*/
    if (p.y > q.y) return  1;	/* p is greater than q.			*/
    return 0;			/* p is equal to q.			*/
}

int GetPoint(f, p)		/* Read p's x- and y-coordinate from f	*/
FILE	*f;			/* and return true, iff successful.	*/
Point2d *p;
{
    return !feof(f) && (2 == fscanf(f, "%lf%lf", &(p->x), &(p->y)));
}

int GetDifferentPoint(f, previous, next)
FILE	*f;			/* Read next point into 'next' until it */
Point2d previous, *next;	/* is different from 'previous' and	*/
{				/* return true iff successful.		*/
    int eof;
    while((eof = GetPoint(f, next)) && (Compare(previous, *next) == 0));
    return eof;
}

/* CheckTriple tests three consecutive points for change of direction
 * and for orientation.
 */
#define CheckTriple							\
	if ( (thisDir = Compare(second, third)) == -curDir )		\
	    ++dirChanges;						\
	curDir = thisDir;						\
	if ( thisSign = WhichSide(first, second, third) ) {		\
	    if ( angleSign == -thisSign )				\
		return NotConvex;					\
	    angleSign = thisSign;					\
	}								\
	first = second; second = third;

/* Classify the polygon vertices on file 'f' according to: 'NotConvex'	*/
/* 'NotConvexDegenerate', 'ConvexDegenerate', 'ConvexCCW', 'ConvexCW'.	*/
PolygonClass ClassifyPolygon(f)
FILE			    *f;
{
    int		 curDir, thisDir, thisSign, angleSign = 0, dirChanges = 0;
    Point2d	 first, second, third, saveFirst, saveSecond;

    if ( !GetPoint(f, &first) || !GetDifferentPoint(f, first, &second) )
	return ConvexDegenerate;
    saveFirst = first;	saveSecond = second;
    curDir = Compare(first, second);
    while( GetDifferentPoint(f, second, &third) ) {
	CheckTriple;
    }
    /* Must check that end of list continues back to start properly */
    if ( Compare(second, saveFirst) ) {
	third = saveFirst; CheckTriple;
    }
    third = saveSecond;	 CheckTriple;

    if ( dirChanges > 2 ) return angleSign ? NotConvex : NotConvexDegenerate;
    if ( angleSign  > 0 ) return ConvexCCW;
    if ( angleSign  < 0 ) return ConvexCW;
    return ConvexDegenerate;
}

int main()
{
    switch ( ClassifyPolygon(stdin) ) {
	case NotConvex:		  fprintf( stderr,"Not Convex\n");
				  exit(-1); break;
	case NotConvexDegenerate: fprintf( stderr,"Not Convex Degenerate\n");
				  exit(-1); break;
	case ConvexDegenerate:	  fprintf( stderr,"Convex Degenerate\n");
				  exit( 0); break;
	case ConvexCCW:		  fprintf( stderr,"Convex Counter-Clockwise\n");
				  exit( 0); break;
	case ConvexCW:		  fprintf( stderr,"Convex Clockwise\n");
				  exit( 0); break;
    }
}
