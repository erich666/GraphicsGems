#include "solver.h"

/********************************************************
*							*
* This function determines if a double is small enough	*
* to be zero. The purpose of the subroutine is to try	*
* to overcome precision problems in math routines.	*
*							*
********************************************************/

static int isZero(double x)
{
return x > -EQN_EPS && x < EQN_EPS;
}


int solveLinear(double c[2], double s[1])
{
if (isZero(c[1]))
    return 0;
s[0] = - c[0] / c[1];
return 1;
}



/********************************************************
*							*
* This function determines the roots of a quadric	*
* equation.						*
* It takes as parameters a pointer to the three		*
* coefficient of the quadric equation (the c[2] is the	*
* coefficient of x2 and so on) and a pointer to the	*
* two element array in which the roots are to be	*
* placed.						*
* It outputs the number of roots found.			*
*							*
********************************************************/

int solveQuadric(double c[3], double s[2])
{
double p, q, D;


// make sure we have a d2 equation

if (isZero(c[2]))
    return solveLinear(c, s);


// normal for: x^2 + px + q
p = c[1] / (2.0 * c[2]);
q = c[0] / c[2];
D = p * p - q;

if (isZero(D))
    {
    // one double root
    s[0] = s[1] = -p;
    return 1;
    }

if (D < 0.0)
    // no real root
    return 0;

else
    {
    // two real roots
    double sqrt_D = sqrt(D);
    s[0] = sqrt_D - p;
    s[1] = -sqrt_D - p;
    return 2;
    }
}



/********************************************************
*							*
* This function determines the roots of a cubic		*
* equation.						*
* It takes as parameters a pointer to the four		*
* coefficient of the cubic equation (the c[3] is the	*
* coefficient of x3 and so on) and a pointer to the	*
* three element array in which the roots are to be	*
* placed.						*
* It outputs the number of roots found			*
*							*
********************************************************/

int solveCubic(double c[4], double s[3])
{
int	i, num;
double	sub,
	A, B, C,
	sq_A, p, q,
	cb_p, D;

// normalize the equation:x ^ 3 + Ax ^ 2 + Bx  + C = 0
A = c[2] / c[3];
B = c[1] / c[3];
C = c[0] / c[3];

// substitute x = y - A / 3 to eliminate the quadric term: x^3 + px + q = 0

sq_A = A * A;
p = 1.0/3.0 * (-1.0/3.0 * sq_A + B);
q = 1.0/2.0 * (2.0/27.0 * A *sq_A - 1.0/3.0 * A * B + C);

// use Cardano's formula

cb_p = p * p * p;
D = q * q + cb_p;

if (isZero(D))
    {
    if (isZero(q))
	{
	// one triple solution
	s[0] = 0.0;
	num = 1;
	}
    else
	{
	// one single and one double solution
	double u = cbrt(-q);
	s[0] = 2.0 * u;
	s[1] = - u;
	num = 2;
	}
    }
else
    if (D < 0.0)
	{
	// casus irreductibilis: three real solutions
	double phi = 1.0/3.0 * acos(-q / sqrt(-cb_p));
	double t = 2.0 * sqrt(-p);
	s[0] = t * cos(phi);
	s[1] = -t * cos(phi + M_PI / 3.0);
	s[2] = -t * cos(phi - M_PI / 3.0);
	num = 3;
	}
    else
	{
	// one real solution
	double sqrt_D = sqrt(D);
	double u = cbrt(sqrt_D + fabs(q));
	if (q > 0.0)
	    s[0] = - u + p / u ;
	else
	    s[0] = u - p / u;
	num = 1;
	}

// resubstitute
sub = 1.0 / 3.0 * A;
for (i = 0; i < num; i++)
    s[i] -= sub;
return num;
}




/********************************************************
*							*
* This function determines the roots of a quartic	*
* equation.						*
* It takes as parameters a pointer to the five		*
* coefficient of the quartic equation (the c[4] is the	*
* coefficient of x4 and so on) and a pointer to the	*
* four element array in which the roots are to be	*
* placed. It outputs the number of roots found.		*
*							*
********************************************************/

int
solveQuartic(double c[5], double s[4])
{
double	    coeffs[4],
	    z, u, v, sub,
	    A, B, C, D,
	    sq_A, p, q, r;
int	    i, num;


// normalize the equation:x ^ 4 + Ax ^ 3 + Bx ^ 2 + Cx + D = 0

A = c[3] / c[4];
B = c[2] / c[4];
C = c[1] / c[4];
D = c[0] / c[4];

// subsitute x = y - A / 4 to eliminate the cubic term: x^4 + px^2 + qx + r = 0

sq_A = A * A;
p = -3.0 / 8.0 * sq_A + B;
q = 1.0 / 8.0 * sq_A * A - 1.0 / 2.0 * A * B + C;
r = -3.0 / 256.0 * sq_A * sq_A + 1.0 / 16.0 * sq_A * B - 1.0 / 4.0 * A * C + D;

if (isZero(r))
    {
    // no absolute term:y(y ^ 3 + py + q) = 0
    coeffs[0] = q;
    coeffs[1] = p;
    coeffs[2] = 0.0;
    coeffs[3] = 1.0;

    num = solveCubic(coeffs, s);
    s[num++] = 0;
    }
else
    {
    // solve the resolvent cubic...
    coeffs[0] = 1.0 / 2.0 * r * p - 1.0 / 8.0 * q * q;
    coeffs[1] = -r;
    coeffs[2] = -1.0 / 2.0 * p;
    coeffs[3] = 1.0;
    (void) solveCubic(coeffs, s);

    // ...and take the one real solution...
    z = s[0];

    // ...to build two quadratic equations
    u = z * z - r;
    v = 2.0 * z - p;

    if (isZero(u))
	u = 0.0;
    else if (u > 0.0)
	u = sqrt(u);
    else
	return 0;

    if (isZero(v))
	v = 0;
    else if (v > 0.0)
	v = sqrt(v);
    else
	return 0;

    coeffs[0] = z - u;
    coeffs[1] = q < 0 ? -v : v;
    coeffs[2] = 1.0;

    num = solveQuadric(coeffs, s);

    coeffs[0] = z + u;
    coeffs[1] = q < 0 ? v : -v;
    coeffs[2] = 1.0;

    num += solveQuadric(coeffs, s + num);
    }

// resubstitute
sub = 1.0 / 4 * A;
for (i = 0; i < num; i++)
    s[i] -= sub;

return num;

}



