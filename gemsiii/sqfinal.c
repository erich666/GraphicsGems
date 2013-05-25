/*
 * This code calculates the volume, mass, and inertia tensors of 
 * superquadric ellipsoids and toroids. The code includes methods
 * to numerically compute gamma functions and beta functions
 */

#include <stdio.h>
#include <math.h>

/*
 * The following function, abgam() is based on a continued fraction numerical
 * method found in Abremowitz and Stegun, Handbook of Mathematical Functions
 * 
 */
double  abgam (x)
double  x;
{
  double  gam[10],
          temp;

  gam[0] = 1./ 12.;
  gam[1] = 1./ 30.;
  gam[2] = 53./ 210.;
  gam[3] = 195./ 371.;
  gam[4] = 22999./ 22737.;
  gam[5] = 29944523./ 19733142.;
  gam[6] = 109535241009./ 48264275462.;
  temp = 0.5*log (2*M_PI) - x + (x - 0.5)*log (x)
    + gam[0]/(x + gam[1]/(x + gam[2]/(x + gam[3]/(x + gam[4] /
	  (x + gam[5]/(x + gam[6]/x))))));

  return temp;
}



/* 
 * A method to compute the gamma() function.
 *
 */  
double  gamma (x)
double  x;
{
  double  result,
          abgam ();
  result = exp (abgam (x + 5))/(x*(x + 1)*(x + 2)*(x + 3)*(x + 4));
  return result;
}

/* 
 * A method to compute the beta() function.
 */  
double  beta (m, n)
double  m,
        n;
{
  double  gamma ();
  return (gamma (m)*gamma (n)/gamma (m + n));
}


/* 
 * A method to compute the volume of a superquadric ellipsoid
 * with axis lengths a1, a2, a3, north-south exponent n
 * and east-west exponent e
 */  
double  sqellipvol (a1, a2, a3, n, e)
double  a1,                               /* x radius */ 
        a2,                               /* y radius */
        a3,                               /* z radius */
        n,                                /* north-south param */
        e;                                /* east-west param */
{
  double  beta ();
  return ((2./ 3.)*a1*a2*a3*e*n*beta (e/2., e/2.)*beta (n, n/2.));
}


/* 
 * A method to compute the volume of a superquadric toroid
 * with axis lengths a1, a2, a3, north-south exponent n
 * east-west exponent e, and hole parameter alpha
 */  
double  sqtoroidvol (a1, a2, a3, n, e, alpha)
double  a1,                               /* x radius */ 
        a2,                               /* y radius */
        a3,                               /* z radius */
        n,                                /* north-south param */
        e,                                /* east-west param */
        alpha;                            /* torus hole-size parameter */
{
  return (2.*a1*a2*a3*alpha*e*n*beta (e/2., e/2.)*beta (n/2., n/2.));

}



/* 
 * A procedure to print the inertia tensor of a canonical superquadric 
 * ellipsoid in its body coordinate system. 
 */  
void sq_ellipsoid_tensor (a1, a2, a3, e, n)
double  a1,
        a2,
        a3,
        e,
        n;
{
  double  iellip[3][3],
          i1E,
          i2E,
          i3E;
  i1E = (2./ 5.)* a1*a1*a1*a2*a3*e*n*beta(3.* e/2., e/2.)*beta(2.* n, n/2.);
  i2E = (2./ 5.)* a1*a2*a2*a2*a3*e*n*beta(e/2., 3.* e/2.)*beta(2.* n, n/2.);
  i3E = (2./ 5.)* a1*a2*a3*a3*a3*e*n*beta(e/2., e/2.)*beta(n, 3.* n/2.);

  iellip[0][0] = 0;
  iellip[1][0] = 0;
  iellip[2][0] = 0;
  iellip[0][1] = 0;
  iellip[1][1] = 0;
  iellip[2][1] = 0;
  iellip[0][2] = 0;
  iellip[1][2] = 0;
  iellip[2][2] = 0;
  
  iellip[0][0] = i2E + i3E;
  iellip[1][1] = i1E + i3E;
  iellip[2][2] = i1E + i2E;
  
  printf ("ellipsoid inertia tensor in body coordinates\n");

  printf ("  iellip1 = %f %f %f \n", iellip[0][0], iellip[1][0], iellip[2][0]);
  printf ("  iellip2 = %f %f %f \n", iellip[0][1], iellip[1][1], iellip[2][1]);
  printf ("  iellip3 = %f %f %f \n", iellip[0][2], iellip[1][2], iellip[2][2]);

}


/* 
 * A procedure to print the inertia tensor of a canonical superquadric 
 * toroid in its body coordinate system. 
 */  
