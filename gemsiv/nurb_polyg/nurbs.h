/*
 * Nurbs.h Nurb surface processing code.
 *
 * John Peterson
 */

#include "GraphicsGems.h"

#ifndef THINK_C
typedef unsigned char Boolean;
#endif

/* Rational (homogeneous) point */

typedef struct Point4Struct {
    double x, y, z, w;
    } Point4;
typedef Point4 Vector4;

/*
 * Sampled point on a surface.	This contains the point, normal and
 * surface coordinates (u,v).  This structure is passed to the rendering
 * code for shading, etc.
 */
typedef struct SurfSample {
    Point3 point, normal;   /* Point on surface, normal at that point */
    double normLen;	    /* Used for normalizing normals */
    double u, v;	    /* Parameters, e.g., used for texture mapping. */
    /* Note the parameter's range is determined by the surface's knot vector,
     * i.e., u goes from kvU[orderU-1] to kvU[numU], and likewise for v */
} SurfSample;

#define MAXORDER 20	    /* Maximum order allowed (for local array sizes) */

typedef struct NurbSurface {
    /* Number of Points in the U and V directions, respectivly */
    long numU, numV;
    /* Order of the surface in U and V (must be >= 2, < MAXORDER) */
    long orderU, orderV;
    /* Knot vectors, indexed as [0..numU+orderU-1] and [0..numV+orderV-1] */
    double * kvU, * kvV;
    /* Control points, indexed as points[0..numV-1][0..numU-1] */
    /* Note the w values are *premultiplied* with the x, y and z values */
    Point4 ** points;

    /* These fields are added to support subdivision */
    Boolean strV0, strVn,   /* Edge straightness flags for subdivision */
	    strU0, strUn;
    Boolean flatV, flatU;   /* Surface flatness flags for subdivision */
    SurfSample c00, c0n,
	       cn0, cnn;    /* Corner data structures for subdivision */
} NurbSurface;

extern double SubdivTolerance;	/* Screen space tolerance for subdivision */

#define CHECK( n ) \
    { if (!(n)) { fprintf( stderr, "Ran out of memory\n" ); exit(-1); } }

#define DIVW( rpt, pt ) \
    { (pt)->x = (rpt)->x / (rpt)->w; \
      (pt)->y = (rpt)->y / (rpt)->w; \
      (pt)->z = (rpt)->z / (rpt)->w; }

/* Function prototypes */

extern void DrawSubdivision( NurbSurface * );
extern void DrawEvaluation( NurbSurface * );

extern long FindBreakPoint( double u, double * kv, long m, long k );
extern void AllocNurb( NurbSurface *, double *, double * );
extern void CloneNurb( NurbSurface *, NurbSurface * );
extern void FreeNurb( NurbSurface * );
extern void RefineSurface( NurbSurface *, NurbSurface *, Boolean );

extern void CalcPoint( double, double, NurbSurface *, Point3 *, Point3 *, Point3 * );
