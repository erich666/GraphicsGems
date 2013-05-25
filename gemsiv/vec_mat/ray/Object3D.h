/************************************************************************
*									*
* CLASS: Object3D							*
* AUTHOR: Jean-Francois DOUE						*
* LAST MODIFICATION: 12 Oct 1993					*
*									*
* This class is an abstract root class for all the 3D objects.		*
* It stores the position an the orientation of the object		*
*									*
************************************************************************/

#ifndef Object3D_h
#define Object3D_h 1
#include "algebra3.h"

class Object3D
{
protected:
 vec3	pos;
 mat4	orient;

public:

 void setPosition(vec3& p) { pos = p; };
 vec3 position() { return pos; };
 void setOrientation(mat4& o) { orient = o; };
 mat4 orientation() { return orient; };

 // friends
 friend istream& operator >> (istream& s, Object3D& a);
};

#endif