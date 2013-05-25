#include <stddef.h> // for size_t
// #include <sys/stdtypes.h> // for size_t, on some systems
#include "Bezier.h"
#include <math.h>
/* The value of 1.0 / (1L<<23) is float machine epsilon. */
#ifdef FLOAT_ACCURACY
#define INV_EPS (1L<<23)
#else
/* The value of 1.0 / (1L<<14) is enough for most applications */
#define INV_EPS (1L<<14)
#endif
#define log2(x) (log(x)/log(2.))

extern "C" void qsort( char *base, int nel, size_t width, int (*compar)(void *, void *));

int compare_doubles( void *a, void *b )
    {
    register double *A = (double *)a, *B = (double *)b;
    return( *A > *B )?1:(*A < *B ? -1 : 0 );
    }

void Sort( double *array, int length )
    {
    qsort( (char *)array, length, sizeof( double ), compare_doubles );
    }
    
/*
 * Split the curve at the midpoint, returning an array with the two parts
 * Temporary storage is minimized by using part of the storage for the result
 * to hold an intermediate value until it is no longer needed.
 */
#define left r[0]
#define right r[1]
Bezier *Bezier::Split()
    {
    Bezier *r = new Bezier[2];
    (left.p0 = p0)->refcount++;
    (right.p3 = p3)->refcount++;
    left.p1 = new point(  p0, p1, 1);
    right.p2 = new point( p2, p3, 1);
    right.p1 = new point( p1, p2, 1); // temporary holding spot
    left.p2 = new point ( left.p1, right.p1, 1);
    *right.p1 = mid( right.p1, right.p2 ); // Real value this time
    left.p3 = right.p0 = new point( left.p2, right.p1, 2 );
    return r;
    }
#undef left
#undef right

    
/*
* Test the bounding boxes of two Bezier curves for interference.
* Several observations:
*	First, it is cheaper to compute the bounding box of the second curve
*	and test its bounding box for interference than to use a more direct
*	approach of comparing all control points of the second curve with 
*	the various edges of the bounding box of the first curve to test
* 	for interference.
*	Second, after a few subdivisions it is highly probable that two corners
*	of the bounding box of a given Bezier curve are the first and last 
*	control point.  Once this happens once, it happens for all subsequent
*	subcurves.  It might be worth putting in a test and then short-circuit
*	code for further subdivision levels.
*	Third, in the final comparison (the interference test) the comparisons
*	should both permit equality.  We want to find intersections even if they
*	occur at the ends of segments.
*	Finally, there are tighter bounding boxes that can be derived. It isn't
*	clear whether the higher probability of rejection (and hence fewer
*	subdivisions and tests) is worth the extra work.
*/
int IntersectBB( Bezier a, Bezier b )
    {
    // Compute bounding box for a
    double minax, maxax, minay, maxay;
    if( a.p0->x > a.p3->x )	 // These are the most likely to be extremal
	minax = a.p3->x, maxax = a.p0->x;
    else
	minax = a.p0->x, maxax = a.p3->x;
    if( a.p2->x < minax )
	minax = a.p2->x;
    else if( a.p2->x > maxax )
	maxax = a.p2->x;
    if( a.p1->x < minax )
	minax = a.p1->x;
    else if( a.p1->x > maxax )
	maxax = a.p1->x;
    if( a.p0->y > a.p3->y ) 		
	minay = a.p3->y, maxay = a.p0->y;
    else
	minay = a.p0->y, maxay = a.p3->y;
    if( a.p2->y < minay )
	minay = a.p2->y;
    else if( a.p2->y > maxay )
	maxay = a.p2->y;
    if( a.p1->y < minay )
	minay = a.p1->y;
    else if( a.p1->y > maxay )
	maxay = a.p1->y;
    // Compute bounding box for b
    double minbx, maxbx, minby, maxby;
    if( b.p0->x > b.p3->x ) 		
	minbx = b.p3->x, maxbx = b.p0->x;
    else
	minbx = b.p0->x, maxbx = b.p3->x;
    if( b.p2->x < minbx )
	minbx = b.p2->x;
    else if( b.p2->x > maxbx )
	maxbx = b.p2->x;
    if( b.p1->x < minbx )
	minbx = b.p1->x;
    else if( b.p1->x > maxbx )
	maxbx = b.p1->x;
    if( b.p0->y > b.p3->y ) 		
	minby = b.p3->y, maxby = b.p0->y;
    else
	minby = b.p0->y, maxby = b.p3->y;
    if( b.p2->y < minby )
	minby = b.p2->y;
    else if( b.p2->y > maxby )
	maxby = b.p2->y;
    if( b.p1->y < minby )
	minby = b.p1->y;
    else if( b.p1->y > maxby )
	maxby = b.p1->y;
    // Test bounding box of b against bounding box of a
    if( ( minax > maxbx ) || ( minay > maxby )  // Not >= : need boundary case
	|| ( minbx > maxax ) || ( minby > maxay ) )
	return 0; // they don't intersect
    else
	return 1; // they intersect
    }
	
