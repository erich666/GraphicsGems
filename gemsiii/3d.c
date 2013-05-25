/* rolling-ball-gems3.c */
/* 
 * Take a 3D graphics object "obj" and the incremental mouse
 * motion data "(dx,dy,state)" and rotate the object
 * using the rolling ball algorithm.  A generic interactive
 * graphics interface is assumed that maps onto common protocols 
 * such as Xlib in an obvious way.
 *
 * It is assumed that an object "obj" of type "Polyhedron" has
 * already been drawn once on the display, and that it carries
 * a 4x4 frame matrix "obj->frame[i][j]" specifying the position
 * and orientation at which it is to be drawn.
 *
 * Make3DRot  is assumed to construct a 4x4 rotation matrix from
 * the angle and axis parameters as shown in the text and in
 * GGI "Rotation Tools," by M. Pique, p.466.
 *
 * Make3DTranslation  is assumed to store an (x,y,z) position in a 4x4 
 * matrix in such a way that it can be extracted by  Tx = obj->frame[3][0]; .
 *
 * CombineMatrices3D  performs a left to right matrix multiplication,
 * leaving the result in the location specified by the last argument.
 *
 * CopyMatrix3D  copies a matrix into another.
 *
 * Entry:  display       - generic graphics device specification.
 *         obj           - the object (typically a wire frame) to be rotated.
 *         dx,dy         - the distance the mouse moved in screen coordinates
 *                         since the previous event was processed.
 *         state         - state of the mouse such as button presses.
 *
 * Exit: Object "obj" erased, rotated, and redrawn.
 */

#include <math.h>
#include "defs.h"

ModifyObject(display, obj, dx, dy, state)
     Display *display;
     Polyhedron *obj;
     int dx, dy, state;
{double Tx, Ty, Tz, n[3], dr, denom, cos_theta, sin_theta;
 double Matrix3D[4][4], TmpMat[4][4], TransToOrigin[4][4], Rmat[4][4];
 static double Radius=100.0;

 /* Example of interactive method: apply rolling ball to
  *  object's orientation as long as mouse Button1 is 
  *  held down.
  */
 if (state == Button1)
   {
     /* Obtain current object position from its frame. */
     Tx = obj->frame[3][0];
     Ty = obj->frame[3][1];
     Tz = obj->frame[3][2];

     /* Compute the rolling ball axis and angle from the incremental mouse 
      * displacements (dx,dy) and compute corresponding rotation matrix RMat.
      * See text for full form of Rmat returned by Make3DRot.
      * NOTE: For window systems using a left-handed screen 
      * coordinate system, the formula (-dy,dx,0) given
      * in the text for the rotation axis direction must
      * be changed to (+dy,dx,0) to give the desired effect!
      * We explicitly use this coordinate system in the example
      * code because so many systems possess this reversal.
      */
     dr = sqrt((double)(dx*dx + dy*dy));
     denom = sqrt(Radius*Radius + dr*dr);
     cos_theta = Radius/denom;
     sin_theta = dr/denom;
     n[0] = (double)(dy)/dr;   /* Change sign for right-handed coord system. */
     n[1] = (double)(dx)/dr;
     n[2] = 0.0;
     Make3DRot(cos_theta, sin_theta, n, Rmat);

     /* Translate current object frame to origin. */
     Make3DTranslation(-Tx, -Ty, -Tz, TransToOrigin);
     CombineMatrices3D(TransToOrigin, obj->frame, Matrix3D);

     /* Rotate about origin. */
     CombineMatrices3D(Rmat, Matrix3D, TmpMat);
     
     /* Translate rotated temporary frame back to original object position. */
     Make3DTranslation(Tx, Ty, Tz, TransToOrigin);
     CombineMatrices3D(TransToOrigin, TmpMat, Matrix3D);

     /* Erase current object. */
     SetFunction(display, ERASE);
     DrawObject(display, obj);

     /* Install new frame in object, draw rotated object. */
     CopyMatrix3D(Matrix3D, obj->frame);
     SetFunction(display, DRAW);
     DrawObject(display, obj);

   }}

