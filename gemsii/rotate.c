
#include <math.h>
#include "GraphicsGems.h"

/*=========================================================================*
 *  Author: Jim Arvo                                                       *
 *                                                                         *
 *  This routine maps three values (x[0], x[1], x[2]) to a 3x3 rotation    *
 *  matrix, M.  If x0, x1, and x2 are uniformly distributed random numbers *
 *  in [0,1], then M will be a random rotation matrix.                     *
 *                                                                         *
 *  NOTE: This function will not produce UNIFORMLY distributed rotation    *
 *  matrices as claimed in Gems II.  Watch for a better version in         *
 *  Gems III.                                                              *
 *                                                                         *
 *=========================================================================*/
void Rand_rotation( x, M )
float x[];
Matrix3 *M;
    {
    float  a, b, c, d, s;
    float  z, r, theta, omega;       
    float  bb, cc, dd;
    float  ab, ac, ad;
    float  bc, bd, cd;

    /* Use the random variables x[0] and x[1] to determine the axis of  */
    /* rotation in cylindrical coordinates and the random variable x[2] */
    /* to determine the amount of rotation, omega, about this axis.     */

    z = x[0];
    r = sqrt( 1 - z * z );
    theta = 2.0 * PI * x[1];
    omega = PI * x[2];

    /* Compute the unit quaternion (a,b,c,d) where a is the cosine of    */
    /* half the rotation angle and the axis vector (b,c,d) is determined */
    /* by "r", "theta" and "z" computed above.                           */

    s = sin( omega );
    a = cos( omega );
    b = s * cos( theta ) * r;
    c = s * sin( theta ) * r;
    d = s * z;

    /* Compute all the pairwise products of a, b, c, and d, except a * a. */

    bb = b * b;   cc = c * c;   dd = d * d;
    ab = a * b;   ac = a * c;   ad = a * d;
    bc = b * c;   bd = b * d;   cd = c * d;

    /* Construct an orthogonal matrix corresponding to  */
    /* the unit quaternion (a,b,c,d).                   */

    M->element[0][0] = 1.0 - 2.0 * ( cc + dd );
    M->element[0][1] =       2.0 * ( bc + ad );
    M->element[0][2] =       2.0 * ( bd - ac );

    M->element[1][0] =       2.0 * ( bc - ad );
    M->element[1][1] = 1.0 - 2.0 * ( bb + dd );
    M->element[1][2] =       2.0 * ( cd + ab );

    M->element[2][0] =       2.0 * ( bd + ac );
    M->element[2][1] =       2.0 * ( cd - ab );
    M->element[2][2] = 1.0 - 2.0 * ( bb + cc );

    } /* Rand_rotation */



