/* 
* Recursively intersect two curves keeping track of their real parameters 
* and depths of intersection.
* The results are returned in a 2-D array of doubles indicating the parameters
* for which intersections are found.  The parameters are in the order the
* intersections were found, which is probably not in sorted order.
* When an intersection is found, the parameter value for each of the two 
* is stored in the index elements array, and the index is incremented.
* 
* If either of the curves has subdivisions left before it is straight
*	(depth > 0)
* that curve (possibly both) is (are) subdivided at its (their) midpoint(s).
* the depth(s) is (are) decremented, and the parameter value(s) corresponding
* to the midpoints(s) is (are) computed.
* Then each of the subcurves of one curve is intersected with each of the 
* subcurves of the other curve, first by testing the bounding boxes for
* interference.  If there is any bounding box interference, the corresponding
* subcurves are recursively intersected.
* 
* If neither curve has subdivisions left, the line segments from the first
* to last control point of each segment are intersected.  (Actually the 
* only the parameter value corresponding to the intersection point is found).
*
* The apriori flatness test is probably more efficient than testing at each
* level of recursion, although a test after three or four levels would
* probably be worthwhile, since many curves become flat faster than their 
* asymptotic rate for the first few levels of recursion.
*
* The bounding box test fails much more frequently than it succeeds, providing
* substantial pruning of the search space.
*
* Each (sub)curve is subdivided only once, hence it is not possible that for 
* one final line intersection test the subdivision was at one level, while
* for another final line intersection test the subdivision (of the same curve)
* was at another.  Since the line segments share endpoints, the intersection
* is robust: a near-tangential intersection will yield zero or two
* intersections.
*/
void RecursivelyIntersect( Bezier a, double t0, double t1, int deptha,
			   Bezier b, double u0, double u1, int depthb,
			   double **parameters, int &index )
    {
    if( deptha > 0 )
	{
	Bezier *A = a.Split();
	double tmid = (t0+t1)*0.5;
	deptha--;
	if( depthb > 0 )
	    {
	    Bezier *B = b.Split();
	    double umid = (u0+u1)*0.5;
	    depthb--;
	    if( IntersectBB( A[0], B[0] ) )
		RecursivelyIntersect( A[0], t0, tmid, deptha,
				      B[0], u0, umid, depthb,
				      parameters, index );
	    if( IntersectBB( A[1], B[0] ) )
		RecursivelyIntersect( A[1], tmid, t1, deptha,
				      B[0], u0, umid, depthb,
				      parameters, index );
	    if( IntersectBB( A[0], B[1] ) )
		RecursivelyIntersect( A[0], t0, tmid, deptha,
				      B[1], umid, u1, depthb,
				      parameters, index );
	    if( IntersectBB( A[1], B[1] ) )
		RecursivelyIntersect( A[1], tmid, t1, deptha,
				      B[1], umid, u1, depthb,
				      parameters, index );
	    }
	else
	    {
	    if( IntersectBB( A[0], b ) )
		RecursivelyIntersect( A[0], t0, tmid, deptha,
				      b, u0, u1, depthb,
				      parameters, index );
	    if( IntersectBB( A[1], b ) )
		RecursivelyIntersect( A[1], tmid, t1, deptha,
				      b, u0, u1, depthb,
				      parameters, index );
	    }
	}
    else
	if( depthb > 0 )
	    {
	    Bezier *B = b.Split();
	    double umid = (u0 + u1)*0.5;
	    depthb--;
	    if( IntersectBB( a, B[0] ) )
		RecursivelyIntersect( a, t0, t1, deptha,
				      B[0], u0, umid, depthb,
				      parameters, index );
	    if( IntersectBB( a, B[1] ) )
		RecursivelyIntersect( a, t0, t1, deptha,
				      B[1], umid, u1, depthb,
				      parameters, index );
	    }
	else // Both segments are fully subdivided; now do line segments
	    {
	    double xlk = a.p3->x - a.p0->x;
	    double ylk = a.p3->y - a.p0->y;
	    double xnm = b.p3->x - b.p0->x;
	    double ynm = b.p3->y - b.p0->y;
	    double xmk = b.p0->x - a.p0->x;
	    double ymk = b.p0->y - a.p0->y;
	    double det = xnm * ylk - ynm * xlk;
	    if( 1.0 + det == 1.0 )
		return;
	    else
		{
		double detinv = 1.0 / det;
		double s = ( xnm * ymk - ynm *xmk ) * detinv;
		double t = ( xlk * ymk - ylk * xmk ) * detinv;
		if( ( s < 0.0 ) || ( s > 1.0 ) || ( t < 0.0 ) || ( t > 1.0 ) )
		    return;
		parameters[0][index] = t0 + s * ( t1 - t0 );
		parameters[1][index] = u0 + t * ( u1 - u0 );
		index++;
		}
	    }
    }

inline double log4( double x ) { return 0.5 * log2( x ); }
    
/*
 * Wang's theorem is used to estimate the level of subdivision required,
 * but only if the bounding boxes interfere at the top level.
 * Assuming there is a possible intersection, RecursivelyIntersect is
 * used to find all the parameters corresponding to intersection points.
 * these are then sorted and returned in an array.
 */

