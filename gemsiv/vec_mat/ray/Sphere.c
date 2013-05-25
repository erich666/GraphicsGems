#include "Sphere.h"
#include "solver.h"

/************************************************************************
*									*
* This method computes the intersection between a ray and the sphere.	*
* It returns the distance between the origin of the ray and the closest *
* point of intersection (or 0.0 if no intersection occurs).		*
*									*
************************************************************************/

double Sphere::intersect(vec3& ray_org, vec3& ray_dir)
{
double	c[3],			    // coefficients of the quadric equation.
	s[2];			    // solutions of the quadric equation
vec3	d = ray_org - pos;	    // vector from the center of the sphere to
				    // the origin of the ray.

// compute the coefficients of the resolvent equation
c[2] = 1.0;
c[1] = 2.0 * ray_dir * d;
c[0] = d * d - radius * radius;

// return the closest intersection point
return closest_intersection(s, solveQuadric(c, s));
}

/************************************************************************
*									*
* This method computes the normal vector to the sphere at the point of	*
* intersection. (NB: normalization is "built-in" for the sphere).	*
*									*
************************************************************************/

vec3 Sphere::normalAt(vec3& p)
{ return (p - pos) / radius; }

/************************************************************************
*									*
* Input from stream.							*
*									*
************************************************************************/

istream& operator >> (istream& s, Sphere& a)
{
s >> *((Primitive*) &a);
s >> a.radius;
return s;
}
