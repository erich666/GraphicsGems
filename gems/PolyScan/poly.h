/* poly.h: definitions for polygon package */

#ifndef POLY_HDR
#define POLY_HDR

#define POLY_NMAX 10		/* max #sides to a polygon; change if needed */
/* note that poly_clip, given an n-gon as input, might output an (n+6)gon */
/* POLY_NMAX=10 is thus appropriate if input polygons are triangles or quads */

typedef struct {		/* A POLYGON VERTEX */
    double sx, sy, sz, sw;	/* screen space position (sometimes homo.) */
    double x, y, z;		/* world space position */
    double u, v, q;		/* texture position (sometimes homogeneous) */
    double r, g, b;		/* (red,green,blue) color */
    double nx, ny, nz;		/* world space normal vector */
} Poly_vert;
/* update poly.c if you change this structure */
/* note: don't put > 32 doubles in Poly_vert struct, or mask will overflow */

typedef struct {		/* A POLYGON */
    int n;			/* number of sides */
    unsigned long mask;		/* interpolation mask for vertex elems */
    Poly_vert vert[POLY_NMAX];	/* vertices */
} Poly;
/*
 * mask is an interpolation mask whose kth bit indicates whether the kth
 * double in a Poly_vert is relevant.
 * For example, if the valid attributes are sx, sy, and sz, then set
 *	mask = POLY_MASK(sx) | POLY_MASK(sy) | POLY_MASK(sz);
 */

typedef struct {		/* A BOX (TYPICALLY IN SCREEN SPACE) */
    double x0, x1;		/* left and right */
    double y0, y1;		/* top and bottom */
    double z0, z1;		/* near and far */
} Poly_box;

typedef struct {		/* WINDOW: A DISCRETE 2-D RECTANGLE */
    int x0, y0;			/* xmin and ymin */
    int x1, y1;			/* xmax and ymax (inclusive) */
} Window;

#define POLY_MASK(elem) (1 << (&poly_dummy->elem - (double *)poly_dummy))

#define POLY_CLIP_OUT 0		/* polygon entirely outside box */
#define POLY_CLIP_PARTIAL 1	/* polygon partially inside */
#define POLY_CLIP_IN 2		/* polygon entirely inside box */

extern Poly_vert *poly_dummy;	/* used superficially by POLY_MASK macro */

void	poly_print(/* str, p */);
void	poly_vert_label(/* str, mask */);
void	poly_vert_print(/* str, v, mask */);
int	poly_clip_to_box(/* p1, box */);
void	poly_clip_to_halfspace(/* p, q, index, sign, k, name */);
void	poly_scan(/* p, win, pixelproc */);

#endif
