#include "Scene3D.h"
#include "Sphere.h"
#include "Polyhedron.h"

#include <string.h>
#include <ctype.h>

#define FAR_AWAY 1e06

/************************************************************************
*									*
* Class constructor							*
*									*
************************************************************************/

Scene3D::Scene3D()
{
#define CAPACITY 128

lList = new Light*[CAPACITY];
pList = new Primitive*[CAPACITY];
lightN = primitiveN = 0;
}

/************************************************************************
*									*
* Class destructor							*
*									*
************************************************************************/

Scene3D::~Scene3D()
{
int i;

// delete the light sources
for (i = 0; i < lightN; i++)
    delete lList[i];
delete lList;

// delete the primitives
for (i = 0; i < primitiveN; i++)
    delete pList[i];
delete pList;

// delete the camera
delete camera;
}

/************************************************************************
*									*
* This method renders the scene using ray-tracing, at a resolution	*
* specified by res. The method outputs an array of size			*
* res[VX] * res[VY], each pixel being coded on 24 bits (8 bits per	*
* color channel). The ray-tracing algorithm is simple and		*
* non-recursive: it finds the closest intersection along each ray and	*
* shades the corresponding point using Phong's reflection model.	*
*									*
************************************************************************/

char* Scene3D::rayTrace(vec2 res)
{
int	    i, j,
	    idx = 0;			    // current image data
char	    *picture;			    // storage for the ray-traced image
vec3	    ray_org(camera->position()),    // origin of the ray
	    ray_dir,			    // direction of the ray
	    p,				    // point of intersection
	    n,				    // normal vector
	    l,				    // light vector
	    r,				    // reflection vector
	    I,				    // light intensity
	    p_color;			    // color of the current pixel
Primitive   *hit;			    // object being intersected
double	    u,v,			    // coordinates on the film plane
	    t, t_min,			    // distance to the closest object
	    d;

// allocate enough room for a 24 bits picture
picture = new char[(int)res[VX] * (int)res[VY] * 3];

// for each point in the picture
for (v = 1.; v > -1.; v -= 2. / (res[VY] - 1.))
    for (u = -1.; u < 1.; u += 2. / (res[VX] - 1.)) {
	// compute the direction of the ray through that point
	ray_dir = camera->pointToRay(vec2(u,v));

	// find the intersection with the closest primitive
	t_min = FAR_AWAY;
	for (i=0; i<primitiveN; i++)
	    if ((t = pList[i]->intersect(ray_org, ray_dir)) && t < t_min) {
		t_min = t;
		hit = pList[i];
		}

	// shade the intersection point
	if (t_min < FAR_AWAY) {
	    I = vec3(0.0);
	    p = ray_org + t_min * ray_dir;
	    n = hit->normalAt(p);

	    // for all the light sources in the scene
	    for (j=0; j<lightN; j++) {

		// compute the light vector and the reflection vector
		l = (lList[j]->position() - p).normalize();
		r = 2.0 * (n * l) * n  - l;

		// add the diffuse component
		if ((d = n * l) > 0.0)
		    I += hit->phong(KD) * d * lList[j]->color();

		// add the specular component
		if ((d = -ray_dir * r) > 0.0)
		    I += hit->phong(KS) * pow(d, hit->phong(ES))
		    * lList[j]->color();
		}
	    p_color = min(255.0 * prod(I, hit->color()), vec3(255.0));
	    }
	else
	    p_color = vec3(0.0);
	picture[idx++] = (int) p_color[RED];
	picture[idx++] = (int) p_color[GREEN];
	picture[idx++] = (int) p_color[BLUE];
	}
return picture;
}

/************************************************************************
*									*
* This function converts vocabulary words specified in a vocabulary	*
* table into integers equal to the index of the word in the table.	*
* It automatically skips tabs, comment characters '%'.			*
*									*
************************************************************************/

int parseStream(istream& s, char** vocabulary)
{
int	i;
char	c,
	string[1024];

// skip white space and comments
while(s.get(c) && (isspace(c) || c == '%'))
    if (c == '%')   // allow comment lines starting with the percent sign
	while(s.get(c) && c != '\n') ;
s.putback(c);

// read the text string and force it to uppercase
strcpy(string, "");
s >> string;
for (i=0; i< strlen(string); i++)
    string[i] = toupper(string[i]);

// If the string is in the dictionary, return its index
i = 0;
while(vocabulary[i]) {
    if (strcmp(vocabulary[i], string) == 0)
	break;
    i++;
    }
return i+1;
}

/************************************************************************
*									*
* Input from stream.							*
*									*
************************************************************************/

istream& operator >> (istream& s, Scene3D& a)
{
enum {CAMERA=1,
      LIGHT=2,
      SPHERE=3,
      POLYHEDRON=4,
      UNKNOWN=5};
int	    key;
static char *key_words[] = {"CAMERA",
			    "LIGHT",
			    "SPHERE",
			    "POLYHEDRON",
			    0};

while ((key = parseStream(s, key_words)) && !s.eof())
    switch(key) {
	case CAMERA:
	    a.camera = new Camera;
	    s >> *a.camera;
	    break;

	case LIGHT:
	    Light *light = new Light;
	    a.lList[a.lightN++] = light;
	    s >> *light;
	    break;

	case SPHERE:
	    Sphere *sphere = new Sphere;
	    a.pList[a.primitiveN++] = sphere;
	    s >> *sphere;
	    break;

	case POLYHEDRON:
	    Polyhedron *polyhedron = new Polyhedron;
	    a.pList[a.primitiveN++] = polyhedron;
	    s >> *polyhedron;
	    break;

	case UNKNOWN:
	    cerr << "\nScene 3D: corrupted stream passed to operator >>";
	    return s;
	    break;
	}

return s;
}
