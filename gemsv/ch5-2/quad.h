/* ------------------------------------------------------------------------- *\
   QUAD.H :

   by Christophe Schlick and Gilles Subrenat (15 May 1994)

   "Ray Intersection of Tessellated Surfaces : Quadrangles versus Triangles"
   in Graphics Gems V (edited by A. Paeth), Academic Press
\* ------------------------------------------------------------------------- */

#ifndef _QUAD_
#define _QUAD_

#include <math.h>
#include "../ch7-7/mactbox/tool.h"
#include "../ch7-7/mactbox/real.h"
#include "../ch7-7/mactbox/vec2.h"
#include "../ch7-7/mactbox/vec3.h"


/*
** Type definitions
*/
typedef struct {
    realvec3  A,B,C,D;   /* Vertices in counter clockwise order */
    realvec3  Normal;    /* Normal vector pointing outwards     */
} QUAD;

typedef struct {
    realvec3  Point;     /* Ray origin                          */
    realvec3  Vector;    /* Ray direction                       */
} RAY;

typedef struct {
    realvec3  Point;     /* Intersection point                               */
    real      Distance;  /* Distance from ray origin to intersection point   */
    real      u, v;      /* Parametric coordinates of the intersection point */
} HIT;

/*
** External declarations
*/
extern bool ray_hit_quad (RAY *, QUAD *, HIT *);

#endif

/* ------------------------------------------------------------------------- */
