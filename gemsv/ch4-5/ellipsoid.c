/* ellipsoid.c */

#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include "ellipsoid.h"

#ifndef M_PI_2
#define	M_PI_2	1.57079632679489661923
#endif

typedef struct slot { float cos, sin; enum { None, Only, Done } flag; } slot;

static int n_max = 0;         /* current maximum degree of subdivision */
static slot *table = NULL;    /* an array of slots */
static vertex *octant = NULL; /* the base octant of the ellipsoid */

#define SetP(p,px,py,pz) (p).x=(px), (p).y=(py), (p).z=(pz)
#define SetV(v,px,py,pz,nx,ny,nz) SetP((v)->p,px,py,pz), SetP((v)->n,nx,ny,nz)
#define SetF(f,i0,i1,i2) (f)->v0 = i0, (f)->v1 = i1, (f)->v2 = i2

/*
// Compute the necessary cosine and sine values for generating ellipsoids
// with the degree of subdivision n, and initialize the array table[].
// The largest n becomes n_max, and calls with n <= n_max return immediately.
// The memory for the base octant is allocated to cope with any n <= n_max.
*/
void ellipsoid_init (int n)
{
   int n_table, i, j, k, l, m, h, d;
   slot *t0, *t1, *t2;
   float theta;

   if (n > n_max) {
      n_max = n;
      if (table) free (table);
      if ((n_table = ((n-1)*n)/2) == 0) table = NULL;
      else table = (slot *) malloc (n_table * sizeof(slot));
      if (octant) free (octant);
      octant = (vertex *) malloc (((n+1)*(n+2))/2 * sizeof(vertex));

      for (t0 = table, k = n_table; k > 0; k--, t0++) t0->flag = None;
      for (t0 = table, k = 0, l = 1, m = 3, i = 2; i <= n_max; i++) {
         l += m, m += 2, h = n_max / i - 1;
         for (t1 = t0+i - 2, j = 1; j < i; j++, k++, t0++, t1--) {
            if (t0->flag == None) {
               theta = (M_PI_2 * j) / i;
               t0->cos = t1->sin = cos (theta);
               t0->sin = t1->cos = sin (theta);
               t0->flag = t1->flag = Only;
            }
            if (t0->flag == Only) {
               t0->flag = Done;
               for (d = k+l, t2 = t0; h > 0; h--) {
                  t2 += d, d += l;
                  t2->cos = t0->cos;
                  t2->sin = t0->sin;
                  t2->flag = Done;
               }
            }
         }
      }
   }
}

/*
// Construct the base octant of the ellipsoid whose parameters are a, b, and c,
// with the degree of subdivision n using the cosine and sine values in table[].
// It is assumed that n <= n_max.
*/
static void ellipsoid_octant (int n, float a, float b, float c)
{
   int i, j;
   float a_1, b_1, c_1;
   float cos_ph, sin_ph, px, py, pz, nx, ny, nz, nznz, rnorm, tmp;
   vertex *o = octant;
   slot *table_th, *table_ph;

   a_1 = 1.0 / a; b_1 = 1.0 / b; c_1 = 1.0 / c;
   o = octant;
   table_th = table;
   table_ph = table + ((n-1)*(n-2))/2;

   SetV (o, 0.0, 0.0, c, 0.0, 0.0, 1.0), o++;         /* i = 0, j = 0 */
   for (i = 1; i < n; i++, table_ph++) {
      cos_ph = table_ph->cos;
      sin_ph = table_ph->sin;
      pz = cos_ph * c;
      nz = cos_ph * c_1;
      nznz = nz * nz;

      px = sin_ph * a;
      nx = sin_ph * a_1;
      rnorm = 1.0 / sqrt (nx*nx + nznz);              /* 0 < i < n, j = 0 */
      SetV (o, px, 0.0, pz, nx*rnorm, 0.0, nz*rnorm), o++;
      for (j = i; --j > 0; table_th++) {
         tmp = table_th->cos * sin_ph;
         px = tmp * a;
         nx = tmp * a_1;
         tmp = table_th->sin * sin_ph;
         py = tmp * b;
         ny = tmp * b_1;
         rnorm = 1.0 / sqrt (nx*nx + ny*ny + nznz);   /* 0 < i < n, 0 < j < i */
         SetV (o, px, py, pz, nx*rnorm, ny*rnorm, nz*rnorm), o++;
      }
      py = sin_ph * b;
      ny = sin_ph * b_1;
      rnorm = 1.0 / sqrt (ny*ny + nznz);              /* 0 < i < n, j = i */
      SetV (o, 0.0, py, pz, 0.0, ny*rnorm, nz*rnorm), o++;
   }
   SetV (o, a, 0.0, 0.0, 1.0, 0.0, 0.0), o++;         /* i = n, j = 0 */
   for (j = i; --j > 0; table_th++) {
      tmp = table_th->cos;
      px = tmp * a;
      nx = tmp * a_1;
      tmp = table_th->sin;
      py = tmp * b;
      ny = tmp * b_1;
      rnorm = 1.0 / sqrt (nx*nx + ny*ny);             /* i = n, 0 < j < i */
      SetV (o, px, py, 0.0, nx*rnorm, ny*rnorm, 0.0), o++;
   }
   SetV (o, 0.0, b, 0.0, 0.0, 1.0, 0.0);              /* i = n, j = i */
}

