#include "Camera.h"

/************************************************************************
*									*
* These methods set the field of view of the camera. They transform	*
* the field of view between a user representation (2 angles specified	*
* in degrees) an internal representation (the size of the film square). *
*									*
************************************************************************/

void Camera::setFieldOfView(vec2& v)
{ film_size = (v * (M_PI / 360.0)).apply(&tan); }

vec2 Camera::fieldOfView()
{ return (360.0 * M_1_PI) * (vec2(film_size).apply(&atan)); }

/************************************************************************
*									*
* This method transforms a 2D point located on the film plane into a	*
* normalized ray. The method only returns the direction of the ray.	*
* The origin can be found by asking the camera its position.		*
*									*
************************************************************************/

vec3 Camera::pointToRay(vec2&  p)
{ return orient * vec3(prod(film_size, p), -1.0).normalize(); }

/************************************************************************
*									*
* Input from stream.							*
*									*
************************************************************************/

istream& operator >> (istream& s, Camera& a)
{
vec2	fov;

s >> *((Object3D*) &a);
s >> fov;
a.setFieldOfView(fov);
return s;
}
