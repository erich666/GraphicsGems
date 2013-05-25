/*
 *  BezierLength.c, 1994 
 *
 *  Jens Gravesen
 *  Mathematical Institute
 *  Technical University of Denmark
 *  email: J.Gravesen@mat.dtu.dk
 *
 *  The file contain six functions length<n><x>, (with <n>=1,2,3 and
 *  <x>=a, r) which calculate the length of a Bezier curve with a
 *  given bound on the absolute error (<x>=a) or the relative error
 *  (<x>=r), using the error estimates:
 *  1) Lp-Lc.
 *  2) (Lp-Lc)^2.
 *  3) (La^1-La^0)/15.
 *
 *  It is assumed that elsewhere there are defined 
 *  1) A type: BezierCurve which contains information on a BezierCurve
 *     such as the degree and the coordinates of the control points. Eg.:
 *     typedef struct{
 *       int dim, deg; 
 *       double **Q;
 *     } BezierCurve;
 *  Functions:
 *  2) double degree(BezierCurve *b);
 *       which returns the degree of *b. It could be a macro:
 *       #define degree(b) (b)->deg
 *
 *  3) BezierCurve *DiffBezierCurve(BezierCurve *b); 
 *       which returns a pointer to a BezierCurve containing the
 *       forward differences of*b, or NULL in case of failure. 
 *
 *  4) void FreeBezierCurve(BezierCurve *b);
 *       which frees the memory occupied by *b.
 *
 *  5) double length_of_sum(BezierCurve *b);
 *       which returns the length of the sum of the control points of *b.
 *
 *  6) double sum_of_length(BezierCurve *b);
 *       which returns the sum of the length of the control points of *b.
 *
 *  7) BezierCurve *destructive_subdiv(BezierCurve *b); 
 *       The function returns a pointer to the forward difference of b
 *	 and leaves the forward difference of b in b. In case of failure 
 *	 the function returns NULL.
 *
 *  We assume all this has been declared in a header file say, BezierCode.h
 *
 */

#include <math.h>
#include "BezierCode.h"   /* arbitrary name, see above */
 
#define SQRT2      1.4142135623730951
#define ONE_OVER15 0.0666666666666667
#define ABS(x)     (x)<0?-(x):(x)

/* Forward declarations: */
static double destructive_length_1a(BezierCurve *b, double eps);
static double destructive_length_1r(BezierCurve *b, double eps);
static double destructive_length_2a(BezierCurve *b, double eps);
static double destructive_length_2r(BezierCurve *b, double eps);
static double destructive_length_3a(BezierCurve *b, double La0, double eps);
static double destructive_length_3r(BezierCurve *b, double La0, double eps); 



/*********************************************************************
 *  Public functions: 
 *********************************************************************/

/*  
 * -----------------------------------------------------------------
 * BezierLength1a
 *  
 *      Given a BezierCurve produces the arc-length of the curve with 
 *      a given bound on the absolut error.
 *      Using Lp-Lc as the error estimate.
 *  
 *  Results:
 *      The return value is normally the length of the curve, in case
 *      of failure the function returns -HUGE_VAL.
 *
 *  Side effects:
 *      None.
 *
 *------------------------------------------------------------------
 */
double BezierLength1a(b, eps)
     BezierCurve *b;            /* The Bezier Curve */
     double eps;                /* The given tolerance */
{
  BezierCurve *db;

  db = DiffBezierCurve(b);  
  if (!db) {                     /* Something is wrong, */ 
    return -HUGE_VAL;            /* signal it with negative length */
  }
  return destructive_length_1a(db, eps)/(degree(b)+1);
                                 /* destructive_length_1a works with */
				 /* the length multiplied by degree(b)+1 */
} 



/*  
 * -----------------------------------------------------------------
 * BezierLength1r
 *  
 *      Given a BezierCurve produces the arc-length of the curve with 
 *      a given bound on the relative error.
 *      Using Lp-Lc as the error estimate.
 *  
 *  Results:
 *      The return value is normally the length of the curve, in case
 *      of failure the function returns -HUGE_VAL.
 *
 *  Side effects:
 *      None.
 *
 *------------------------------------------------------------------
 */
