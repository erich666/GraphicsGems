/************************************************************************
*									*
* CLASS: Polyhedron							*
* AUTHOR: Jean-Francois DOUE						*
* LAST MODIFICATION: 12 Oct 1993					*
*									*
* This class implements a 3D concave polyhedron.			*
* The polyhedron is defined by the following elements:			*
* + an array which contains the its vertices (vList).			*
* + an array of "facets" (iList). Each facet is itself defined as an	*
*   array of integer. The first one is the number of points composing	*
*   the facet. The other ones are the indices of these vertices in the	*
*   vList array.							*
* + an array of dimension 4 vectors, which stores the equations of the	*
*   planes on which the facets lie.					*
*									*
************************************************************************/

#ifndef Polyhedron_h
#define Polyhedron_h 1
#include "Primitive.h"

class Polyhedron : public Primitive
{
protected:
 vec3	*vList;	    // vertices of the polyhedron
 vec4	*pList;	    // equations for the planes supporting the facets
 int	vertexN,    // number of vertices of the polyhedron
	facetN,	    // number of facets of the polyhedron
	**iList;    // lists of the vertex indices which define the facets

public:

 // Constructors and destructors
 ~Polyhedron();

 // virtual methods
 double intersect(vec3& ray_org, vec3& ray_dir);
 vec3 normalAt(vec3& p);

 // friends
 friend istream& operator >> (istream& s, Polyhedron& a);
};

#endif