void sq_toroid_tensor ( a1, a2, a3, e, n, alpha)
double  a1,
        a2,
        a3,
        e,
        n,
        alpha;
{
  double  itor[3][3],
          i1T,
          i2T,
          i3T;
  i1T = a1*a1*a1*a2*a3*alpha*e*n*beta(3*e/2,e/2)*(2*alpha*alpha*beta(n/2,n/2)+
      3*beta(3*n/2,n/2));
  i2T = a1*a2*a2*a2*a3*alpha*e*n*beta(e/2,3*e/2)*(2*alpha*alpha*beta(n/2,n/2)+
      3*beta(3*n/2,n/2));
  i3T = a1*a2*a3*a3*a3*alpha*e*n*beta(e/2,e/2)*beta(n/2,3*n/2);

    itor[0][0] = 0;
    itor[1][0] = 0;
    itor[2][0] = 0;
    itor[0][1] = 0;
    itor[1][1] = 0;
    itor[2][1] = 0;
    itor[0][2] = 0;
    itor[1][2] = 0;
    itor[2][2] = 0;
    itor[0][0] = i2T + i3T;
    itor[1][1] = i1T + i3T;
    itor[2][2] = i1T + i2T;
    printf ("toroid inertia tensor in body coordinates\n");
    printf ("  itor1  = %f %f %f \n", itor[0][0], itor[1][0], itor[2][0]);
    printf ("  itor2  = %f %f %f \n", itor[0][1], itor[1][1], itor[2][1]);
    printf ("  itor3  = %f %f %f \n", itor[0][2], itor[1][2], itor[2][2]);
}

/* 
 * A procedure to print the inertia tensor components in world coordinates,
 * given an inertia tensor in body coordinates, and the 3x3 rotation matrix
 * which rotates body vectors into world coordinates.  
 */  
void iworld (Ibody, R)
double  Ibody[3][3],
         R[3][3];
{
  double  Iworld[3][3];
 Iworld[0][0] =
    (R[0][0]*Ibody[0][0]+R[0][1]*Ibody[1][0]+R[0][2]*Ibody[2][0])*R[0][0] +
    (R[0][0]*Ibody[0][1]+R[0][1]*Ibody[1][1]+R[0][2]*Ibody[2][1])*R[0][1] +
    (R[0][0]*Ibody[0][2]+R[0][1]*Ibody[1][2]+R[0][2]*Ibody[2][2])*R[0][2];
  
  Iworld[1][0] =
     (R[1][0]*Ibody[0][0]+R[1][1]*Ibody[1][0]+R[1][2]*Ibody[2][0])*R[0][0]+
     (R[1][0]*Ibody[0][1]+R[1][1]*Ibody[1][1]+R[1][2]*Ibody[2][1])*R[0][1]+
     (R[1][0]*Ibody[0][2]+R[1][1]*Ibody[1][2]+R[1][2]*Ibody[2][2])*R[0][2];
  
  Iworld[2][0] =
     (R[2][0]*Ibody[0][0]+R[2][1]*Ibody[1][0]+R[2][2]*Ibody[2][0])*R[0][0]+
     (R[2][0]*Ibody[0][1]+R[2][1]*Ibody[1][1]+R[2][2]*Ibody[2][1])*R[0][1]+
     (R[2][0]*Ibody[0][2]+R[2][1]*Ibody[1][2]+R[2][2]*Ibody[2][2])*R[0][2];

  Iworld[0][1] =
     (R[0][0]*Ibody[0][0]+R[0][1]*Ibody[1][0]+R[0][2]*Ibody[2][0])*R[1][0]+
     (R[0][0]*Ibody[0][1]+R[0][1]*Ibody[1][1]+R[0][2]*Ibody[2][1])*R[1][1]+
     (R[0][0]*Ibody[0][2]+R[0][1]*Ibody[1][2]+R[0][2]*Ibody[2][2])*R[1][2];
  
  Iworld[1][1] =
     (R[1][0]*Ibody[0][0]+R[1][1]*Ibody[1][0]+R[1][2]*Ibody[2][0])*R[1][0]+
     (R[1][0]*Ibody[0][1]+R[1][1]*Ibody[1][1]+R[1][2]*Ibody[2][1])*R[1][1]+
     (R[1][0]*Ibody[0][2]+R[1][1]*Ibody[1][2]+R[1][2]*Ibody[2][2])*R[1][2];
  
  Iworld[2][1] =
     (R[2][0]*Ibody[0][0]+R[2][1]*Ibody[1][0]+R[2][2]*Ibody[2][0])*R[1][0]+
     (R[2][0]*Ibody[0][1]+R[2][1]*Ibody[1][1]+R[2][2]*Ibody[2][1])*R[1][1]+
     (R[2][0]*Ibody[0][2]+R[2][1]*Ibody[1][2]+R[2][2]*Ibody[2][2])*R[1][2];
   
  Iworld[0][2] =
     (R[0][0]*Ibody[0][0]+R[0][1]*Ibody[1][0]+R[0][2]*Ibody[2][0])*R[2][0]+
     (R[0][0]*Ibody[0][1]+R[0][1]*Ibody[1][1]+R[0][2]*Ibody[2][1])*R[2][1]+
     (R[0][0]*Ibody[0][2]+R[0][1]*Ibody[1][2]+R[0][2]*Ibody[2][2])*R[2][2];
  
  Iworld[1][2] =
     (R[1][0]*Ibody[0][0]+R[1][1]*Ibody[1][0]+R[1][2]*Ibody[2][0])*R[2][0]+
     (R[1][0]*Ibody[0][1]+R[1][1]*Ibody[1][1]+R[1][2]*Ibody[2][1])*R[2][1]+
     (R[1][0]*Ibody[0][2]+R[1][1]*Ibody[1][2]+R[1][2]*Ibody[2][2])*R[2][2];
  
  Iworld[2][2] =
     (R[2][0]*Ibody[0][0]+R[2][1]*Ibody[1][0]+R[2][2]*Ibody[2][0])*R[2][0]+
     (R[2][0]*Ibody[0][1]+R[2][1]*Ibody[1][1]+R[2][2]*Ibody[2][1])*R[2][1]+
     (R[2][0]*Ibody[0][2]+R[2][1]*Ibody[1][2]+R[2][2]*Ibody[2][2])*R[2][2];
  
  printf ("toroid inertia tensor in body coordinates\n");
  printf (" Iworld1 = %f %f %f \n", Iworld[0][0], Iworld[1][0], Iworld[2][0]);
  printf (" Iworld2 = %f %f %f \n", Iworld[0][1], Iworld[1][1], Iworld[2][1]);
  printf (" Iworld3 = %f %f %f \n", Iworld[0][2], Iworld[1][2], Iworld[2][2]);
}

