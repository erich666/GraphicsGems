/****** lincrv.c ******/
/* Ken Shoemake, 1994 */

#include "lincrv.h"

/* Perform a generic vector unary operation. */
#define V_Op(vdst,gets,op,vsrc,n) {register int V_i;\
    for(V_i=(n)-1;V_i>=0;V_i--) (vdst)[V_i] gets op ((vsrc)[V_i]);}

static void lerp(Knot t, Knot a0, Knot a1, Vect p0, Vect p1, int m, Vect p)
{
    register Knot t0=(a1-t)/(a1-a0), t1=1-t0;
    register int i;
    for (i=m-1; i>=0; i--) p[i] = t0*p0[i] + t1*p1[i];
}

/* DialASpline(t,a,p,m,n,work,Cn,interp,val) computes a point val at parameter
    t on a spline with knot values a and control points p. The curve will have
    Cn continuity, and if interp is TRUE it will interpolate the control points.
    Possibilities include Langrange interpolants, Bezier curves, Catmull-Rom
    interpolating splines, and B-spline curves. Points have m coordinates, and
    n+1 of them are provided. The work array must have room for n+1 points.
 */
int DialASpline(Knot t, Knot a[], Vect p[], int m, int n, Vect work[],
                    unsigned int Cn, Bool interp, Vect val)
{
    register int i, j, k, h, lo, hi;

    if (Cn>n-1) Cn = n-1;       /* Anything greater gives one polynomial */
    for (k=0; t> a[k]; k++);    /* Find enclosing knot interval */
    for (h=k; t==a[k]; k++);    /* May want to use fewer legs */
    if (k>n) {k = n; if (h>k) h = k;}
    h = 1+Cn - (k-h); k--;
    lo = k-Cn; hi = k+1+Cn;

    if (interp) {               /* Lagrange interpolation steps */
        int drop=0;
        if (lo<0) {lo = 0; drop += Cn-k;
                   if (hi-lo<Cn) {drop += Cn-hi; hi = Cn;}}
        if (hi>n) {hi = n; drop += k+1+Cn-n;
                   if (hi-lo<Cn) {drop += lo-(n-Cn); lo = n-Cn;}}
        for (i=lo; i<=hi; i++) V_Op(work[i],=,,p[i],m);
        for (j=1; j<=Cn; j++) {
            for (i=lo; i<=hi-j; i++) {
                lerp(t,a[i],a[i+j],work[i],work[i+1],m,work[i]);
            }
        }
        h = 1+Cn-drop;
    } else {                    /* Prepare for B-spline steps */
        if (lo<0) {h += lo; lo = 0;}
        for (i=lo; i<=lo+h; i++) V_Op(work[i],=,,p[i],m);
        if (h<0) h = 0;
    }
    for (j=0; j<h; j++) {
        int tmp = 1+Cn-j;
        for (i=h-1; i>=j; i--) {
            lerp(t,a[lo+i],a[lo+i+tmp],work[lo+i],work[lo+i+1],m,work[lo+i+1]);
        }
    }
    V_Op(val,=,,work[lo+h],m);
    return (k);
}
