/*
ACCURATE FORM-FACTOR COMPUTATION

Filippo Tampieri
Cornell University
*/
/*
    The routines in this module provide a way of computing the
	radiosity contribution of a source patch to a point on a
	receiving surface.  The source must be a convex quadrilateral
	and have a uniform radiosity distribution (or approximately
	uniform).  For simplicity and clarity, the radiosity is
	computed for a single wavelength.
*/

#include <stdio.h>
#include <math.h>

static float computeFormFactor() ;
static float computeUnoccludedFormFactor() ;
static void splitQuad() ;
static float quadArea() ;

#define X    0
#define Y    1
#define Z    2

/* set vector v to zero */
#define VZERO(v)        (v[X] = v[Y] = v[Z] = 0.0)
/* increment vector v by vector dv */
#define VINCR(v, dv)    (v[X] += dv[X], v[Y] += dv[Y], v[Z] += dv[Z])
/* scale vector v by a constant c */
#define VSCALE(v, c)    (v[X] *= c, v[Y] *= c, v[Z] *= c)
/* copy vector u to vector v */
#define VCOPY(v, u)     (v[X] = u[X], v[Y] = u[Y], v[Z] = u[Z])
/* compute the dot product of vectors u and v */
#define VDOT(u, v)      (u[X] * v[X] + u[Y] * v[Y] + u[Z] * v[Z])
/* compute the cross product of vectors s and t */
#define VCROSS(v, s, t) (v[X] = s[Y] * t[Z] - s[Z] * t[Y], \
                         v[Y] = s[Z] * t[X] - s[X] * t[Z], \
                         v[Z] = s[X] * t[Y] - s[Y] * t[X])
/* compute the length of vector v */
#define VNORM(v)        (sqrt(VDOT(v, v)))
/* subtract vector t from vector s and assign the result to v */
#define VSUB(v, s, t)   (v[X] = s[X] - t[X], \
                         v[Y] = s[Y] - t[Y], \
                         v[Z] = s[Z] - t[Z])
/* compute the average of vectors s and t */
#define VAVG2(v, s, t)  (v[X] = 0.5 * (s[X] + t[X]), \
                         v[Y] = 0.5 * (s[Y] + t[Y]), \
                         v[Z] = 0.5 * (s[Z] + t[Z]))

typedef float Vector[3];
typedef Vector Point;
typedef Vector Normal;

/* represent a quadrilateral as an array of four points */
typedef Point Quad[4];

/*
    computeVisibility(Pr, S)
    Point Pr;
    Quad S;

    computeVisibility--takes as input a receiving point 'Pr' and a
	source 'S' and estimates the visibility of 'S' as seen from 'Pr'.
	It returns 0 for total occlusion, 1 for total visibility, and a
	number representing the fraction of 'S' that can be seen from
	'Pr' in the case of partial occlusion.
*/
extern float computeVisibility();

static float eps_F, eps_A;

/*
    computeContribution--takes as input the descriptions of a source
	patch and a receiving point and returns the radiosity
	contribution of the source to the receiver. The source is
	described as a quadrilateral 'S', its UNIT normal 'Ns', and its
	average unshot radiosity 'Bs'. The receiving point is described
	by its location 'Pr', its UNIT normal 'Nr', and its reflectivity
	'rho'. The parameter 'eps_B' controls the accuracy of the
    computation in the presence of partial occlusion; smaller values
	of 'eps_B' will result in more accurate estimates at the cost of
	computation speed. The parameter 'minA' provides the means to
	control the recursive subdivision of the source: no region of
	area less than or equal to 'minA' will be subdivided.
*/
float computeContribution(Pr, Nr, rho, S, Ns, Bs, eps_B, minA)
Point Pr;
Normal Nr, Ns;
float rho, Bs, eps_B, minA;
Quad S;
{
    Vector v;
    float computeFormFactor();

    if(VDOT(Nr, Ns) >= 0.0)
        /* the receiving point is oriented away from the source */
        return 0.0;
    
    VSUB(v, Pr, S[0]);
    if(VDOT(Ns, v) <= 0.0)
        /* the receiving point is behind the source */
        return 0.0;

    eps_F = eps_B / (rho * Bs);

    eps_A = minA;

    return(rho * Bs * computeFormFactor(Pr, Nr, S, Ns));
}