/*
 * sgn(x) returns -1.0 or 1.0 for speed. 
 * sgn(x) can return 0.0 on zero if you wish, depending on
 * your convention 
 */ 
double sgn(x)
double x;
{
  if (x <= 0.0) return (-1.0);
  else return(1.0);
} 
  

/*
 * computes position on the surface of a superquadric ellipsoid
 * v goes from -Pi/2 to Pi/2; u  goes from -Pi to Pi. 
 *
 */ 
void sqellipsoidposn(a1,a2,a3,n,e,alpha,u,v)
double a1,a2,a3,n,e,alpha,u,v;
{
  double cu, su, cv, sv, x,y,z;
  cu = cos(u);
  su = sin(u);
  cv = cos(v);
  sv = cos(v);

  x = a1*(alpha + pow(cv,n))*pow(cu,e)*sgn(cu)*sgn(cv);
  y = a2*(alpha + pow(cv,n))*pow(su,e)*sgn(su)*sgn(cv);
  z = a3*pow(sv,n)*sgn(sv);
}  


/*
 * computes position on the surface of a superquadric toroid
 * u and v go from -Pi to Pi
 */

void sqtoroidposn(a1,a2,a3,n,e,u,v)
double a1,a2,a3,n,e,u,v;
{
  double cu, su, cv, sv, x,y,z;
  cu = cos(u);
  su = sin(u);
  cv = cos(v);
  sv = cos(v);

  x = a1*pow(cv,n)*pow(cu,e)*sgn(cu)*sgn(cv);
  y = a2*pow(cv,n)*pow(su,e)*sgn(su)*sgn(cv);
  z = a3*pow(sv,n)*sgn(sv);
 
}  

/*  
 * A procedure to test some of the above code
 * 
 */ 
main () {
  printf (" gamma(1)= 1.0 = %12.10lf\n", gamma (1.0));
  printf (" gamma(1/2)^2= Pi =%12.10lf\n", gamma (0.5)*gamma (0.5));
  printf (" gamma(2)= 1.0 = %12.10lf\n", gamma (2.0));
  printf (" gamma(3)= 2.0 = %12.10lf\n", gamma (3.0));
  printf (" gamma(4)= 6.0 = %12.10lf\n", gamma (4.0));
  printf("\n");
  printf ("beta(1,1)= 1.0 = %12.10lf\n", beta (1.0, 1.0));
  printf ("beta(1,1/2)= 2.0 = %12.10lf\n", beta (1.0, 0.5));
  printf ("beta(1/2,1/2)= Pi = %12.10lf\n", beta (0.5, 0.5));
  printf("\n");
  printf ("sq ellipsoid volume/pi= 4/3 = %12.10lf\n",
                      sqellipvol(1., 1., 1., 1., 1.)/M_PI);
  printf ("sq toroid volume/pi^2 = 2.0 = %12.10lf\n",
           sqtoroidvol (1., 1., 1., 1., 1., 1.)/M_PI/M_PI);
  printf("\n");
  sq_ellipsoid_tensor (1., 1., 1., 1., 1.);
  sq_toroid_tensor (1., 1., 1., 1., 1., 1.);
}

