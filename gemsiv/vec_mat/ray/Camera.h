/************************************************************************
*									*
* CLASS: Camera								*
* AUTHOR: Jean-Francois DOUE						*
* LAST MODIFICATION: 12 Oct 1993					*
*									*
* This class implements a simple model of 3D camera. The camera can	*
* be positioned and rotated arbitrarily in space. Its focal length is	*
* always equal to 1.							*
* This camera is a simplified version of the model proposed by		*
* Foley, Van Dam, Feiner, Hughes in "Computer Graphics, Principles and	*
* Practice". The following diagram shows what conventions are used:	*
*									*
*									*
*		     y							*
*		x    |	^						*
*		 \   |	|						*
*		  \  |	| y film_size	  target object			*
*		   \ |	|		     ____			*
*		    \|	|		    /	 \			*
* z <----+-----------|	v		    |	 |			*
*	 <---- 1 ---->			    \____/			*
*      focal	    position						*
*   point (PRP)	     (VRP)						*
*									*
* The field of view of the camera can be easily specified by passing	*
* the camera opening angles along the x and y axes (in degree). This	*
* system is similar to the one used in Renderman.			*
*									*
************************************************************************/

#ifndef Camera_h
#define Camera_h 1
#include "Object3D.h"

class Camera: public Object3D
{
protected:
 vec2	film_size;

public:

 void setFieldOfView(vec2& v);
 vec2 fieldOfView();
 vec3 pointToRay(vec2&	p);

 // friends
 friend istream& operator >> (istream& s, Camera& a);
};

#endif
