/*************************************************
 *  SWEEP.CPP
 *  Andreas Leipelt, "Ray Tracing a Swept Sphere"
 *  from "Graphics Gems", Academic Press
 *
 *  This file contains the code to handle a swept sphere in
 *  ray tracing
 */

#include <math.h>
#include "poly.h"

#define rayeps   1E-8  // tolerance for intersection test

// refer to  Andrew Woo, "Fast Ray-Box Intersection",
// "Graphics Gems I"
extern char HitBoundingBox(double*,double*,double*,double*);

// class of the swept sphere primitive
class swept_sph {
  polynomial m[3]; // center of the sphere
  polynomial r;    // radius of the sphere
  polynomial r2;   // r2 = r*r
  double a, b;     // the interval [a;b], where  m  and  r  live
  double minB[3],  // lower left corner of the bounding box
         maxB[3];  // upper right corner of the bounding box
  double param;    // parameter of last intersection, used for member
                   // 'normal'
  public:

  swept_sph() {}
  swept_sph(polynomial*,polynomial,double,double);
  int  intersect(double*,double*,double*);
  void normal(double*,double*);
  int  inside(double*);
};

// constructor of the swept_sph-class
swept_sph::swept_sph(polynomial *M, polynomial R, double A, double B)
// M : trajectory of the center of the moving sphere.
//     An array of polynomials, which is interpreted as a
//     vector valued polynomial.
// R : varying radius of the moving sphere. The radius is assumed
//     to be non-negative.
{
  for (int i=0; i < 3; i++) m[i] = M[i];
  r = R;
  r2 = r*r;
  a = A;  b = B;
  // Calculate the axis aligned bounding box
  for (i=0; i < 3; i++) {
    minB[i] = (m[i] - r).min(a, b);
    maxB[i] = (m[i] + r).max(a, b);
  }
}

int swept_sph::intersect(double *origin, double *dir, double *l)
// origin : origin of the ray
// dir    : unit direction of the ray
// t      : intersection parameter of the ray
{
  polynomial p, q, dp, dq, s;
  double save[3];
  double roots[MAX_DEGREE];
  double p_val, q_val, D, test;

  if (!HitBoundingBox(minB, maxB, origin, dir)) return 0;
  // save the constant term of the trajectory
  for (int i=0; i < 3; i++) {
    save[i] = m[i].coef[0];
    m[i].coef[0] -= origin[i];
  }
  p = dir[0]*m[0] + dir[1]*m[1] + dir[2]*m[2];
  q = m[0]*m[0] + m[1]*m[1] + m[2]*m[2] - r2;
  dp = p.derivative();
  dq = q.derivative();
  s = dq*dq + 4.0*dp*(dp*q - p*dq);
  int n = s.roots_between(a, b, roots);
  roots[n++] = a;
  roots[n]   = b;
  *l = 1E20;
  // test all possible values
  for (i=0; i <= n; i++) {
    // calculate the real solutions of the equation
    // l = p_val +- sqrt(p_val*p_val - q_val)
    p_val = p.eval(roots[i]);
    q_val = q.eval(roots[i]);
    D = p_val*p_val - q_val;
    if (D >= 0.0) {
      // check, if the candidate  roots[i]  leads to a better
      // intersection value  l
      D = sqrt(D);
      test = p_val - D;
      if (test < rayeps) test = p_val + D;
      if ((test >= rayeps) && (test < *l)) {
	param = roots[i];
	*l = test;
      }
    }
  }
  // restore the constant term of the trajectory
  for (i=0; i < 3; i++) m[i].coef[0] = save[i];
  if (*l < 1E20) return 1;
  else return 0;
}

void swept_sph::normal(double *IP, double* Nrm)
// IP  : intersection point
// Nrm : normal at IP
{
  double R = r.eval(param);
  // if the radius is zero, return an arbitrary normal.
  if (R < polyeps) {
    Nrm[0] = Nrm[1] = 0.0;
    Nrm[2] = 1.0;
    return;
  }
  for (int i=0; i < 3; i++) Nrm[i] = (IP[i] - m[i].eval(param))/R;
}

// returns 1, if the point P lies inside.
int swept_sph::inside(double *P)
{
  double save[3];
  int is_inside;

  for (int i=0; i < 3; i++) {
    save[i] = m[i].coef[0];
    m[i].coef[0] -= P[i];
  };
  is_inside =
    ((m[0]*m[0]+m[1]*m[1]+m[2]*m[2]-r2).min(a, b) < rayeps);
  for (i=0; i < 3; i++) m[i].coef[0] = save[i];
  return is_inside;
}
