/*
 * C code from the article
 * "An Incremental Angle Point in Polygon Test"
 * by Kevin Weiler, kjw@autodesk.com
 * in "Graphics Gems IV", Academic Press, 1994
 */

#include "polygon.h"

	/* quadrant id's, incremental angles, accumulated angle values */
typedef short quadrant_type;

	/* result value from point in polygon test */
typedef enum pt_poly_relation {INSIDE, OUTSIDE} pt_poly_relation;

	/* determine the quadrant of a polygon point
	   relative to the test point */
#define quadrant(vertex, x, y) \
  ( (vertex->x > x) ? ((vertex->y > y) ? 0 : 3) : ( (vertex->y > y) ? 1 : 2) )

	/* determine x intercept of a polygon edge
	   with a horizontal line at the y value of the test point */
#define x_intercept(pt1, pt2,  yy) \
  (pt2->x - ( (pt2->y - yy) * ((pt1->x - pt2->x) / (pt1->y - pt2->y)) ) )

	/* adjust delta */
#define adjust_delta(delta, vertex, next_vertex, xx, yy)		\
  switch (delta) {							\
	    /* make quadrant deltas wrap around */			\
    case  3:	delta = -1; break;					\
    case -3:	delta =	 1; break;					\
	    /* check if went around point cw or ccw */			\
    case  2: case -2: if (x_intercept(vertex, next_vertex, yy) > xx)	\
		    delta =  - (delta);					\
		break;							\
    }

	/* determine if a test point is inside of or outside of a polygon */
	/* polygon is "poly", test point is at "x","y" */
pt_poly_relation
point_in_poly(polygon_ptr poly, double x, double y)
{
  vtx_ptr  vertex, first_vertex, next_vertex;
  quadrant_type quad, next_quad, delta, angle;

	    /* initialize */
  vertex = NULL;    /* because polygon_get_vertex is a macro */
  vertex = first_vertex = polygon_get_vertex(poly,vertex);
  quad = quadrant(vertex, x, y);
  angle = 0;
	    /* loop on all vertices of polygon */
  do {
    next_vertex = polygon_get_vertex(poly,vertex);
		/* calculate quadrant and delta from last quadrant */
    next_quad = quadrant(next_vertex, x, y);
    delta = next_quad - quad;
    adjust_delta(delta,vertex,next_vertex,x,y);
		/* add delta to total angle sum */
    angle = angle + delta;
		/* increment for next step */
    quad = next_quad;
    vertex = next_vertex;
    } while (vertex != first_vertex);

	    /* complete 360 degrees (angle of + 4 or -4 ) means inside */
  if ((angle == +4) || (angle == -4)) return INSIDE; else return OUTSIDE;

	    /* odd number of windings rule */
  /* if (angle & 4) return INSIDE; else return OUTSIDE; */
	    /* non-zero winding rule */
  /* if (angle != 0) return INSIDE; else return OUTSIDE; */
}