double BezierLength1r(b, eps)
     BezierCurve *b;            /* The Bezier Curve */
     double eps;                /* The given tolerance */
{
  BezierCurve *db;

  db = DiffBezierCurve(b);  
  if (!db) {                     /* Something is wrong, */ 
    return -HUGE_VAL;            /* signal it with negative length */
  }
  eps /= (degree(b)+1);          /* destructive_length_1r works with */
				 /* the length multiplied by degree(b)+1 */
  return destructive_length_1r(db, eps)/(degree(b)+1);
} 



/*  
 * -----------------------------------------------------------------
 * BezierLength2a
 *  
 *      Given a BezierCurve produces the arc-length of the curve with 
 *      a given bound on the absolut error.
 *      Using (Lp-Lc)^2 as the error estimate.
 *  
 *  Results:
 *      The return value is normally the length of the curve, in case
 *      of failure the function returns -HUGE_VAL.
 *
 *  Side effects:
 *      None.
 *
 *------------------------------------------------------------------
 */
double BezierLength2a(b, eps)
     BezierCurve *b;            /* The Bezier Curve */
     double eps;                /* The given tolerance */
{
  BezierCurve *db;
  
  db = DiffBezierCurve(b);
  if (!db) {                   /* Something is wrong, */ 
    return -HUGE_VAL;          /* signal it with negative length */
  }
  eps = sqrt(eps);             /* (Lp-Lc)^2 < eps <=> Lp-Lc < sqrt(eps) */
  return destructive_length_2a(db, eps)/(degree(b)+1);
                               /* destructive_length_1a works with */
			       /* the length multiplied by degree(b)+1 */
} 



/*  
 * -----------------------------------------------------------------
 * BezierLength2r
 *  
 *      Given a BezierCurve produces the arc-length of the curve with 
 *      a given bound on the relative error.
 *      Using (Lp-Lc)^2 as the error estimate.
 *  
 *  Results:
 *      The return value is normally the length of the curve, in case
 *      of failure the function returns -HUGE_VAL.
 *
 *  Side effects:
 *      None.
 *
 *------------------------------------------------------------------
 */
double BezierLength2r(b, eps)
     BezierCurve *b;            /* The Bezier Curve */
     double eps;                /* The given tolerance */
{
  BezierCurve *db;
  
  db = DiffBezierCurve(b);
  if (!db) {                     /* Something is wrong, */ 
    return -HUGE_VAL;            /* signal it with negative length */
  }
  eps /= (degree(b)+1);          /* destructive_length_1r works with */
				 /* the length multiplied by degree(b)+1 */
  return destructive_length_2r(db, eps)/(degree(b)+1);
} 



/*  
 * -----------------------------------------------------------------
 * BezierLength3a
 *  
 *      Given a BezierCurve produces the arc-length of the curve with 
 *      a given bound on the absolut error.
 *      Using (La^1-La^0)/15 as the error estimate.
 *  
 *  Results:
 *      The return value is normally the length of the curve, in case
 *      of failure the function returns -HUGE_VAL.
 *
 *  Side effects:
 *      None.
 *
 *------------------------------------------------------------------
 */
double BezierLength3a(b, eps)
     BezierCurve *b;            /* The Bezier Curve */
     double eps;                /* The given tolerance */
{
  BezierCurve *db;
  double Lp, Lc, L;
  
  db = DiffBezierCurve(b);
  if (!db) {                    /* Something is wrong, */
    return -HUGE_VAL;           /* signal it with negative length */
  }
  Lp = sum_of_length(db);
  Lc = length_of_sum(db);
  L = 2*Lc + degree(db)*Lp;     /* La^0*(deg+1) of the curve*/

  eps *= 30*(degree(b)+1);      /* (La^1-La)/15 < eps  <=> */
				/* 2*(La^1-La)*(deg+1) < 30*(deg+1)*eps */

  return destructive_length_3a(db,L,eps)/(2*degree(b)+2); 
                                /* destructive_length_3 works with the */
                                /* lenght multiplied by degree(b)+1, */
				/* and return works with curves which */
				/* have twice the right size */ 
}



/*  
 * -----------------------------------------------------------------
 * BezierLength3r
 *  
 *      Given a BezierCurve produces the arc-length of the curve with 
 *      a given bound on the relative error.
 *      Using (La^1-La^0)/15 as the error estimate.
 *  
 *  Results:
 *      The return value is normally the length of the curve, in case
 *      of failure the function returns -HUGE_VAL.
 *
 *  Side effects:
 *      None.
 *
 *------------------------------------------------------------------
 */
