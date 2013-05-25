

















#include "GraphicsGems.h"
#undef ON

/* Comparison macros */
#define LEFT  -1  /* value to left of (less than) another */
#define ON     0  /* two values equal */
#define RIGHT  1  /* value to right of (greater than) another */

typedef struct endpoint
{ /* An a priori endpoint */
  short x, y;
} ENDPOINT;

typedef struct segment
{ /* A priori line segment with integer endpoints */
  ENDPOINT first, last; /* defined to be ordered from "first" to "last" */
} SEGMENT;

typedef struct param
{ /* Parameterized description of an intersection along a SEGMENT */
  long num, denom;
} PARAM;

typedef struct subsegment
{
  SEGMENT apriori;     /* The a priori segment this subsegment falls on */
  PARAM ParOne, ParTwo;/* Parameterized description of intersection points */
} SUBSEGMENT;

typedef struct intpoint
{ /* Intersection point returned by SegIntersect */
  PARAM par[2];  /* par[0] is on the first segment, par[1] on the second */
  long a, b, c, d; /* storing these allows fast computation for direction of
		      crossing */
} INTPOINT;

/* All a priori endpoints must have coordinates falling in this range */
#define PointRange(X)  (((X) > -16384) && ((X) < 16383))

#define SHORTMASK 0xffff  /* Used by mult64 */

/* TRUE iff A and B have same signs. */
#define SAME_SIGNS(A, B) (((long)((unsigned long)A ^ (unsigned long)B)) >= 0)

/* Return the max value, storing the minimum value in min */
#define  maxmin(x1, x2, min) (x1 >= x2 ? (min = x2, x1) : (min = x1, x2))

/* *********************************************************************
   Below are two utility functions for implementing exact intersection
   calculation: SubsegIntersect and SideOfPoint.  SubsegIntersect uses
   SegIntersect to do the bulk of its work.
********************************************************************* */

/* *********************************************************************
   Compute the intersection points between two a priori line segments.
   Return 0 if no intersection, 1 if intersects in a point,
   and 2 if intersecting lines are colinear. Parameter values for
   intersection point are returned in ipt.

   Entry: s1, s2: the line segments
   Exit:  ipt: the intersection point.
********************************************************************* */
int SegIntersect(SEGMENT *s1, SEGMENT *s2,INTPOINT *ipt)
{
  long a, b, c, d, tdet, sdet, det;  /* parameter calculation variables */
  short max1, max2, min1, min2; /* bounding box check variables */
  ENDPOINT p1, p2, q1, q2; /* dereference a priori endpoints */

  p1 = s1->first;    p2 = s1->last;
  q1 = s2->first;    q2 = s2->last;

  /*  First make the bounding box test. */
  max1 = maxmin(p1.x, p2.x, min1);
  max2 = maxmin(q1.x, q2.x, min2);
  if((max1 < min2) || (min1 > max2)) return(0); /* no intersection */
  max1 = maxmin(p1.y, p2.y, min1);
  max2 = maxmin(q1.y, q2.y, min2);
  if((max1 < min2) || (min1 > max2)) return(0); /* no intersection */

  /* See if the endpoints of the second segment lie on the opposite
     sides of the first.  If not, return 0. */
  a = (long)(q1.x - p1.x) * (long)(p2.y - p1.y) -
      (long)(q1.y - p1.y) * (long)(p2.x - p1.x);
  b = (long)(q2.x - p1.x) * (long)(p2.y - p1.y) -
      (long)(q2.y - p1.y) * (long)(p2.x - p1.x);
  if(a!=0 && b!=0 && SAME_SIGNS(a, b)) return(0);

  /* See if the endpoints of the first segment lie on the opposite
     sides of the second.  If not, return 0.  */
  c = (long)(p1.x - q1.x) * (long)(q2.y - q1.y) -
      (long)(p1.y - q1.y) * (long)(q2.x - q1.x);
  d = (long)(p2.x - q1.x) * (long)(q2.y - q1.y) -
      (long)(p2.y - q1.y) * (long)(q2.x - q1.x);
  if(c!=0 && d!=0 && SAME_SIGNS(c, d) ) return(0);

  /* At this point each segment meets the line of the other. */
  det = a - b;
  if(det == 0) return(2); /* The segments are colinear.  Determining
     colinear intersection parameters would be tedious and not instructive. */

  /* The segments intersect since each segment crosses the other's line;
     however, since the lines are not parallel, either a or b is not
     zero.  Similarly either c or d is not zero. */
  tdet = -c;   sdet =   a;
  if(det < 0)  /* The denominator of the parameter must be positive. */
    {  det = -det;  sdet = -sdet;  tdet = -tdet;  }
  ipt->a = a; ipt->b = b;
  ipt->c = c; ipt->d = d;
  ipt->par[0].num = tdet;
  ipt->par[0].denom = det;
  ipt->par[1].num = sdet;
  ipt->par[1].denom = det;
  return(1);
}