double ** FindIntersections( Bezier a, Bezier b )
    {
    double **parameters = new double *[2];
    parameters[0] = new double[9];
    parameters[1] = new double[9];
    int index = 0;
    if( IntersectBB( a, b ) )
	{
	vector la1 = vabs( ( *(a.p2) - *(a.p1) ) - ( *(a.p1) - *(a.p0) ) );
	vector la2 = vabs( ( *(a.p3) - *(a.p2) ) - ( *(a.p2) - *(a.p1) ) );
	vector la;
	if( la1.x > la2.x ) la.x = la1.x; else la.x = la2.x;
	if( la1.y > la2.y ) la.y = la1.y; else la.y = la2.y;
	vector lb1 = vabs( ( *(b.p2) - *(b.p1) ) - ( *(b.p1) - *(b.p0) ) );
	vector lb2 = vabs( ( *(b.p3) - *(b.p2) ) - ( *(b.p2) - *(b.p1) ) );
	vector lb;
	if( lb1.x > lb2.x ) lb.x = lb1.x; else lb.x = lb2.x;
	if( lb1.y > lb2.y ) lb.y = lb1.y; else lb.y = lb2.y;
	double l0;
	if( la.x > la.y ) 
	    l0 = la.x;
	else 
	    l0 = la.y;
	int ra;
	if( l0 * 0.75 * M_SQRT2 + 1.0 == 1.0 ) 
	    ra = 0;
	else
	    ra = (int)ceil( log4( M_SQRT2 * 6.0 / 8.0 * INV_EPS * l0 ) );
	if( lb.x > lb.y ) 
	    l0 = lb.x;
	else 
	    l0 = lb.y;
	int rb;
	if( l0 * 0.75 * M_SQRT2 + 1.0 == 1.0 ) 
	    rb = 0;
	else
	    rb = (int)ceil(log4( M_SQRT2 * 6.0 / 8.0 * INV_EPS * l0 ) );
	RecursivelyIntersect( a, 0., 1., ra, b, 0., 1., rb, parameters, index );
	}
    if( index < 9 )
	parameters[0][index] = parameters[1][index] = -1.0;
    Sort( parameters[0], index );
    Sort( parameters[1], index );
    return parameters;
    }

void
Bezier::ParameterSplitLeft( double t, Bezier &left )
    {
    left.p0 = p0;
    left.p1 = new point( *p0 + t * ( *p1 - *p0 ) );
    left.p2 = new point( *p1 + t * ( *p2 - *p1 ) ); // temporary holding spot
    p2->refcount--;
    p2 = new point( *p2 + t * ( *p3 - *p2 ) );
    p1->refcount--;
    p1 = new point( *(left.p2) + t * ( *p2 - *(left.p2) ) );
    *(left.p2) = ( *(left.p1) + t * ( *(left.p2) - *(left.p1) ) );
    (left.p3 = p0 = new point(*(left.p2) + t * (*(p1)-*(left.p2))))->refcount++;
    left.p0->refcount++; left.p1->refcount++; 
    left.p2->refcount++; left.p3->refcount++;
    }
    
/*
 * Intersect two curves, returning an array of two arrays of curves.
 * The first array of curves corresponds to `this' curve, the second 
 * corresponds to curve B, passed in.
 * The intersection parameter values are computed by FindIntersections,
 * and they come back in the range 0..1, using the original parameterization.
 * Once one segment has been removed, ie the curve is split at splitT, the
 * parameterization of the second half is from 0..1, so the parameter for
 * the next split point, if any, must be adjusted.
 * If we split at t[i], the split point at t[i+1] is 
 * ( t[i+1] - t[i] ) / ( t - t[i] ) of the way to the end from the new
 * start point.
 */

Bezier **Bezier::Intersect( Bezier B )
    {
    // The return from FindIntersections will decrement all refcounts.
    // (a c++-ism)
    B.p0->refcount++; B.p1->refcount++; B.p2->refcount++; B.p3->refcount++; 
    Bezier **rvalue = new Bezier *[2];
    rvalue[0] = new Bezier[10];
    rvalue[1] = new Bezier[10];
    double **t = FindIntersections( *this, B );
    int index = 0;
    if( t[0][0] > -0.5 )
	{
	ParameterSplitLeft( t[0][0], rvalue[0][0] );
	B.ParameterSplitLeft( t[1][0], rvalue[1][0] );
	index++;
	while( t[0][index] > -0.5 && index < 9 )
	    {
	    double splitT = (t[0][index] - t[0][index-1])/(1.0 - t[0][index-1]);
	    ParameterSplitLeft( splitT, rvalue[0][index] );
	    splitT = (t[1][index] - t[1][index-1])/(1.0 - t[1][index-1]);
	    B.ParameterSplitLeft( splitT, rvalue[1][index] );
	    index++;
	    }
	}
    rvalue[0][index] = *this;
    rvalue[1][index] = B;
    return rvalue;
    }
