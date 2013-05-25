
#include <math.h>
#include "GraphicsGems.h"

/*=========================================================================*
 *  R A N D _ R O T A T I O N      Author: Jim Arvo, 1991                  *
 *                                                                         *
 *  This routine maps three values (x[0], x[1], x[2]) in the range [0,1]   *
 *  into a 3x3 rotation matrix, M.  Uniformly distributed random variables *
 *  x0, x1, and x2 create uniformly distributed random rotation matrices.  *
 *  To create small uniformly distributed "perturbations", supply          *
 *  samples in the following ranges                                        *
 *                                                                         *
 *      x[0] in [ 0, d ]                                                   *
 *      x[1] in [ 0, 1 ]                                                   *
 *      x[2] in [ 0, d ]                                                   *
 *                                                                         *
 * where 0 < d < 1 controls the size of the perturbation.  Any of the      *
 * random variables may be stratified (or "jittered") for a slightly more  *
 * even distribution.                                                      *
 *                                                                         *
 *=========================================================================*/
void rand_rotation( float x[], Matrix3 *M )
    {
    float theta = x[0] * PITIMES2; /* Rotation about the pole (Z).      */
    float phi   = x[1] * PITIMES2; /* For direction of pole deflection. */
    float z     = x[2] * 2.0;      /* For magnitude of pole deflection. */

    /* Compute a vector V used for distributing points over the sphere  */
    /* via the reflection I - V Transpose(V).  This formulation of V    */
    /* will guarantee that if x[1] and x[2] are uniformly distributed,  */
    /* the reflected points will be uniform on the sphere.  Note that V */
    /* has length sqrt(2) to eliminate the 2 in the Householder matrix. */

    float r  = sqrt( z );
    float Vx = sin( phi ) * r;
    float Vy = cos( phi ) * r;
    float Vz = sqrt( 2.0 - z );    

    /* Compute the row vector S = Transpose(V) * R, where R is a simple */
    /* rotation by theta about the z-axis.  No need to compute Sz since */
    /* it's just Vz.                                                    */

    float st = sin( theta );
    float ct = cos( theta );
    float Sx = Vx * ct - Vy * st;
    float Sy = Vx * st + Vy * ct;

    /* Construct the rotation matrix  ( V Transpose(V) - I ) R, which   */
    /* is equivalent to V S - R.                                        */

    M->element[0][0] = Vx * Sx - ct;
    M->element[0][1] = Vx * Sy - st;
    M->element[0][2] = Vx * Vz;

    M->element[1][0] = Vy * Sx + st;
    M->element[1][1] = Vy * Sy - ct;
    M->element[1][2] = Vy * Vz;

    M->element[2][0] = Vz * Sx;
    M->element[2][1] = Vz * Sy;
    M->element[2][2] = 1.0 - z;   /* This equals Vz * Vz - 1.0 */
    }



