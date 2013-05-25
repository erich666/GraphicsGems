/* ------------------------------------------------------------------------- *\
   QUAD_GG.H :

   by Christophe Schlick and Gilles Subrenat (15 May 1994)

   "Ray Intersection of Tessellated Surfaces : Quadrangles versus Triangles"
   in Graphics Gems V (edited by A. Paeth), Academic Press
\* ------------------------------------------------------------------------- */

#ifndef _QUAD_GG_
#define _QUAD_GG_

#include <math.h>
#include "../ch7-7/GG4D/GGems.h"

/*
** Type definitions
*/
typedef double   REAL;

typedef struct {
    Point3   A,B,C,D;   /* Vertices in counter clockwise order */
    Vector3  Normal;    /* Normal vector pointing outwards     */
} QUAD;

typedef struct {
    Point3   Point;     /* Ray origin                          */
    Vector3  Vector;    /* Ray direction                       */
} RAY;

typedef struct {
    Point3   Point;     /* Intersection point                               */
    REAL     Distance;  /* Distance from ray origin to intersection point   */
    REAL     u, v;      /* Parametric coordinates of the intersection point */
} HIT;

/*
** External declarations
*/
extern boolean ray_hit_quad (RAY *, QUAD *, HIT *);

#endif

/* ------------------------------------------------------------------------- */
