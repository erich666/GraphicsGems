/*
 * Spherical linear interpolation of unit quaternions with spins
 */
#include <math.h>

#define EPSILON 1.0E-6 			/* a tiny number */
#define TRUE  1
#define FALSE 0

typedef struct {			/* quaternion type */
	double w, x, y, z;
} Quaternion;

slerp(alpha, a, b, q, spin)
	double alpha;			/* interpolation parameter (0 to 1) */
	Quaternion *a, *b;		/* start and end unit quaternions */
	Quaternion *q;			/* output interpolated quaternion */
	int spin;			/* number of extra spin rotations */
{
	double beta;			/* complementary interp parameter */
	double theta;			/* angle between A and B */
	double sin_t, cos_t;		/* sine, cosine of theta */
	double phi;			/* theta plus spins */
	int bflip;			/* use negation of B? */

	/* cosine theta = dot product of A and B */
	cos_t = a->x*b->x + a->y*b->y + a->z*b->z + a->w*b->w;

	/* if B is on opposite hemisphere from A, use -B instead */
 	if (cos_t < 0.0) {
		cos_t = -cos_t;
		bflip = TRUE;
	} else
		bflip = FALSE;

	/* if B is (within precision limits) the same as A,
	 * just linear interpolate between A and B.
	 * Can't do spins, since we don't know what direction to spin.
	 */
	if (1.0 - cos_t < EPSILON) {
		beta = 1.0 - alpha;
 	} else {				/* normal case */
 		theta = acos(cos_t);
 		phi = theta + spin * M_PI;
 		sin_t = sin(theta);
 		beta = sin(theta - alpha*phi) / sin_t;
 		alpha = sin(alpha*phi) / sin_t;
 	}

	if (bflip)
		alpha = -alpha;

	/* interpolate */
 	q->x = beta*a->x + alpha*b->x;
 	q->y = beta*a->y + alpha*b->y;
 	q->z = beta*a->z + alpha*b->z;
 	q->w = beta*a->w + alpha*b->w;
}

