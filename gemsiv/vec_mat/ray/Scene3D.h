/************************************************************************
*									*
* CLASS: Scene3D							*
* AUTHOR: Jean-Francois DOUE						*
* LAST MODIFICATION: 12 Oct 1993					*
*									*
* This class implements a 3D scene.					*
* The scene is composed of a camera, a collection of light sources	*
* and a collection of primitives to render.				*
* The scene is capable of ray-tracing itself.				*
* Scenes are typically parsed from a text file description. (see the	*
* README file for a description of the format).				*
*									*
************************************************************************/

#include "Camera.h"
#include "Light.h"
#include "Primitive.h"

class Scene3D
{
protected:
 Camera	    *camera;	    // a camera
 Light	    **lList;	    // a list of point lights
 Primitive  **pList;	    // a list of primitives
 int	    lightN,	    // number of light sources in the list
	    primitiveN;	    // number of primitives in the scene
public:
 Scene3D();
 ~Scene3D();
 char* rayTrace(vec2 res);

 // friends
 friend istream& operator >> (istream& s, Scene3D& a);
};