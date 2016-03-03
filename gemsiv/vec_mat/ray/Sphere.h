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
 double intersect(const vec3& ray_org, const vec3& ray_dir);
 vec3 normalAt(const vec3& p);

 // friends
 friend std::istream& operator >> (std::istream& s, Sphere& a);
};

#endif