/*
// Note the following conventions in ellipsoid_seq() and ellipsoid_par():
// the north pole:        th =   0,      ph  =   0,
// the 1st octant:   0 <= th <  90,  0 < ph <=  90,
// the 2nd octant:  90 <= th < 180,  0 < ph <=  90,
// the 3rd octant: 180 <= th < 270,  0 < ph <=  90,
// the 4th octant: 270 <= th < 360,  0 < ph <=  90,
// the 5th octant:   0 <= th <  90, 90 < ph <= 180,
// the 6th octant:  90 <= th < 180, 90 < ph <= 180,
// the 7th octant: 180 <= th < 270, 90 < ph <= 180,
// the 8th octant: 270 <= th < 360, 90 < ph <= 180, and
// the south pole:        th =   0,      ph  = 180.
*/

/*
// Generate the vertices fot the ellipsoid with parameters a, b, and c
// with the degree of subdivision n, by reflecting the base octant.
// Also generate triangular faces of the ellipsoid with vertices ordered
// counterclockwise when viewed from the outside.
*/

/* sequential version */
void ellipsoid_seq (object *ellipsoid, int n, float a, float b, float c)
{
   vertex *v, *o;
   face *f;
   int i, j, ko, kv, kw, kv0, kw0;

   /* Check parameters for validity. */
   if (n <= 0 || n_max < n || a <= 0.0 || b <= 0.0 || c <= 0.0) {
      ellipsoid->nv = 0; ellipsoid->v = NULL;
      ellipsoid->nf = 0; ellipsoid->f = NULL;
      return;
   }

   /* Initialize the base octant. */
   ellipsoid_octant (n, a, b, c);

   /* Allocate memories for vertices and faces. */
   ellipsoid->nv = 4*n*n + 2;
   ellipsoid->nf = 8*n*n;
   ellipsoid->v = (vertex *) malloc (ellipsoid->nv * sizeof(vertex));
   ellipsoid->f = (face *) malloc (ellipsoid->nf * sizeof(face));

   /* Generate vertices of the ellipsoid from octant[]. */
   v = ellipsoid->v;
   o = octant;
#define op o->p
#define on o->n
   SetV (v,  op.x,  op.y,  op.z,  on.x,  on.y,  on.z), v++; /* the north pole */
   for (i = 0; ++i <= n;) {
      o += i;
      for (j = i; --j >= 0; o++, v++)                       /* 1st octant */
         SetV (v,  op.x,  op.y,  op.z,  on.x,  on.y,  on.z);
      for (j = i; --j >= 0; o--, v++)                       /* 2nd octant */
         SetV (v, -op.x,  op.y,  op.z, -on.x,  on.y,  on.z);
      for (j = i; --j >= 0; o++, v++)                       /* 3rd octant */
         SetV (v, -op.x, -op.y,  op.z, -on.x, -on.y,  on.z);
      for (j = i; --j >= 0; o--, v++)                       /* 4th octant */
         SetV (v,  op.x, -op.y,  op.z,  on.x, -on.y,  on.z);
   }
   for (; --i > 1;) {
      o -= i;
      for (j = i; --j > 0; o++, v++)                        /* 5th octant */
         SetV (v,  op.x,  op.y, -op.z,  on.x,  on.y, -on.z);
      for (j = i; --j > 0; o--, v++)                        /* 6th octant */
         SetV (v, -op.x,  op.y, -op.z, -on.x,  on.y, -on.z);
      for (j = i; --j > 0; o++, v++)                        /* 7th octant */
         SetV (v, -op.x, -op.y, -op.z, -on.x, -on.y, -on.z);
      for (j = i; --j > 0; o--, v++)                        /* 8th octant */
         SetV (v,  op.x, -op.y, -op.z,  on.x, -on.y, -on.z);
   }
   o--, SetV (v, -op.x, -op.y, -op.z, -on.x, -on.y, -on.z); /* the south pole */
#undef op
#undef on

   /* Generate triangular faces of the ellipsoid. */
   f = ellipsoid->f;
   kv = 0, kw = 1;
   for (i = 0; i < n; i++) {
      kv0 = kv, kw0 = kw;
      for (ko = 1; ko <= 3; ko++)                /* the 1st, 2nd, 3rd octants */
         for (j = i;; j--) {
            SetF (f, kv, kw, ++kw), f++;
            if (j == 0) break;
            SetF (f, kv, kw, ++kv), f++;
         }
      for (j = i;; j--) {                        /* the 4th octant */
         if (j == 0) { SetF (f, kv0, kw, kw0), kv++, kw++, f++; break; }
         SetF (f, kv, kw, ++kw), f++;
         if (j == 1) SetF (f, kv, kw, kv0), f++;
         else SetF (f, kv, kw, ++kv), f++;
      }
   }
   for (; --i >= 0;) {
      kv0 = kv, kw0 = kw;
      for (ko = 5; ko <= 7; ko++)                /* the 5th, 6th, 7th octants */
         for (j = i;; j--) {
            SetF (f, kv, kw, ++kv), f++;
            if (j == 0) break;
            SetF (f, kv, kw, ++kw), f++;
         }
      for (j = i;; j--) {                        /* the 8th octant */
         if (j == 0) { SetF (f, kv, kw0, kv0), kv++, kw++, f++; break; }
         SetF (f, kv, kw, ++kv), f++;
         if (j == 1) SetF (f, kv, kw, kw0), f++;
         else SetF (f, kv, kw, ++kw), f++;
      }
   }
}

