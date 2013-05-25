/* 
Generating Random Points in Triangles
by Greg Turk
from "Graphics Gems", Academic Press, 1990
*/

#include <math.h>
#include "GraphicsGems.h"

/*****************************************************************
Compute relative areas of sub-triangles that form a convex polygon.
There are vcount-2 sub-triangles, each defined by the first point
in the polygon and two other adjacent points.

This procedure should be called once before using 
square_to_polygon().

Entry:
  vertices - list of the vertices of a convex polygon
  vcount   - number of vertices of polygon
Exit:
  areas - relative areas of sub-triangles of polygon
*******************************************************************/

triangle_areas (vertices, vcount, areas)
  	Point3 vertices[];
	int vcount;
  	float areas[];
{
  	int i;
  	float area_sum = 0;
  	Vector3 v1,v2,v3;

  /* compute relative areas of the sub-triangles of polygon */

  	for (i = 0; i < vcount - 2; i++) {
     	V3Sub(&vertices[i+1], &vertices[0], &v1);
     	V3Sub(&vertices[i+2], &vertices[0], &v2);
     	V3Cross(&v1, &v2, &v3);
     	areas[i] = V3Length(&v3);
     	area_sum += areas[i];
  }

  /* normalize areas so that the sum of all sub-triangles is one */

  	for (i = 0; i < vcount - 2; i++)
    	areas[i] /= area_sum;
}


/******************************************************************
Map a point from the square [0,1] x [0,1] into a convex polygon.
Uniform random points in the square will generate uniform random 
points in the polygon.

The procedure triangle_areas() must be called once to compute 
'areas', and then this procedure can be called repeatedly.

Entry:
  vertices - list of the vertices of a convex polygon
  vcount   - number of vertices of polygon
  areas    - relative areas of sub-triangles of polygon
  s,t      - position in the square [0,1] x [0,1]
Exit:
  p - position in polygon
*********************************************************************/

square_to_polygon (vertices, vcount, areas, s, t, p)
 	Point3 vertices[];
  	int vcount;
  	float areas[];
  	float s,t;
  	Point3 *p;
{ 
	int i;
	float area_sum = 0;
  	float a,b,c;

  /* use 's' to pick one sub-triangle, weighted by relative */
  /* area of triangles */

  	for (i = 0; i < vcount - 2; i++) {
    	area_sum += areas[i];
    	if (area_sum >= s)
      		break;
  }

  /* map 's' into the interval [0,1] */

  s = (s - area_sum + areas[i]) / areas[i];

  /* map (s,t) to a point in that sub-triangle */

  t = sqrt(t);

  a = 1 - t;
  b = (1 - s) * t;
  c = s * t;

  p->x = a * vertices[0].x + b * vertices[i+1].x + c * vertices[i+2].x;
  p->y = a * vertices[0].y + b * vertices[i+1].y + c * vertices[i+2].y;
  p->z = a * vertices[0].z + b * vertices[i+1].z + c * vertices[i+2].z;
}
