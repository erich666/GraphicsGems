#include "Light.h"

istream& operator >> (istream& s, Light& a)
{
s >> *((Object3D*) &a);
s >> a.col;
return s;
}