double BezierLength3r(b, eps)
     BezierCurve *b;            /* The Bezier Curve */
     double eps;                /* The given tolerance */
{
  BezierCurve *db;
  double Lp, Lc, L;
  
  db = DiffBezierCurve(b);
  if (!db) {                /* Something is wrong, */
    return -HUGE_VAL;       /* signal it with negative length */
  }
  Lp = sum_of_length(db);
  Lc = length_of_sum(db);
  L = 2*Lc + degree(db)*Lp; /* La^0*(deg+1) of the curve */

  
 
  eps *= 15;                /* (La^1-La)/15 < eps*La^1 <=> */
                            /* (La^1-La)*(deg+1) < 15*eps*La^1*(deg+1) */

  return destructive_length_3r(db,L,eps)/(2*degree(b)+2); 
                            /* destructive_length_3 works with the */
                            /* length multiplied by degree(b)+1, and */
			    /* the size of the curves is twice the */
			    /* right size */ 
}



/*********************************************************************
 * Private functions: 
 *********************************************************************/

/*  
 * -----------------------------------------------------------------
 * destructive_length_1a
 *  
 *      Given the forward differences of a BezierCurve produces the 
 *      arc-length of the curve multiplied by (n+1), (n = the degree of
 *      original curve), with the bound (n+1)*eps on the absolut error.
 *      Using Lp-Lc as the error estimate.
 *  
 *  Results:
 *      The return value is normally the length of the curve, in case
 *      of failure the function returns -HUGE_VAL.
 *
 *  Side effects:
 *      The memory held by the input curve is freed.
 *
 *------------------------------------------------------------------
 */
static double destructive_length_1a(b, eps)
     BezierCurve *b;            /* The forward differences of the */
				/* Bezier Curve */
     double eps;                /* The given tolerance */
{
  BezierCurve *b1;
  double Lp, Lc;

  Lp = sum_of_length(b);
  Lc = length_of_sum(b);
  if ( Lp-Lc < eps ) {
    FreeBezierCurve(b);
    return 2*Lc+degree(b)*Lp;      /* the degree of b is n-1 */
  }
  b1 = destructive_subdiv(b);
  if (!b1) {                       /* Something is wrong, */
    FreeBezierCurve(b);            
    return -HUGE_VAL;              /* signal it with negative length */
  }

  /* We don't change eps, instead the two half are twice as big as */
  /* the forward differences of the two halfs of the original curve, */
  /* this correspond to putting eps = eps/2, i.e. to distribute the */
  /* error evenly on the two halfs */

  return (destructive_length_1a(b1,eps)+destructive_length_1a(b,eps))/2;

  /* We work on the forward differences of the control points so when */
  /* we subdivide the control points should be divide by 2, instead we */
  /* divide the length with 2 */
}



/*  
 * -----------------------------------------------------------------
 * destructive_length_1r
 *  
 *      Given the forward differences of a BezierCurve produces the 
 *      arc-length of the curve multiplied by (n+1), (n = the degree of
 *      original curve), with the bound (n+1)*eps on the relative error.
 *      Using Lp-Lc as the error estimate.
 *  
 *  Results:
 *      The return value is normally the length of the curve, in case
 *      of failure the function returns -HUGE_VAL.
 *
 *  Side effects:
 *      The memory held by the input curve is freed.
 *
 *------------------------------------------------------------------
 */
static double destructive_length_1r(b, eps)
     BezierCurve *b;            /* The forward differences of the */
				/* Bezier Curve */
     double eps;                /* The tolerance dividede by n+1 */
{
  BezierCurve *b1;
  double Lp, Lc, L;

  Lp = sum_of_length(b);
  Lc = length_of_sum(b);
  L = 2*Lc + degree(b)*Lp; /* the degree of b is n-1 */
  if (Lp-Lc < eps*L ) { 
    FreeBezierCurve(b);
    return L;
  }
  b1 = destructive_subdiv(b);
  if(!b1) {                 /* Something is wrong, */
    FreeBezierCurve(b);     /* signal it with negative length */
    return -HUGE_VAL;
  }
                            /* Don't change the tolerence */
  return (destructive_length_1r(b1,eps) + destructive_length_1r(b,eps))/2;
  
  /* We work on the forward differences of the control points so when */
  /* we subdivide the control points should be divide by 2, instead we */
  /* divide the length with 2 */
}



