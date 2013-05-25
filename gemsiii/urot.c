/* urot.c */
/* Generates a uniform random rotation */
/* Ken Shoemake, September 1991 */

#include <stdlib.h>
#include <math.h>
#include "GraphicsGems.h"

/* Define an INT32 value to be a 32 bit signed integer */
typedef int INT32;

typedef struct {float x, y, z, w;} Quat;
enum QuatPart {X, Y, Z, W, QuatLen, V=0};

/* * * * * *  Utility for quaternion conversion * * * * * */

/** Qt_ToMatrix
 *  Construct rotation matrix from quaternion (unit or not).
 *  Assumes matrix is used to multiply row vector on the right:
 *  vnew = vold mat.  Works correctly for right-handed coordinate system
 *  and right-handed rotations. For column vectors or for left-handed
 *  coordinate systems, transpose the matrix.
 */
void Qt_ToMatrix(Quat q, Matrix3 *out)
{
	double norm = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
	double s = (norm > 0.0) ? 2.0/norm : 0.0;
	double xs = q.x*s,		ys = q.y*s,		zs = q.z*s;
	double wx = q.w*xs,	wy = q.w*ys,	wz = q.w*zs,
		 	xx = q.x*xs,	xy = q.x*ys,	xz = q.x*zs,
		 	yy = q.y*ys,	yz = q.y*zs,	zz = q.z*zs;
	double (*mat)[3] = out->element;
	mat[X][X] = 1.0 - (yy + zz); mat[X][Y] = xy + wz; mat[X][Z] = xz - wy;
	mat[Y][X] = xy - wz; mat[Y][Y] = 1.0 - (xx + zz); mat[Y][Z] = yz + wx;
	mat[Z][X] = xz + wy; mat[Z][Y] = yz - wx; mat[Z][Z] = 1.0 - (xx + yy);
} /* Qt_ToMatrix */


/* * * * * *  How to do it using gaussians * * * * * */

/** Qt_RandomG
 *  Generate uniform random unit quaternion from random seed.
 */
Quat Qt_RandomG(INT32 *argseed)
{
/*  This algorithm generates a gaussian deviate for each coordinate, so
 *  the total effect is to generate a symmetric 4-D gaussian distribution,
 *  by separability. Projecting onto the surface of the hypersphere gives
 *  a uniform distribution.
 */
	Quat q;
	/* uurand generates doubles uniformly distributed between Ð1 and +1 */
	/* This linear congruential generator is inline to exploit signed ints */
	register INT32 seed = *argseed;
#define uurand() ((seed = (seed+1)*69069)/2147483648.0)
	register double x = uurand(), y = uurand();
	register double z = uurand(), w = uurand();
	register double s1, s2;
	double num1, num2, root1, root2, r;
	while ((s1 = x*x+y*y) > 1.0) {x = uurand(); y = uurand();}
	while ((s2 = z*z+w*w) > 1.0) {z = uurand(); w = uurand();}
	/* Now the point (x,y) is uniformly distributed in the unit disk */
	/* So is the point (z,w), independently */
	num1 = -2*log(s1); num2 = -2*log(s2);
	/* Now x*sqrt(num1/s1) is gaussian distributed, using polar method */
	/* Similarly for y, z, and w, and all are independent */
	r = num1 + num2;	/* Sum of squares of four gaussians */
	root1 = sqrt((num1/s1)/r); root2 = sqrt((num2/s2)/r);
	/* Normalizing onto unit sphere gives uniform unit quaternion */
	q.x = x*root1; q.y = y*root1; q.z = z*root2; q.w = w*root2;
	*argseed = seed;
#undef uurand
	return (q);
} /* Qt_RandomG */

/** M3_RandomRotG
 *  Generate uniform random rotation matrix from random seed.
 */
void M3_RandomRotG(INT32 *seed, Matrix3 *m)
{
	Qt_ToMatrix(Qt_RandomG(seed), m);
} /* M3_RandomRotG */


/* * * * * *  How to do it using subgroup algorithm * * * * * */

/** Qt_Random
 *  Generate uniform random unit quaternion from uniform deviates.
 *  Each x[i] should vary between 0 and 1.
 */
Quat Qt_Random(double x[3])
{
/*  The subgroup algorithm can be condensed to this efficient form.
 *  Use rotations around z as a subgroup, with coset representatives
 *  the rotations pointing the z axis in different directions.
 */
	Quat q;
	register double r1 = sqrt(1.0 - x[0]), r2 = sqrt(x[0]);
	register double t1 = PITIMES2*x[1], t2 = PITIMES2*x[2];
	register double c1 = cos(t1), s1 = sin(t1);
	register double c2 = cos(t2), s2 = sin(t2);
	q.x = s1*r1; q.y = c1*r1; q.z = s2*r2; q.w = c2*r2;
	return (q);
} /* Qt_Random */

/** M3_RandomRot
 *  Generate uniform random rotation matrix from uniform deviates.
 */
void M3_RandomRot(double x[3], Matrix3 *m)
{
	Qt_ToMatrix(Qt_Random(x), m);
} /* M3_RandomRot */

/* End of urot.c */