/* parallel version */
void ellipsoid_par (object *ellipsoid, int n, float a, float b, float c)
{
   int nv, nf;
   vertex *ev;
   face *ef;

   /* Check parameters for validity. */
   if (n <= 0 || n_max < n || a <= 0.0 || b <= 0.0 || c <= 0.0) {
      ellipsoid->nv = 0; ellipsoid->v = NULL;
      ellipsoid->nf = 0; ellipsoid->f = NULL;
      return;
   }

   /* Initialize the base octant. */
   ellipsoid_octant (n, a, b, c);

   /* Allocate memories for vertices and faces. */
   nv = ellipsoid->nv = 4*n*n + 2;
   nf = ellipsoid->nf = 8*n*n;
   ev = ellipsoid->v = (vertex *) malloc (nv * sizeof(vertex));
   ef = ellipsoid->f = (face *) malloc (nf * sizeof(face));

   ellipsoid_par_help (n, nv, nf, ev, ef, octant);
}

/* A utility function that simply outputs the ellipsoid data. */
void ellipsoid_print (object *ellipsoid)
{
   int i;
   vertex *v;
   face *f;

   for (v = ellipsoid->v, i = 0; i < ellipsoid->nv; i++, v++)
      printf ("v %g %g %g %g %g %g\n",
         v->p.x, v->p.y, v->p.z, v->n.x, v->n.y, v->n.z);

   for (f = ellipsoid->f, i = 0; i < ellipsoid->nf; i++, f++)
      printf ("f %d %d %d\n", f->v0, f->v1, f->v2);
}

/* undo of ellipsoid_init () */
void ellipsoid_done (void)
{
   n_max = 0;
   if (table) free (table), table = NULL;
   if (octant) free (octant), octant = NULL;
}

/* undo of ellipsoid_seq () */
void ellipsoid_free (object *ellipsoid)
{
   free (ellipsoid->v), ellipsoid->nv = 0, ellipsoid->v = NULL;
   free (ellipsoid->f), ellipsoid->nf = 0, ellipsoid->f = NULL;
}