/*  
 * -----------------------------------------------------------------
 * destructive_length_2a
 *  
 *      Given the forward differences of a BezierCurve produces the 
 *      arc-length of the curve multiplied by (n+1), (n = the degree of
 *      original curve), with the bound eps^2 on the absolut error.
 *      Using (Lp-Lc)^2 as the error estimate.
 *  
 *  Results:
 *      The return value is normally the length of the curve, in case
 *      of failure the function returns -HUGE_VAL.
 *
 *  Side effects:
 *      The memory held by the input curve is freed.
 *
 *------------------------------------------------------------------
 */
static double destructive_length_2a(b, eps)
     BezierCurve *b;            /* The forward differences of the */
				/* Bezier Curve */
     double eps;                /* The sqaure root of the tolerance */
{
  BezierCurve *b1;
  double Lp, Lc;

  Lp = sum_of_length(b);
  Lc = length_of_sum(b);
  if ( Lp-Lc < eps ) {            /* (Lp-Lc)^2 < eps^2 <=> Lp-Lc < eps */
    FreeBezierCurve(b);
    return 2*Lc + degree(b)*Lp;  /* the degree of b is n-1 */
  }
  b1=destructive_subdiv(b);
  if(!b1) {                 /* Something is wrong, */
    FreeBezierCurve(b);     
    return -HUGE_VAL;       /* signal it with negative length */
  }
  eps *= SQRT2;
  /* We distribute the error evenly on the two halfs, but the two */
  /* halfs are twice as big as the forward differences of the two */
  /* halfs of the original curve, so we have */
  /* (Lp/2-Lc/2)^2 < eps^2/2 <=> Lp-Lc < sqrt(2)*eps  */

  return (destructive_length_2a(b1,eps)+destructive_length_2a(b,eps))/2;

  /* We work on the forward differences of the control points so when */
  /* we subdivide the control points should be divide by 2, instead we */
  /* divide the length with 2 */
}



/*  
 * -----------------------------------------------------------------
 * destructive_length_2r
 *  
 *      Given the forward differences of a BezierCurve produces the 
 *      arc-length of the curve multiplied by (n+1), (n = the degree of
 *      original curve), with the bound eps*(n+1) on the relative error.
 *      Using (Lp-Lc)^2 as the error estimate.
 *  
 *  Results:
 *      The return value is normally the length of the curve, in case
 *      of failure the function returns -HUGE_VAL.
 *
 *  Side effects:
 *      The memory held by the input curve is freed.
 *
 *------------------------------------------------------------------
 */
static double destructive_length_2r(b, eps)
     BezierCurve *b;            /* The forward differences of the */
				/* Bezier Curve */
     double eps;                /* The tolerance dividede by n+1 */
{
  BezierCurve *b1;
  double Lp, Lc, L, err;

  Lp = sum_of_length(b);
  Lc = length_of_sum(b);
  L = 2*Lc + degree(b)*Lp;     /* the degree of b is n-1 */
  err = Lp-Lc;
  if ( err*err < eps*L ) {      
    FreeBezierCurve(b);
    return L;
  }
  b1 = destructive_subdiv(b);
  if(!b1) {                 /* Something is wrong, */
    FreeBezierCurve(b);     
    return -HUGE_VAL;       /* signal it with negative length */
  }
  eps *=2;                  /* We want the same tolerance on the two */
			    /* halfs as we had before, but the size of */
			    /* the control polygon is two times to */
			    /* large. So we have */
                            /* (Lp/2 - Lc/2)^2 < eps*L/2 <=> */
			    /* (Lp-Lc)^2 < 2*eps */

  return (destructive_length_2r(b1,eps) + destructive_length_2r(b,eps))/2;
  
  /* We work on the forward differences of the control points so when */
  /* we subdivide the control points should be divide by 2, instead we */
  /* divide the length with 2 */
}



