/************************************************************************
*									*
* CLASS: Light								*
* AUTHOR: Jean-Francois DOUE						*
* LAST MODIFICATION: 12 Oct 1993					*
*									*
* This class implements a simple point light source.			*
* It just contains the color of the light source.			*
*									*
************************************************************************/

#ifndef Light_h
#define Light_h 1
#include "Object3D.h"

class Light: public Object3D
{
protected:
 vec3	col;	    // color of the light source

public:

 vec3 color() { return col; };
 friend istream& operator >> (istream& s, Light& a);
};

#endif