/* ellipsoid_par_help.c */

#include "ellipsoid.h"

#define SetP(p,px,py,pz) (p).x=(px), (p).y=(py), (p).z=(pz)
#define SetV(v,px,py,pz,nx,ny,nz) SetP((v)->p,px,py,pz), SetP((v)->n,nx,ny,nz)
#define SetF(f,i0,i1,i2) (f)->v0 = i0, (f)->v1 = i1, (f)->v2 = i2

void ellipsoid_par_help (int n, int nv, int nf, vertex *ev, face *ef, vertex *octant)
{
   int i, j, vv, ww, vv0, ww0;
   vertex *v, *o;
   face *f;

   /* Generate vertices and triangular faces of the ellipsoid.   */
   /* Note that each of the 18 statements below (par_0 ~ par_17) */
   /*    are independent of each other.                          */

#pragma parallel
#pragma local(i,j,v,o,f,vv,ww,vv0,ww0)
#pragma byvalue(n,nv,nf,ev,ef,octant)
/** #pragma distinct(*v,*ev) **/
/** #pragma distinct(*o,*octant) **/
/** #pragma distinct(*f,*ef) **/
/* parallel */ {
#pragma independent
   /* par_0 */ { /* the north pole */
      v = ev, o = octant;
            SetV (v,  o->p.x,  o->p.y,  o->p.z,  o->n.x,  o->n.y,  o->n.z);
      ;
   }
#pragma independent
   /* par_1 */ { /* vertices on the 1st octant */
      v = ev+1, o = octant+1;
      for (i = 1;;) {
         for (j = i; --j >= 0; o++, v++)
            SetV (v,  o->p.x,  o->p.y,  o->p.z,  o->n.x,  o->n.y,  o->n.z);
         if (i == n) break;
         v += 3*i, i++, o++;
      }
      ; /* This empty statement is necessary because of the compiler-bug(?). */
   }
#pragma independent
   /* par_2 */ { /* vertices on the 2nd octant */
      v = ev+2, o = octant+2;
      for (i = 1;;) {
         for (j = i; --j >= 0; o--, v++)
            SetV (v, -o->p.x,  o->p.y,  o->p.z, -o->n.x,  o->n.y,  o->n.z);
         if (i == n) break;
         v += 3*i+1, i++, o += 2*i;
      }
      ;
   }
#pragma independent
   /* par_3 */ { /* vertices on the 3rd octant */
      v = ev+3, o = octant+1;
      for (i = 1;;) {
         for (j = i; --j >= 0; o++, v++)
            SetV (v, -o->p.x, -o->p.y,  o->p.z, -o->n.x, -o->n.y,  o->n.z);
         if (i == n) break;
         v += 3*i+2, i++, o++;
      }
      ;
   }
#pragma independent
   /* par_4 */ { /* vertices on the 4th octant */
      v = ev+4, o = octant+2;
      for (i = 1;;) {
         for (j = i; --j >= 0; o--, v++)
            SetV (v,  o->p.x, -o->p.y,  o->p.z,  o->n.x, -o->n.y,  o->n.z);
         if (i == n) break;
         i++, v += 3*i, o += 2*i;
      }
      ;
   }
#pragma independent
   /* par_5 */ { /* vertices on the 5th octant */
      v = ev+nv-5, o = octant+1;
      for (i = 1;;) {
         for (j = i; --j >= 0; o++, v++)
            SetV (v,  o->p.x,  o->p.y, -o->p.z,  o->n.x,  o->n.y, -o->n.z);
         if (i == n) break;
         v -= 5*i+4, i++, o++;
      }
      ;
   }
#pragma independent
   /* par_6 */ { /* vertices on the 6th octant */
      v = ev+nv-4, o = octant+2;
      for (i = 1;;) {
         for (j = i; --j >= 0; o--, v++)
            SetV (v, -o->p.x,  o->p.y, -o->p.z, -o->n.x,  o->n.y, -o->n.z);
         if (i == n) break;
         v -= 5*i+3, i++, o += 2*i;
      }
      ;
   }
#pragma independent
   /* par_7 */ { /* vertices on the 7th octant */
      v = ev+nv-3, o = octant+1;
      for (i = 1;;) {
         for (j = i; --j >= 0; o++, v++)
            SetV (v, -o->p.x, -o->p.y, -o->p.z, -o->n.x, -o->n.y, -o->n.z);
         if (i == n) break;
         v -= 5*i+2, i++, o++;
      }
      ;
   }
#pragma independent
   /* par_8 */ { /* vertices on the 8th octant */
      v = ev+nv-2, o = octant+2;
      for (i = 1;;) {
         for (j = i; --j >= 0; o--, v++)
            SetV (v,  o->p.x, -o->p.y, -o->p.z,  o->n.x, -o->n.y, -o->n.z);
         if (i == n) break;
         v -= 5*i+1, i++, o += 2*i;
      }
      ;
   }
#pragma independent
   /* par_9 */ { /* the south pole */
      v = ev+nv-1, o = octant;
            SetV (v, -o->p.x, -o->p.y, -o->p.z, -o->n.x, -o->n.y, -o->n.z);
      ;
   }
#pragma independent
   /* par_10 */ { /* faces on the 1st octant */
      f = ef; vv = 0, ww = 1;
      for (i = 1;;) {
         for (j = i;;) {
            SetF (f, vv, ww, ++ww), f++;
            if (--j == 0) break;
            SetF (f, vv, ww, ++vv), f++;
         }
         if (i == n) break;
         f += 6*i-3; vv = ww-i; ww += 3*i; i++;
      }
      ;
   }
#pragma independent
   /* par_11 */ { /* faces on the 2nd octant */
      f = ef+1; vv = 0, ww = 2;
      for (i = 1;;) {
         for (j = i;;) {
            SetF (f, vv, ww, ++ww), f++;
            if (--j == 0) break;
            SetF (f, vv, ww, ++vv), f++;
         }
         if (i == n) break;
         f += 6*i-1; vv = ww-i; ww += 3*i+1; i++;
      }
      ;
   }
#pragma independent
   /* par_12 */ { /* faces on the 3rd octant */
      f = ef+2; vv = 0, ww = 3;
      for (i = 1;;) {
         for (j = i;;) {
            SetF (f, vv, ww, ++ww), f++;
            if (--j == 0) break;
            SetF (f, vv, ww, ++vv), f++;
         }
         if (i == n) break;
         f += 6*i+1; vv = ww-i; ww += 3*i+2; i++;
      }
      ;
   }
#pragma independent
   /* par_13 */ { /* faces on the 4th octant */
      f = ef+3; vv = 0, ww = 4, vv0 = 0, ww0 = 1;
      for (i = 1;;) {
         for (j = i;;) {
            if (--j == 0) { SetF (f, vv0, ww, ww0), vv++, ww++, f++; break; }
            SetF (f, vv, ww, ++ww), f++;
            if (j == 1) SetF (f, vv, ww, vv0), f++;
            else SetF (f, vv, ww, ++vv), f++;
         }
         if (i == n) break;
         vv0 = ww0; ww0 += 4*i;
         f += 6*i+3; vv = ww-i; i++; ww += 3*i;
      }
      ;
   }
#pragma independent
   /* par_14 */ { /* faces on the 5th octant */
      f = ef+nf-4; vv = nv-5, ww = nv-1;
      for (i = 1;;) {
         for (j = i;;) {
            SetF (f, vv, ww, ++vv), f++;
            if (--j == 0) break;
            SetF (f, vv, ww, ++ww), f++;
         }
         if (i == n) break;
         f -= 10*i+3; ww = vv-i; i++; vv -= 5*i-1;
      }
      ;
   }
#pragma independent
   /* par_15 */ { /* faces on the 6th octant */
      f = ef+nf-3; vv = nv-4, ww = nv-1;
      for (i = 1;;) {
         for (j = i;;) {
            SetF (f, vv, ww, ++vv), f++;
            if (--j == 0) break;
            SetF (f, vv, ww, ++ww), f++;
         }
         if (i == n) break;
         f -= 10*i+1; ww = vv-i; i++; vv -= 5*i-2;
      }
      ;
   }
#pragma independent
   /* par_16 */ { /* faces on the 7th octant */
      f = ef+nf-2; vv = nv-3, ww = nv-1;
      for (i = 1;;) {
         for (j = i;;) {
            SetF (f, vv, ww, ++vv), f++;
            if (--j == 0) break;
            SetF (f, vv, ww, ++ww), f++;
         }
         if (i == n) break;
         f -= 10*i-1; ww = vv-i; i++; vv -= 5*i-3;
      }
      ;
   }
#pragma independent
   /* par_17 */ { /* faces on the 8th octant */
      f = ef+nf-1; vv = nv-2, ww = nv-1, vv0 = nv-5, ww0 = nv-1;
      for (i = 1;;) {
         for (j = i;;) {
            if (--j == 0) { SetF (f, vv, ww0, vv0), vv++, ww++, f++; break; }
            SetF (f, vv, ww, ++vv), f++;
            if (j == 1) SetF (f, vv, ww, ww0), f++;
            else SetF (f, vv, ww, ++ww), f++;
         }
         if (i == n) break;
         f -= 10*i-3; ww = vv-i; i++; vv -= 5*i-4;
         ww0 = vv0; vv0 -= 4*i;
      }
      ;
   }
/* parallel */ }
}
