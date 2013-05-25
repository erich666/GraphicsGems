/************************************************
 * POLY.CPP
 * Andreas Leipelt, "Ray Tracing a Swept Sphere"
 * from "Graphics Gems", Academic Press
 *
 * Implementation of the polynomial class. The code is
 * not complete ! You need to insert a root solver in
 * the method 'root_between' .
 */

#include <math.h>
#include "poly.h"

// constructor of the polynomial class 
polynomial::polynomial()
{
  deg = 0;
  for (double *fp = &coef[MAX_DEGREE]; fp >= coef; fp--) *fp = 0.0;
}

// evaluates the polynomial with Horner's scheme.
double polynomial::eval(double x)
{
  double *fp = &coef[deg], val;
  for (val = *fp--; fp >= coef; fp--) val = val*x + *fp;
  return val;
}

// returns the first derivative of the polynomial.
polynomial polynomial::derivative()
{
  polynomial ret;

  if (!deg) return ret;
  ret.deg = deg-1;
  for (int i=0; i <= ret.deg; i++) ret.coef[i] = (i+1)*coef[i+1];
  return ret;
}

// returns the absolute minimum of the given polynomial in the
// interval [a , b]
double polynomial::min(double a, double b)
{
  double roots[MAX_DEGREE], tmp, Min = eval(a);

  int n = derivative().roots_between(a, b, roots);
  roots[n] = b;
  for (int i=0; i <= n; i++) {
    tmp = eval(roots[i]);
    if (tmp < Min) Min = tmp;
  }
  return Min;
}

// returns the absolute maximum of the given polynomial in the
// interval [a ; b]
double polynomial::max(double a, double b)
{
  double roots[MAX_DEGREE], tmp, Max = eval(a);

  int n = derivative().roots_between(a, b, roots);
  roots[n] = b;
  for (int i=0; i <= n; i++) {
    tmp = eval(roots[i]);
    if (tmp > Max) Max = tmp;
  }
  return Max;
}

int polynomial::roots_between(double a, double b, double *roots)
{
  // This function should return the number of roots between
  // a  and  b  and the array 'roots' should contain these roots.
  // Refer to Hook and McAree, "Using Sturm Sequences to Bracket
  // Real Roots of Polynomial Equations" in "Graphics Gems I"
  return 0;
}

polynomial operator+(polynomial& p, polynomial& q)
{
  polynomial sum;

  if (p.deg < q.deg) sum.deg = q.deg;
  else sum.deg = p.deg;
  for (int i=0; i <= sum.deg; i++)
     sum.coef[i] = p.coef[i] + q.coef[i];
  if (p.deg == q.deg) {
    while (sum.deg > -1 && fabs(sum.coef[sum.deg]) < polyeps)
       sum.coef[sum.deg--] = 0.0;
    if (sum.deg < 0) sum.deg = 0;
  }
  return sum;
}

polynomial operator-(polynomial& p, polynomial& q)
{
  polynomial dif;

  if (p.deg < q.deg) dif.deg = q.deg;
  else dif.deg = p.deg;
  for (int i=0; i <= dif.deg; i++)
     dif.coef[i] = p.coef[i] - q.coef[i];
  if (p.deg == q.deg) {
    while (dif.deg > -1 && fabs(dif.coef[dif.deg]) < polyeps)
       dif.coef[dif.deg--] = 0.0;
    if (dif.deg < 0) dif.deg = 0;
  }
  return dif;
}

polynomial operator*(polynomial& p, polynomial& q)
{
  polynomial prod;

  prod.deg = p.deg + q.deg;
  for (int i=0; i <= p.deg; i++)
    for (int j=0; j <= q.deg; j++)
       prod.coef[i+j] += p.coef[i]*q.coef[j];
  return prod;
}

polynomial operator*(double s, polynomial& p)
{
  polynomial scale;

  if (s == 0.0) return scale;
  scale.deg = p.deg;
  for (int i=0; i <= p.deg; i++) scale.coef[i] = s*p.coef[i];
  return scale;
}