/*  
 * -----------------------------------------------------------------
 * destructive_length_3a
 *  
 *      Given the forward differences of a BezierCurve produces the 
 *      arc-length of the curve multiplied by 2*(n+1), (n = the degree of
 *      original curve), with the bound eps/(30*(n+1) on the absolut error.
 *      Using (La^1-La^0)/15 as the error estimate.
 *  
 *  Results:
 *      The return value is normally the length of the curve, in case
 *      of failure the function returns -HUGE_VAL.
 *
 *  Side effects:
 *      The memory held by the input curve is freed.
 *
 *------------------------------------------------------------------
 */
static double destructive_length_3a(b, La0, eps)
     BezierCurve *b;            /* The forward differences of the */
				/* Bezier Curve */
     double La0;                /* The "average length" of the */
				/* Bezier curve */
     double eps;                /* The tolerance multiplied by 15*(n+1) */
{
  BezierCurve *b1;
  double Lp, Lc, La1, La2, L, err;

  b1 = destructive_subdiv(b);
  if(!b1) {                       /* Something is wrong, */
    FreeBezierCurve(b);     
    return -HUGE_VAL;             /* signal it with negative length */
  }

  Lp = sum_of_length(b1);
  Lc = length_of_sum(b1);
  La1 = 2*Lc + degree(b1)*Lp;      /* 2*Length*(deg+1) of first half */

  Lp = sum_of_length(b);
  Lc = length_of_sum(b);
  La2 = 2*Lc + degree(b)*Lp;       /* 2*Length*(deg+1) of second half */

  L = La1+La2;                     /* La^1*(deg+1)                  */

  err = L-2*La0;                   /* L = 2*La^1 */
  if ( err < eps && err > -eps) {
    FreeBezierCurve(b1);
    FreeBezierCurve(b);
    return L + err*ONE_OVER15;     /* Do the error correction */
  }

  /* We don't change eps, instead the two half are twice as big as */
  /* the forward differences of the two halfs of the original curve, */
  /* this correspond to putting eps = eps/2, i.e. to distribute the */
  /* error evenly on the two halfs */

  return (destructive_length_3a(b1,La1,eps) +
	  destructive_length_3a(b,La2,eps))/2 ;
  /* We work on the forward differences of the control points so when */
  /* we subdivide the control points should be divide by 2, instead we */
  /* divide the length with 2 */
}



/*  
 * -----------------------------------------------------------------
 * destructive_length_3r
 *  
 *      Given the forward differences of a BezierCurve produces the 
 *      arc-length of the curve multiplied by (n+1), (n = the degree of
 *      original curve), with the bound eps/15 on the relative error.
 *      Using (La^1-La^0)/15 as the error estimate.
 *  
 *  Results:
 *      The return value is normally the length of the curve, in case
 *      of failure the function returns -HUGE_VAL.
 *
 *  Side effects:
 *      The memory held by the input curve is freed.
 *
 *------------------------------------------------------------------
 */
static double destructive_length_3r(b, La0, eps)
     BezierCurve *b;            /* The forward differences of the */
				/* Bezier Curve */
     double La0;                /* The "average length" of the */
				/* Bezier curve */
     double eps;                /* The tolerance multiplied by 15 */
{
  BezierCurve *b1;
  double Lp, Lc, La1, La2, L, err;

  b1 = destructive_subdiv(b);
  if(!b1) {                       /* Something is wrong, */
    FreeBezierCurve(b);     
    return -HUGE_VAL;             /* signal it with negative length */
  }

  Lp = sum_of_length(b1);
  Lc = length_of_sum(b1);
  La1 = 2*Lc + degree(b1)*Lp;      /* 2*Length*(deg+1) of first half */

  Lp = sum_of_length(b);
  Lc = length_of_sum(b);
  La2 = 2*Lc + degree(b)*Lp;       /* 2*Length*(deg+1) of second half */

  L = La1+La2;                     /* 2*La^1*(deg+1)                  */

  err = L-2*La0;                 
  if ( err < eps*L && err > -eps*L ) {
    FreeBezierCurve(b1);
    FreeBezierCurve(b);
    return L + err*ONE_OVER15;     /* Do the error correction */
  }
                                   /* Don't change the tolerence */

  return (destructive_length_3r(b1,La1,eps) +
	  destructive_length_3r(b,La2,eps))/2;
  /* We work on the forward differences of the control points so when */
  /* we subdivide the control points should be divide by 2, instead we */
  /* divide the length with 2 */
}


/* end of file  BezierLength.c, 1994 */
