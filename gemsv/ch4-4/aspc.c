/* aspc.c -- generic adaptive sampling of parametric curves */
#include <stdlib.h>
#include <time.h>

typedef struct point { double t,x,y,z; } Point;

#define T(p)    ((p)->t)
#define X(p)    ((p)->x)
#define Y(p)    ((p)->y)
#define Z(p)    ((p)->z)

extern void gamma(Point* p);            /* user supplied */
extern void line(Point* p, Point* q);   /* user supplied */

static int flat(Point* p, Point* q, Point* r);

static void sample(Point* p, Point* q)
{
 Point rr, *r=&rr;
 double t = 0.45 + 0.1 * (rand()/(double) RAND_MAX);
 T(r) = T(p) + t*(T(q)-T(p));
 gamma(r);
 if (flat(p,q,r)) line(p,q); else { sample(p,r); sample(r,q); }
}

static int flat(Point* p, Point* q, Point* r)
{
 extern double tol;                     /* user supplied */
 double xp = X(p)-X(r); double yp = Y(p)-Y(r); double zp = Z(p)-Z(r);
 double xq = X(q)-X(r); double yq = Y(q)-Y(r); double zq = Z(q)-Z(r);
 double x =  yp*zq-yq*zp;
 double y =  xp*zq-xq*zp;
 double z =  xp*yq-xq*yp;
 return (x*x+y*y+z*z) < tol;            /* |pr x qr|^2 < tol */
}

void aspc(double a, double b)           /* entry point */
{
 Point pp, *p = &pp;
 Point qq, *q = &qq;
 srand(time(0));                        /* randomize */
 T(p)= a; gamma(p); T(q)=b; gamma(q);   /* set up */
 sample(p,q);                           /* sample */
}
