/* ------------------------------------------------------------------------- *\
   AXD.H :

   This package provides an implementation of 6 different algorithms
   for doing procedural axial deformations.

   by Carole Blanc (4 June 1994)

   "A Generic Implementation of Axial Deformation Techniques"
   in Graphics Gems V (edited by A. Paeth), Academic Press
\* ------------------------------------------------------------------------- */

#ifndef _AXD_
#define _AXD_

/*
** This package uses the "Toolbox of Macros Functions for Computer Graphics"
** which provides files : tool.h, real.h, uint.h, sint.h, vec?.h and mat?.h
*/

#include "../ch7-7/mactbox/real.h"

typedef real (*shape) (real);

extern void local_pinch (rv3 *Point, shape Shape, real Ampli);
extern void world_pinch (rv3 *Point, frame3 Frame, shape Shape, real Ampli);
extern void local_taper (rv3 *Point, shape Shape, real Ampli);
extern void world_taper (rv3 *Point, frame3 Frame, shape Shape, real Ampli);
extern void local_mould (rv3 *Point, shape Shape, real Ampli);
extern void world_mould (rv3 *Point, frame3 Frame, shape Shape, real Ampli);
extern void local_twist (rv3 *Point, shape Shape, real Ampli);
extern void world_twist (rv3 *Point, frame3 Frame, shape Shape, real Ampli);
extern void local_shear (rv3 *Point, shape Shape, real Ampli);
extern void world_shear (rv3 *Point, frame3 Frame, shape Shape, real Ampli);
extern void local_bend  (rv3 *Point, shape Shape, real Ampli);
extern void world_bend  (rv3 *Point, frame3 Frame, shape Shape, real Ampli);

#endif

/* ------------------------------------------------------------------------- */