/* *********************************************************************
   Returns the number of intersection points (0, 1 or 2) between line
   subsegments ss1 and ss2.  Note that 2 intersection points would
   represent the endpoints of overlap between two colinear line segments.
   If there is a single intersection point, it is returned in ipt.

   Entry: ss1, ss2: the line subsegments.
   Exit:  ipt: the intersection point (if exactly one).
********************************************************************* */
int SubsegIntersect(SUBSEGMENT *ss1, SUBSEGMENT *ss2, INTPOINT *ipt)
{
  int i;  /* Number of intersection points */

  /* intersect a priori segments */
  i = SegIntersect(&ss1->apriori, &ss2->apriori, ipt);
  if(i != 1) return(i);  /* either no intersection or colinear */
  /* one intersection point - check if it falls outside of subsegments */
  if(LessThan(&(ipt->par[0]), &(ss1->ParOne)) ||
     GreaterThan(&ipt->par[0], &(ss1->ParTwo)))
    return(0);  /* A priori segments intersect, but subsegments do not */
  if(LessThan(&ipt->par[1], &ss2->ParOne) ||
     GreaterThan(&ipt->par[1], &ss2->ParTwo))
    return(0);  /* A priori segments intersect, but subsegments do not */
  return(1);
}


/* *********************************************************************
   64 bit multiplication function.
   Multiply two long integers in1 and in2, returning the result in out.
********************************************************************* */
void mult64(long in1, long in2, unsigned long out[2])
{
  unsigned short *x, *y, *z;
  unsigned long temp;

  x = (unsigned short *) &in1;  y = (unsigned short *) &in2;
  z = (unsigned short *) out;
  temp = x[0] * y[0];
  z[1] = temp >> 16;   z[0] = temp & SHORTMASK;
  temp = x[0] * y[1];
  z[2] = temp >> 16;   z[1] += temp & SHORTMASK;
  z[2] += z[1] >>16;   z[1] = z[1] & SHORTMASK;
  temp = x[1] * y[0];
  z[2] += temp >> 16;  z[1] += temp & SHORTMASK;
  z[2] += z[1] >>16;  z[1] = z[1] & SHORTMASK;
  z[3] = z[2] >>16;  z[2] = z[2] & SHORTMASK;
  temp = x[1] * y[1];
  z[3] += temp >> 16;   z[2] += temp & SHORTMASK;
  z[3] += z[2] >>16;   z[2] = z[2] & SHORTMASK;
}


/* *********************************************************************
Comparison primitive to test par1 <= par2.
Return LEFT if par1 < par2; return ON if par1 = par2;
return RIGHT if par1 > par2.
********************************************************************* */
int CompPrim(PARAM *par1, PARAM *par2)
{
  unsigned long r1[2],r2[2];

  mult64(par1->num, par2->denom, r1);
  mult64(par2->num, par1->denom, r2);
  if(r1[1] != r2[1]) { if(r1[1] < r2[1]) return(LEFT); else return(RIGHT);}
  if(r1[0] != r2[0]) { if(r1[0] < r2[0]) return(LEFT); else return(RIGHT);}
  return(ON);
}


