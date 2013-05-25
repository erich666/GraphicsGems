#include "Object3D.h"

/************************************************************************
*									*
* Input from stream.							*
*									*
************************************************************************/

istream& operator >> (istream& s, Object3D& a)
{
s >> a.pos;
s >> a.orient;
return s;
}
