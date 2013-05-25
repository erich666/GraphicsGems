/************************************************************************
*									*
* CLASS: Sphere								*
* AUTHOR: Jean-Francois DOUE						*
* LAST MODIFICATION: 12 Oct 1993					*
*									*
* This class implements a sphere.					*
*									*
************************************************************************/

#ifndef Sphere_h
#define Sphere_h 1
#include "Primitive.h"

class Sphere : public Primitive
{
protected:
 double radius;

public:

 // virtual methods
 double intersect(vec3& ray_org, vec3& ray_dir);
 vec3 normalAt(vec3& p);

 // friends
 friend istream& operator >> (istream& s, Sphere& a);
};

#endif