/* *********************************************************************
  Helper function for all parameter comparisons.
  Returns LEFT if par1 < par2, ON if par1 = par2, and RIGHT otherwise.
  Denominators must be positive.
********************************************************************* */
int CompHelp(PARAM *par1, PARAM *par2)
{
  PARAM tpar1, tpar2;

  tpar1 = *par1;   tpar2 = *par2;
  if(tpar1.num < 0)
     {
       if(tpar2.num >= 0) return(LEFT);
       tpar1.num = -tpar1.num;  tpar2.num = -tpar2.num;
       return(CompPrim(&tpar2, &tpar1));
     }
  if(tpar2.num < 0) return(RIGHT);
  return(CompPrim(&tpar1, &tpar2));
}


/* *********************************************************************
  Returns TRUE if par1 < par2 and FALSE otherwise;
********************************************************************* */
boolean LessThan(PARAM *par1, PARAM *par2)
{
  double x1, x2;

  x1 = ((double)par1->num) * par2->denom;
  x2 = ((double)par2->num) * par1->denom;
  if(x1 != x2)
    if(x1 < x2) return(TRUE);
    else return(FALSE);
  return(CompHelp(par1, par2) == LEFT);
}


/* *********************************************************************
  Returns TRUE if par1 > par2 and FALSE otherwise;
********************************************************************* */
boolean GreaterThan(PARAM *par1, PARAM *par2)
{
  double x1, x2;

  x1 = ((double)par1->num) * par2->denom;
  x2 = ((double)par2->num) * par1->denom;
  if(x1 != x2)
    if(x1 > x2) return(TRUE);
    else return(FALSE);
  return(CompHelp(par1, par2) == RIGHT);
}


/* *********************************************************************
  Returns TRUE if par1 = par2 and FALSE otherwise;
********************************************************************* */
boolean Equal(PARAM *par1, PARAM *par2)
{
  double x1, x2;

  x1 = ((double)par1->num) * par2->denom;
  x2 = ((double)par2->num) * par1->denom;
  if(x1 != x2) return(FALSE);
  return(CompHelp(par1, par2) == ON);
}


/* *********************************************************************
   Determine if a given point is to the left of, right of,
   or on segment s1.  The point is defined as the position (inpt)
   along a line segment (in).  Returns LEFT, RIGHT or ON respectively.

   Entry:
     in: line segment defining intersection point
     inpt: intersection point parameter along "in"
     s1: segment to check on which side of
********************************************************************* */
int SideOfPoint(SEGMENT *in, PARAM *inpar, SEGMENT *s1)
{
  PARAM par;  /* build a fraction for use in comparison against inpar */
  long delx, dely;  /* multiplications must be done as longs */
  short *x1, *x2, *y1, *y2, *x3, *x4, *y3, *y4; /* alias names */

  x1 = &s1->first.x; y1 = &s1->first.y; x2 = &s1->last.x; y2 = &s1->last.y;
  x3 = &in->first.x; y3 = &in->first.y; x4 = &in->last.x; y4 = &in->last.y;
  delx = *x2 - *x1;  dely = *y2 - *y1;
  par.denom =  (*y4 - *y3) * delx - (*x4 - *x3) * dely;
  par.num   =  (*x3 - *x1) * dely - (*y3 - *y1) * delx;
  if(par.denom > 0) return(CompHelp(&par, inpar));
  else if(par.denom < 0) /* switch signs and compare */
    { par.denom = -par.denom; par.num = -par.num;
      return(CompHelp(inpar, &par)); }
  else if(par.num > 0) return(RIGHT);
  else if(par.num < 0) return(LEFT);
  else return(ON);
}
