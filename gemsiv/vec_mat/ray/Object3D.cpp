#include "Object3D.h"

/************************************************************************
*									*
* Input from stream.							*
*									*
************************************************************************/

std::istream& operator >> (std::istream& s, Object3D& a)
{
s >> a.pos;
s >> a.orient;
return s;
}
