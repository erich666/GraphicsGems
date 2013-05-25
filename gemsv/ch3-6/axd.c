/* ------------------------------------------------------------------------- *\
   AXD.C :

   This package provides an implementation of 6 different algorithms
   for doing procedural axial deformations.
 
   by Carole Blanc (4 June 1994)

   "A Generic Implementation of Axial Deformation Techniques"
   in Graphics Gems V (edited by A. Paeth), Academic Press
\* ------------------------------------------------------------------------- */

/*
** This package uses the "Toolbox of Macros Functions for Computer Graphics"
** which provides files : tool.h, real.h, uint.h, sint.h, vec?.h and mat?.h
*/

#include "axd.h"
#include "../ch7-7/mactbox/mat3.h"

/*
** Each "local_*" routines inputs/outputs the following arguments
**
** Input:  Point = coordinates of the point in the local frame
**         Shape = shape function of the deformation
**         Ampli = amplitude of the deformation
** Output: Point = coordinates of the deformed point in the local frame
**
** Each "world_*" routines inputs/outputs the following arguments
**
** Input:  Point = coordinates of the point in the world frame
**         Frame = local frame in which the deformation is applied
**         Shape = shape function of the deformation
**         Ampli = amplitude of the deformation
** Output: Point = coordinates of the deformed point in the world frame
**
** Note: The "Frame" argument must be initialized by MAKE_FRAME3 (see "mat3.h")
*/

/*
** pinch : Scale the x coordinate of the object according to z
*/

void local_pinch (realvec3 *Point, shape Shape, real Ampli)
{
  Point->x *= 1.0 - Ampli * Shape (Point->z); 
}

void world_pinch (realvec3 *Point, frame3 Frame, shape Shape, real Ampli)
{
  realvec3 Tmp;

  LOCAL_FRAME3 (Tmp, Frame, *Point);
  local_pinch  (&Tmp, Shape, Ampli);
  WORLD_FRAME3 (*Point, Frame, Tmp);
}

/*
** taper : Scale the polar radius of the object according to z
*/

void local_taper (realvec3 *Point, shape Shape, real Ampli)
{
  register real Tmp;

  Tmp = 1.0 - Ampli * Shape (Point->z); Point->x *= Tmp; Point->y *= Tmp;
}

void world_taper (realvec3 *Point, frame3 Frame, shape Shape, real Ampli)
{
  realvec3 Tmp;

  LOCAL_FRAME3 (Tmp, Frame, *Point);
  local_taper  (&Tmp, Shape, Ampli);
  WORLD_FRAME3 (*Point, Frame, Tmp);
}

/*
** mould : Scale the polar radius of the object according to the polar angle
*/

void local_mould (realvec3 *Point, shape Shape, real Ampli)
{
  register real Tmp;

  Tmp = atan2 (Point->y, Point->x) / PI;
  Tmp = 1.0 - Ampli * Shape (Tmp); Point->x *= Tmp; Point->y *= Tmp;
}

void world_mould (realvec3 *Point, frame3 Frame, shape Shape, real Ampli)
{
  realvec3 Tmp;

  LOCAL_FRAME3 (Tmp, Frame, *Point);
  local_mould  (&Tmp, Shape, Ampli);
  WORLD_FRAME3 (*Point, Frame, Tmp);
}

/*
** twist : Scale the polar angle of the object according to z
*/

void local_twist (realvec3 *Point, shape Shape, real Ampli)
{
  register real Tmp, Cos, Sin;

  Tmp = PI * Ampli * Shape (Point->z);
  Cos = cos (Tmp); Sin = sin (Tmp); Tmp = Point->x;
  Point->x = Cos * Tmp - Sin * Point->y;
  Point->y = Sin * Tmp + Cos * Point->y;
}

void world_twist (realvec3 *Point, frame3 Frame, shape Shape, real Ampli)
{
  realvec3 Tmp;

  LOCAL_FRAME3 (Tmp, Frame, *Point);
  local_twist  (&Tmp, Shape, Ampli);
  WORLD_FRAME3 (*Point, Frame, Tmp);
}

/*
** shear : Translate the z axis of the object along x according to z
*/

void local_shear (realvec3 *Point, shape Shape, real Ampli)
{
  Point->x += Ampli * Shape (Point->z);
}

void world_shear (realvec3 *Point, frame3 Frame, shape Shape, real Ampli)
{
  realvec3 Tmp;

  LOCAL_FRAME3 (Tmp, Frame, *Point);
  local_shear  (&Tmp, Shape, Ampli);
  WORLD_FRAME3 (*Point, Frame, Tmp);
}

/*
** bend : Rotate the z axis of the object around y according to z
*/

void local_bend (realvec3 *Point, shape Shape, real Ampli)
{
  register real Tmp, Cos, Sin;

  Tmp = PI * Ampli * Shape (Point->z);
  Cos = cos (Tmp); Sin = sin (Tmp); Tmp = Point->z;
  Point->z = Cos * Tmp - Sin * Point->x;
  Point->x = Sin * Tmp + Cos * Point->x;
}

void world_bend (realvec3 *Point, frame3 Frame, shape Shape, real Ampli)
{
  realvec3 Tmp;

  LOCAL_FRAME3 (Tmp, Frame, *Point);
  local_bend   (&Tmp, Shape, Ampli);
  WORLD_FRAME3 (*Point, Frame, Tmp);
}

/* ------------------------------------------------------------------------- */
