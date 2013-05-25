
#include <math.h>


/****************************************************************************
These definitions of vectors and matrices are used throughout this code.
The constant M_PI_2 is equal to PI/2, or 1.57079632679489661923.
****************************************************************************/

typedef float	vec3[3];
typedef float	mat4[4][4];



/****************************************************************************
Transforms an object space point into world coordinates using the provided
cumulative transformation matrix.
****************************************************************************/

transform_point(world, local, ctm)
vec3 world;			/* returned world space point */
vec3 local;			/* provided local space point */
mat4 ctm;			/* cumulative transformation matrix */
{
   world[0] = local[0] * ctm[0][0] + local[1] * ctm[1][0] +
	      local[2] * ctm[2][0] + ctm[3][0];
   world[1] = local[0] * ctm[0][1] + local[1] * ctm[1][1] +
	      local[2] * ctm[2][1] + ctm[3][1];
   world[2] = local[0] * ctm[0][2] + local[1] * ctm[1][2] +
	      local[2] * ctm[2][2] + ctm[3][2];
}



/****************************************************************************
The provided point is compared against the current minimum and maximum
values in X, Y, and Z.  If a new maximum or minimum is found, the
min and max variables are updated.
****************************************************************************/

update_min_and_max(min, max, point)
vec3 min, max;			/* provided extent that is to be modified */
vec3 point;			/* world space point to be tested */
{
   int	i;

   for (i=0; i<3; i++) {
      if (point[0] < min[0]) min[0] = point[0];
      if (point[0] > max[0]) max[0] = point[0];
      if (point[1] < min[1]) min[1] = point[1];
      if (point[1] > max[1]) max[1] = point[1];
      if (point[2] < min[2]) min[2] = point[2];
      if (point[2] > max[2]) max[2] = point[2];
   }
}



/****************************************************************************
The bounding volume of a cube is found.  Each of the cube's eight vertices
is transformed into world space, and then compared to the current minimum
and maximum values, which are updated if a new extremum is found.  The
canonical cube is centered about the origin, with faces at X=-1, X=1, Y=-1,
Y=1, Z=-1, and Z=1.
****************************************************************************/

cube_volume(min, max, ctm)
vec3 min, max;			/* returned minimum and maximum of extent */
mat4 ctm;			/* cumulative transformation */
{
   int         i;
   vec3        point;
   static vec3 corners[8] = {
      -1, -1, -1,	1, -1, -1,	-1,  1, -1,	1,  1, -1,
      -1, -1,  1,	1, -1,  1,	-1,  1,  1,	1,  1,  1
   };

   min[0] = min[1] = min[2] = MAXFLOAT;
   max[0] = max[1] = max[2] = -MAXFLOAT;

   for (i=0; i<8; i++) {
      transform_point(point, corners[i], ctm);
      update_min_and_max(min, max, point);
   }
}



/****************************************************************************
The bounding volume of a collection of polygons is found.  The verts variable
is a list of the vertices of the polygon collection, and the vertices need
not be unique.  The num variable specifies how many vertices are in the list.
Each vertex is transformed into world space, and then compared to the current
minimum and maximum values, which are updated if a new extremum is found.
****************************************************************************/

polygons_volume(min, max, ctm, verts, num)
vec3 min, max;			/* returned minimum and maximum of extent */
mat4 ctm;			/* cumulative transformation */
vec3 verts[];			/* list of vertices from polygons */
int  num;			/* number of vertices in list */
{
   int  i;
   vec3 point;

   min[0] = min[1] = min[2] = MAXFLOAT;
   max[0] = max[1] = max[2] = -MAXFLOAT;

   for (i=0; i<num; i++) {
      transform_point(point, verts[i], ctm);
      update_min_and_max(min, max, point);
   }
}



/****************************************************************************
Performs a safe arctangent calculation for the two provided numbers.  If the
denominator is 0.o, atan2 is not called (it results in a floating exception
on some machines).  Instead, + or - PI/2 is returned.
****************************************************************************/

float
arctangent(a, b)
float a, b;			/* operands for tan'(a/b) */
{

   if (b != 0.0)
      return atan2(a, b);
   else if (a > 0.0)
      return M_PI_2;
   else
      return -M_PI_2;
}



/****************************************************************************
The bounding volume of a cylinder is found.  The algorithm operates in
three passes, one for each dimension.  In each pass the extrema of the
bottom circle of the cylinder are found as values of the parameter t.
These values are then used to calculate the position of the two points
in the current dimension.  Finally, these points from the bottom circle
and corresponding points from the top circle are considered while computing
the minimum and maximum values of the current dimension.  The canonical
cylinder has a radius of 1.0 from the Y axis, and ranges from Z=-1 to Z=1.
****************************************************************************/

