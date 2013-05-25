/* minimal ray tracer, hybrid version - 888 tokens
 * Paul Heckbert, ucbvax!pixar!ph, 13 Jun 87
 * Using tricks from Darwyn Peachey and Joe Cychosz. */

#define TOL 1e-7
#define AMBIENT vec U, black, amb
#define SPHERE struct sphere {vec cen, color; double rad, kd, ks, kt, kl, ir} \
    *s, *best, sph[]
typedef struct {double x, y, z} vec;
#include "ray.h"
yx;
double u, b, tmin, sqrt(), tan();

double vdot(A, B)
vec A, B;
{
    return A.x*B.x + A.y*B.y + A.z*B.z;
}

vec vcomb(a, A, B)	/* aA+B */
double a;
vec A, B;
{
    B.x += a*A.x;
    B.y += a*A.y;
    B.z += a*A.z;
    return B;
}

vec vunit(A)
vec A;
{
    return vcomb(1./sqrt(vdot(A, A)), A, black);
}

struct sphere *intersect(P, D)
vec P, D;
{
    best = 0;
    tmin = 1e30;
    s = sph+NSPHERE;
    while (s-->sph)
	b = vdot(D, U = vcomb(-1., P, s->cen)),
	u = b*b-vdot(U, U)+s->rad*s->rad,
	u = u>0 ? sqrt(u) : 1e31,
	u = b-u>TOL ? b-u : b+u,
	tmin = u>=TOL && u<tmin ?
	    best = s, u : tmin;
    return best;
}

vec trace(level, P, D)
vec P, D;
{
    double d, eta, e;
    vec N, color;
    struct sphere *s, *l;

    if (!level--) return black;
    if (s = intersect(P, D));
    else return amb;

    color = amb;
    eta = s->ir;
    d = -vdot(D, N = vunit(vcomb(-1., P = vcomb(tmin, D, P), s->cen)));
    if (d<0)
	N = vcomb(-1., N, black),
	eta = 1/eta,
	d = -d;
    l = sph+NSPHERE;
    while (l-->sph)
	if ((e = l->kl*vdot(N, U = vunit(vcomb(-1., P, l->cen)))) > 0 &&
	    intersect(P, U)==l)
		color = vcomb(e, l->color, color);
    U = s->color;
    color.x *= U.x;
    color.y *= U.y;
    color.z *= U.z;
    e = 1-eta*eta*(1-d*d);
    /* the following is non-portable: we assume right to left arg evaluation.
     * (use U before call to trace, which modifies U) */
    return vcomb(s->kt,
	e>0 ? trace(level, P, vcomb(eta, D, vcomb(eta*d-sqrt(e), N, black)))
	    : black,
	vcomb(s->ks, trace(level, P, vcomb(2*d, N, D)),
	    vcomb(s->kd, color, vcomb(s->kl, U, black))));
}

main()
{
    printf("%d %d\n", SIZE, SIZE);
    while (yx<SIZE*SIZE)
	U.x = yx%SIZE-SIZE/2,
	U.z = SIZE/2-yx++/SIZE,
	U.y = SIZE/2/tan(AOV/114.5915590261),	/* 360/PI~=114 */
	U = vcomb(255., trace(DEPTH, black, vunit(U)), black),
	printf("%.0f %.0f %.0f\n", U);		/* yowsa! non-portable! */
}
