/*
Joining Two Lines with a Circular Arc Fillet
Robert D. Miller
*/














#include "math.h"
#include "stdio.h"
#include "stdlib.h"

#define pi        3.14159265358974
#define halfpi    1.5707963267949
#define threepi2  4.71238898038469
#define twopi     6.28318530717959
#define degree    57.29577951308232088
#define radian    0.01745329251994329577

typedef struct point { float x; float y;} point;

point   gv1, gv2;
float   xx, yy, sa, sb,
        qx, qy, rx, ry;
int     x1, yy1, x2, y2;

float   arccos();
float   arcsin();
float   cross2();
float   dot2();

void    moveto();   /* External draw routine */
void    lineto();   /* External draw routine */
void    drawarc();
float   linetopoint();
void    pointperp();
void    fillet();

float cross2(v1,v2)
      point v1, v2;
{ return (v1.x*v2.y - v2.x*v1.y); }

float dot2(v1,v2)   /* Return angle subtended by two vectors.  */
      point v1, v2;
{ float   d, t;

      d= (float) sqrt(((v1.x*v1.x)+(v1.y*v1.y)) * ((v2.x*v2.x)+(v2.y*v2.y)));
      if (d != (float) 0.0)
      {
         t= (v1.x*v2.x+v1.y*v2.y)/d;
         return (arccos(t));
      }
      else
      return ((float) 0.0);
}


/* Draw circular arc in one degree increments. Center is (xc,yc)
        with radius r, beginning at starting angle, startang
        through angle ang. If ang < 0 arc is draw clockwise. */

void drawarc(xc, yc, r, startang, ang)
float xc, yc, r, startang, ang;

{
#define   sindt 0.017452406
#define   cosdt 0.999847695

float   a, x, y, sr;
int     k;

        a= (float) startang*radian;
        x= (float) r*cos(a);
        y= (float) r*sin(a);
        moveto(xc+x, yc+y, 3);
        if (ang >= (float) 0.0)
           sr= (float) sindt;
        else
           sr= (float) -sindt;
        for (k= 1; k <= (int) floor(fabs(ang)); k++)
          {  x= x*cosdt-y*sr;
             y= x*sr+y*cosdt;
             lineto(xc+x,yc+y);
          }
return;
}

/*        Find a,b,c in Ax + By + C = 0  for line p1,p2.         */

void linecoef(a,b,c,p1,p2)
float   *a, *b, *c;
point   p1, p2;

{
        *c=  (p2.x*p1.y)-(p1.x*p2.y);
        *a= p2.y-p1.y;
        *b= p1.x-p2.x;
return;
}

/*       Return signed distance from line Ax + By + C = 0 to point P. */

float linetopoint(a,b,c,p)
float   a, b, c;
point   p;

{
float   d, lp;

        d= sqrt((a*a)+(b*b));
        if (d == 0.0)
           lp =  0.0;
        else
           lp= (a*p.x+b*p.y+c)/d;
return ((float) lp);
}

/*   Given line l = ax + by + c = 0 and point p,
     compute x,y so p(x,y) is perpendicular to l. */

void pointperp(x, y, a, b, c, p)
float     *x, *y, a, b, c;
point     p;

{
float    d, cp;

        *x=  0.0;
        *y=  0.0;
        d=  a*a +b*b;
        cp= a*p.y-b*p.x;
        if (d != 0.0)
          {
             *x= (-a*c-b*cp)/d;
             *y= (a*cp-b*c)/d;
          }
return;
}

/*      Compute a circular arc fillet between lines L1 (p1 to p2) and
	L2 (p3 to p4) with radius R.  The circle center is xc,yc.      */

void fillet(p1, p2, p3, p4, r, xc, yc, pa, aa)
point   *p1, *p2, *p3, *p4;
float   r, *xc, *yc, *pa, *aa;

{
float a1, b1, c1, a2, b2, c2, c1p,
      c2p, d1, d2, xa, xb, ya, yb, d, rr;
point mp, pc;

        linecoef(&a1,&b1,&c1,*p1,*p2);
        linecoef(&a2,&b2,&c2,*p3,*p4);

        if ((a1*b2) == (a2*b1))  /* Parallel or coincident lines */
            goto xit;

        mp.x= ((*p3).x + (*p4).x)/2.0;
        mp.y= ((*p3).y + (*p4).y)/2.0;
        d1= linetopoint(a1,b1,c1,mp);  /* Find distance p1p2 to p3 */
        if (d1 == 0.0) goto xit;

        mp.x= ((*p1).x + (*p2).x)/2.0;
        mp.y= ((*p1).y + (*p2).y)/2.0;
        d2= linetopoint(a2,b2,c2,mp);  /* Find distance p3p4 to p2 */
        if (d2 == 0.0) goto xit;

        rr= r;
        if (d1 <= 0.0) rr= -rr;

        c1p= c1-rr*sqrt((a1*a1)+(b1*b1));  /* Line parallel l1 at d */
        rr= r;

        if (d2 <= 0.0) rr= -rr;

        c2p= c2-rr*sqrt((a2*a2)+(b2*b2));  /* Line parallel l2 at d */
        d= a1*b2-a2*b1;

        *xc= (c2p*b1-c1p*b2)/d;            /* Intersect constructed lines */
        *yc= (c1p*a2-c2p*a1)/d;            /* to find center of arc */
        pc.x=  *xc;
        pc.y=  *yc;

        pointperp(&xa,&ya,a1,b1,c1,pc);      /* Clip or extend lines as required */
        pointperp(&xb,&yb,a2,b2,c2,pc);
        (*p2).x= xa; (*p2).y= ya;
        (*p3).x= xb; (*p3).y= yb;
        gv1.x= xa-*xc; gv1.y= ya-*yc;
        gv2.x= xb-*xc; gv2.y= yb-*yc;

        *pa= (float) atan2(gv1.y,gv1.x);     /* Beginning angle for arc */
        *aa= dot2(gv1,gv2);
        if (cross2(gv1,gv2) < 0.0) *aa= -*aa; /* Angle subtended by arc */

xit:
return;
}