cylinder_volume(min, max, ctm)
vec3 min, max;			/* returned minimum and maximum of extent */
mat4 ctm;			/* cumulative transformation matrix */
{
   int   i;
   float t1, t2, p1, p2, tmp;

   for (i=0; i<3; i++) {

      /* calculate first extremum.  second is +/- PI on other side of circle */
      t1 = arctangent(ctm[2][i], ctm[0][i]);
      if (t1 <= 0)
	 t2 = t1 + M_PI;
      else
	 t2 = t1 - M_PI;

      /* find and sort extrema locations in this dimension */
      p1 = ctm[0][i]*cos(t1) - ctm[1][i] + ctm[2][i]*sin(t1) + ctm[3][i];
      p2 = ctm[0][i]*cos(t2) - ctm[1][i] + ctm[2][i]*sin(t2) + ctm[3][i];
      if (p1 > p2) {
	 tmp = p1;
	 p1 = p2;
	 p2 = tmp;
      }

      /* add the difference between bottom and top circles to an extremum */
      if (ctm[1][i] < 0) {
	 min[i] = p1 + 2 * ctm[1][i];
	 max[i] = p2;
      }
      else {
	 min[i] = p1;
	 max[i] = p2 + 2 * ctm[1][i];
      }
   }
}



/****************************************************************************
The bounding volume of a cone is found.  The algorithm checks the cone's
bottom in three passes, one for each dimension.  In each pass the extrema
of the circle are found as values of the parameter t.  These values are
then used to calculate the position of the two points in the current dimension.
Finally, these points are considered along with the transformed apex of the
cone to compute the minimum and maximum extents.  The canonical cone has
a base of radius 1.0 at Z=-1 and an apex at the point 0,1,0.
****************************************************************************/

cone_volume(min, max, ctm)
vec3 min, max;			/* returned minimum and maximum of extent */
mat4 ctm;			/* cumulative transformation matrix */
{
   int          i;
   float        t1, t2, tmp;
   vec3         point;
   static vec3  apex = { 0, 1, 0 };

   for (i=0; i<3; i++) {

      /* calculate first extremum.  second is +/- PI on other side of circle */
      t1 = arctangent(ctm[2][i], ctm[0][i]);
      if (t1 <= 0)
	 t2 = t1 + M_PI;
      else
	 t2 = t1 - M_PI;

      /* find and sort extrema locations in this dimension */
      min[i] = ctm[0][i]*cos(t1) - ctm[1][i] + ctm[2][i]*sin(t1) + ctm[3][i];
      max[i] = ctm[0][i]*cos(t2) - ctm[1][i] + ctm[2][i]*sin(t2) + ctm[3][i];
      if (min[i] > max[i]) {
	 tmp = max[i];
	 max[i] = min[i];
	 min[i] = tmp;
      }
   }

   /* transform and check apex of cone */
   transform_point(point, apex, ctm);
   update_min_and_max(min, max, point);
}



/****************************************************************************
The bounding volume of a conic is found.  The algorithm checks the conic's
bottom and top in three passes, one for each dimension.  In each pass the
extrema of the circles are found as values of the parameter t.  These values
are then used to calculate the position of the four points in the current
dimension.  The conic has a base of radius 1.0 at Z=-1 and a top in the Y=1
plane with a radius r around the Y axis.
****************************************************************************/

conic_volume(min, max, ctm, r)
vec3  min, max;			/* returned minimum and maximum of extent */
mat4  ctm;			/* cumulative transformation matrix */
float r;			/* radius of top of conic */
{
   int   i;
   float t1, t2, p1, p2;

   for (i=0; i<3; i++) {

      /* calculate first extremum.  second is +/- PI on other side of circle */
      t1 = arctangent(ctm[2][i], ctm[0][i]);
      if (t1 <= 0)
	 t2 = t1 + M_PI;
      else
	 t2 = t1 - M_PI;

      /* find and sort bottom extrema locations in this dimension */
      p1 = ctm[0][i]*cos(t1) - ctm[1][i] + ctm[2][i]*sin(t1) + ctm[3][i];
      p2 = ctm[0][i]*cos(t2) - ctm[1][i] + ctm[2][i]*sin(t2) + ctm[3][i];
      if (p1 < p2) {
	 min[i] = p1;
	 max[i] = p2;
      }
      else {
	 min[i] = p2;
	 max[i] = p1;
      }

      /* find, sort and compare top extrema locations in this dimension */
      p1 = r*ctm[0][i]*cos(t1) + ctm[1][i] + r*ctm[2][i]*sin(t1) + ctm[3][i];
      p2 = r*ctm[0][i]*cos(t2) + ctm[1][i] + r*ctm[2][i]*sin(t2) + ctm[3][i];
      if (p1 < p2) {
	 if (p1 < min[i])
	    min[i] = p1;
	 if (p2 > max[i])
	    max[i] = p2;
      }
      else {
	 if (p2 < min[i])
	    min[i] = p2;
	 if (p1 > max[i])
	    max[i] = p1;
      }
   }
}



