#include "Light.h"

std::istream& operator >> (std::istream& s, Light& a)
{
s >> *((Object3D*) &a);
s >> a.col;
return s;
}