/*
    computeFormFactor--takes as input the descriptions of a source
	patch and a receiving point and returns the form-factor from a
	differential area centered around the receiving point to the
	finite area source.  The source is described as a quadrilateral
	'S' and its UNIT normal 'Ns'.  The receiving point is described
	by its location 'Pr' and its UNIT normal 'Nr'. This routine
	relies on the external function 'computeVisibility' to determine
	a visibility factor.
*/
static float computeFormFactor(Pr, Nr, S, Ns)
Point Pr;
Normal Nr, Ns;
Quad S;
{
    int j;
    Quad dS[4];
    void splitQuad();
    float Frs, vis, computeUnoccludedFormFactor(), quadArea();

    vis = computeVisibility(Pr, S);
    if(vis <= 0.0)
        Frs = 0.0;
    else if(vis >= 1.0)
        Frs = computeUnoccludedFormFactor(Pr, Nr, S);
    else {
        Frs = computeUnoccludedFormFactor(Pr, Nr, S);
        if(Frs <= eps_F || quadArea(S) <= eps_A)
            return(Frs * vis);
        else {
            splitQuad(S, dS);
            Frs = 0.0;
            for(j = 0; j < 4; j++)
                Frs += computeFormFactor(Pr, Nr, dS[j], Ns);
        }
    }

    return(Frs);
}

/*
    computeUnoccludedFormFactor--takes as input the descriptions of
	a source patch and a receiving point and returns the unoccluded
	form-factor from a differential area centered around the
	receiving point to the finite area source.  The source is
	described as a quadrilateral 'S' and its UNIT normal 'Ns'.
	The receiving point is described by its location 'Pr' and its
	UNIT normal 'Nr'. The form-factor is computed analytically.
*/
static float computeUnoccludedFormFactor(Pr, Nr, S)
Point Pr;
Normal Nr;
Quad S;
{
    int i;
    float f, c;
    Vector s, t, sxt, s0;

    f = 0.0;
    VSUB(s, S[3], Pr);
    c = 1.0 / VNORM(s);
    VSCALE(s, c);
    VCOPY(s0, s);
    for(i = 0; i < 4; i++) {
        if(i < 3) {
            VSUB(t, S[i], Pr);
            c = 1.0 / VNORM(t);
            VSCALE(t, c);
        } else
            VCOPY(t, s0);
        VCROSS(sxt, s, t);
        c = 1.0 / VNORM(sxt);
        VSCALE(sxt, c);
        f -= acos(VDOT(s, t)) * VDOT(sxt, Nr);
        VCOPY(s, t);
    }
    
    return(f / (2.0 * M_PI));
}

/*
    splitQuad--takes as input a quadrilateral 'Q', splits it into
	four equal quadrilaterals, and returns the result in array 'dQ'.
*/
static void splitQuad(Q, dQ)
Quad Q, dQ[4];
{
    int i, j;
    Point center, midpt[4];

    /* compute the center of 'Q' and the midpoint of its edges */
    VZERO(center);
    for(i = 0; i < 4; i++) {
        j = (i + 1) % 4;
        VINCR(center, Q[i]);
        VAVG2(midpt[i], Q[i], Q[j]);
    }
    VSCALE(center, 0.25);

    /* initialize the four new quadrilaterals */
    VCOPY(dQ[0][0], Q[0]);
    VCOPY(dQ[0][1], midpt[0]);
    VCOPY(dQ[0][2], center);
    VCOPY(dQ[0][3], midpt[3]);

    VCOPY(dQ[1][1], Q[1]);
    VCOPY(dQ[1][2], midpt[1]);
    VCOPY(dQ[1][3], center);
    VCOPY(dQ[1][0], midpt[0]);

    VCOPY(dQ[2][2], Q[2]);
    VCOPY(dQ[2][3], midpt[2]);
    VCOPY(dQ[2][0], center);
    VCOPY(dQ[2][1], midpt[1]);

    VCOPY(dQ[3][3], Q[3]);
    VCOPY(dQ[3][0], midpt[3]);
    VCOPY(dQ[3][1], center);
    VCOPY(dQ[3][2], midpt[2]);
}

/*
    quadArea--takes as input a quadrilateral 'Q' and returns its
	area.  The area of the quadrilateral is computed as the sum of
	the areas of the two triangles obtained by splitting the
	quadrilateral along one of its diagonals.
*/
static float quadArea(Q)
Quad Q;
{
    float area;
    Vector u, v, uxv;

    VSUB(u, Q[2], Q[1]);
    VSUB(v, Q[0], Q[1]);
    VCROSS(uxv, u, v);
    area = VNORM(uxv);

    VSUB(u, Q[0], Q[3]);
    VSUB(v, Q[2], Q[3]);
    VCROSS(uxv, u, v);
    area += VNORM(uxv);

    return area * 0.5;
}