/****************************************************************************
The bounding volume of a sphere is found.  The algorithm performs three
passes, one for each dimension.  In each pass the extrema of the surface
are found as values of the parameters u and v.  These values are then used
to calculate the position of the two points in the current dimension.
Finally, these points are sorted to find the minimum and maximum extents.
The canonical sphere has a radius of 1.0 and is centered at the origin.
****************************************************************************/

sphere_volume(min, max, ctm)
vec3 min, max;			/* returned minimum and maximum of extent */
mat4 ctm;			/* cumulative transformation matrix */
{
   int   i;
   float u1, u2, v1, v2, tmp, denominator;

   for (i=0; i<3; i++) {

      /* calculate first extremum. */
      u1 = arctangent(ctm[2][i], ctm[0][i]);
      denominator = ctm[0][i]*cos(u1) + ctm[2][i]*sin(u1);
      v1 = arctangent(ctm[1][i], denominator);

      /* second extremum is +/- PI from u1, negative of v1 */
      if (u1 <= 0)
	 u2 = u1 + M_PI;
      else
	 u2 = u1 - M_PI;
      v2 = -v1;

      /* find and sort extrema locations in this dimension */
      min[i] =
	 ctm[0][i] * cos(u1) * cos(v1) +
	 ctm[1][i] * sin(v1) +
	 ctm[2][i] * sin(u1) * cos(v1) +
	 ctm[3][i];
      max[i] =
	 ctm[0][i] * cos(u2) * cos(v2) +
	 ctm[1][i] * sin(v2) +
	 ctm[2][i] * sin(u2) * cos(v2) +
	 ctm[3][i];
      if (min[i] > max[i]) {
	 tmp = max[i];
	 max[i] = min[i];
	 min[i] = tmp;
      }
   }
}



/****************************************************************************
The bounding volume of a torus is found.  The algorithm performs three
passes, one for each dimension.  In each pass the extrema of the surface
are found as values of the parameters u and v.  These values are then used
to calculate the position of the two points in the current dimension.
Finally, these points are sorted to find the minimum and maximum extents.
The torus is defined as the rotation of a circle that is perpendicular to
the XZ plane.  This circle has radius q, and its center lies in the XZ
plane and is rotated about the Y axis in a circle of radius r.  The value
of q must be less than that of r.
****************************************************************************/

torus_volume(min, max, ctm, r, q)
vec3  min, max;			/* returned minimum and maximum of extent */
mat4  ctm;			/* cumulative transformation matrix */
float r;			/* major radius of torus */
float q;			/* minor radius of torus */
{
   int   i;
   float u1, u2, v1, v2, tmp, denominator;

   for (i=0; i<3; i++) {

      /* calculate first extremum.  assure that -PI/2 <= v1 <= PI/2 */
      u1 = arctangent(ctm[2][i], ctm[0][i]);
      denominator = ctm[0][i]*cos(u1) + ctm[2][i]*sin(u1);
      v1 = arctangent(ctm[1][i], denominator);

      /* second extremum is +/- PI from u1, negative of v1 */
      if (u1 <= 0)
	 u2 = u1 + M_PI;
      else
	 u2 = u1 - M_PI;
      v2 = -v1;

      /* find and sort extrema locations in this dimension */
      min[i] =
	 ctm[0][i] * cos(u1) * (r + q * cos(v1)) +
	 ctm[1][i] * sin(v1) * q +
	 ctm[2][i] * sin(u1) * (r + q * cos(v1)) +
	 ctm[3][i];
      max[i] =
	 ctm[0][i] * cos(u2) * (r + q * cos(v2)) +
	 ctm[1][i] * sin(v2) * q +
	 ctm[2][i] * sin(u2) * (r + q * cos(v2)) +
	 ctm[3][i];
      if (min[i] > max[i]) {
	 tmp = max[i];
	 max[i] = min[i];
	 min[i] = tmp;
      }
   }
}
