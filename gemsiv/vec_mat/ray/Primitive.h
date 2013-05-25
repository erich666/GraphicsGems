/************************************************************************
*									*
* CLASS: Primitive							*
* AUTHOR: Jean-Francois DOUE						*
* LAST MODIFICATION: 12 Oct 1993					*
*									*
* This class is an abstract class which defines the basic methods	*
* to which every ray-traceable object (primitive) will have to respond. *
* It also stores the color and the phong coefficients of the primitive	*
*									*
************************************************************************/

#ifndef Primitive_h
#define Primitive_h 1
#include "Object3D.h"

class Primitive: public Object3D
{
protected:
 vec3	col;	    // color of the primitive
 vec4	ph;	    // phong coefficients of the object

public:

 vec3 color() { return col; };
 double phong(int i) { return ph[i]; };
 double closest_intersection(double *x, int x_num);

 // virtual methods
 virtual ~Primitive();
 virtual double intersect(vec3& ray_org, vec3& ray_dir) = 0;
 virtual vec3 normalAt(vec3& p) = 0;

 // friends
 friend istream& operator >> (istream& s, Primitive& a);
};

#endif