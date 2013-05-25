/* arcdivide.c - recursive circular arc subdivision (FP version) */

#define DMAX 0.5   /* max chordal deviation = 1/2 pixel */

#include <math.h>
#include "../ch7-7/GG4D/GGems.h"

/* Function prototype for externally defined functions */
void DrawLine(Point2 p0, Point2 p1);

void
DrawArc(Point2 p0, Point2 p1, double d)
{
    if (fabs(d) <= DMAX) DrawLine(p0, p1);
    else {
        Vector2 v;
        Point2  pm, pb;
        double  dSub;
        
        v.x = p1.x - p0.x;       /* vector from p0 to p1 */
        v.y = p1.y - p0.y;
        
        pm.x = p0.x + 0.5 * v.x; /* midpoint */
        pm.y = p0.y + 0.5 * v.y;
        
        dSub = d / 4;
        V2Scale(&v, dSub);       /* subdivided vector */
        
        pb.x = pm.x - v.y;       /* bisection point */
        pb.y = pm.y + v.x;
        
        DrawArc(p0, pb, dSub);   /* first half arc */
        DrawArc(pb, p1, dSub);   /* second half arc */
    }
}
