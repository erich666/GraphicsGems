/* Quick and Simple Bezier Curve Drawing --- Robert D. Miller
 * This 2-D planar Bezier curve drawing software is 3-D compliant ---
 * redefine Point and change the commented lines as indicated.
 */

#include <stdio.h>
#define MaxCtlPoints  12

typedef struct {float x; float y;} Point;       /* for 2-D curves */
                                                /* for 3-D space curves */
/* typedef struct {float x; float y; float z;} Point; */
typedef Point PtArray[99];
typedef Point BezArray[MaxCtlPoints];

void BezierForm(int NumCtlPoints, PtArray p, BezArray c)
/*   Setup Bezier coefficient array once for each control polygon. */
{
    int k; long n, choose;
    n= NumCtlPoints -1;
    for(k = 0; k <= n; k++) {
        if (k == 0) choose = 1;
        else if (k == 1) choose = n;
        else choose = choose *(n-k+1)/k;
        c[k].x = p[k].x *choose;
        c[k].y = p[k].y *choose;
     /* c[k].z = p[k].z *choose; */   /* use for 3-D curves */
        };
}

void BezierCurve(int NumCtlPoints, BezArray c, Point *pt, float t)
/*  Return Point pt(t), t <= 0 <= 1 from C, given the number
    of Points in control polygon. BezierForm must be called
    once for any given control polygon. */
{   int k, n;
    float t1, tt, u;
    BezArray b;

    n = NumCtlPoints -1;  u =t;
    b[0].x = c[0].x;
    b[0].y = c[0].y;
 /* b[0].z = c[0].z; */      /* for 3-D curves */
    for(k =1; k <=n; k++) {
        b[k].x = c[k].x *u;
        b[k].y = c[k].y *u;
        /* b[k].z = c[k].z *u   /* for 3-D curves */
        u =u*t;
        };

     (*pt).x = b[n].x;  (*pt).y = b[n].y;
     t1 = 1-t;          tt = t1;
     for(k =n-1; k >=0; k--) {
         (*pt).x += b[k].x *tt;
         (*pt).y += b[k].y *tt;
      /* (*pt).z += b[k].z *tt;  */   /* Again, 3-D */
         tt =tt*t1;
         }
}

float u;
int   k;
PtArray pn;
BezArray bc;
Point pt;
void main ()
{
    pn[0].x = 100;  pn[0].y = 20;
    pn[1].x = 120;  pn[1].y = 40;
    pn[2].x = 140;  pn[2].y = 25;
    pn[3].x = 160;  pn[3].y = 20;
    BezierForm(4, pn, bc);

    for(k =0; k <=10; k++) {
        BezierCurve(4, bc, &pt, (float)k/10.0);
        printf("%3d  %8.4f  %8.4f\n",k, pt.x, pt.y);
     /* draw curve  */
     /* if (k == 0) MoveTo(pt.x, pt.y);
        else LineTo(pt.x, pt.y);  */
        }
}
