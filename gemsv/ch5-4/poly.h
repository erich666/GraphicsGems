/*************************************************
 *  POLY.H
 *  Andreas Leipelt, "Ray Tracing a Swept Sphere"
 *  from "Graphics Gems", Academic Press
 *
 */

#ifndef POLY_CLASS
#define POLY_CLASS

#define MAX_DEGREE 10
#define polyeps 1E-10  // tolerance for polynomial coefficients

class polynomial {
  public:

  int deg;
  double coef[MAX_DEGREE+1];

  polynomial();
  double     eval(double);
  int        roots_between(double,double,double*);
  double     min(double,double);
  double     max(double,double);
  polynomial derivative();
};

polynomial operator+(polynomial&, polynomial&);
polynomial operator-(polynomial&, polynomial&);
polynomial operator*(polynomial&, polynomial&);
polynomial operator*(double, polynomial&);

#endif
