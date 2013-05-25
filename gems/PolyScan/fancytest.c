/*
 * fancytest.c: subroutine illustrating the use of poly_clip and poly_scan
 * for Phong-shading and texture mapping.
 *
 * Note: lines enclosed in angle brackets '<', '>' should be replaced
 * with the code described.
 * Makes calls to hypothetical packages "shade", "image", "texture", "zbuffer".
 *
 * Paul Heckbert	Dec 1989
 */

#include <stdio.h>
#include <math.h>
#include "poly.h"

#define XMAX 1280	/* hypothetical image width */
#define YMAX 1024	/* hypothetical image height */
#define LIGHT_INTEN 255		/* light source intensity */

void pixelproc();

fancytest()
{
    int i;
    double WS[4][4];	/* world space to screen space transform */
    Poly p;		/* a polygon */
    Poly_vert *v;

    static Poly_box box = {0, XMAX, 0, YMAX, -32678, 32767.99};
	/* 3-D screen clipping box, continuous coordinates */

    static Window win = {0, 0, XMAX-1, YMAX-1};
	/* 2-D screen clipping window, discrete coordinates */

    <initialize world space position (x,y,z), normal (nx,ny,nz), and texture
     position (u,v) at each vertex of p; set p.n>
    <set WS to world-to-screen transform>

    /* transform vertices from world space to homogeneous screen space */
    for (i=0; i<p.n; i++) {
	v = &p.vert[i];
	mx4_transform(v->x, v->y, v->z, 1., WS, &v->sx, &v->sy, &v->sz, &v->sw);
    }

    /* interpolate sx, sy, sz, sw, nx, ny, nz, u, v in poly_clip */
    p.mask = POLY_MASK(sx) | POLY_MASK(sy) | POLY_MASK(sz) | POLY_MASK(sw) |
	POLY_MASK(nx) | POLY_MASK(ny) | POLY_MASK(nz) |
	POLY_MASK(u) | POLY_MASK(v);

    poly_print("before clipping", &p);
    if (poly_clip_to_box(&p, &box) == POLY_CLIP_OUT)	/* clip polygon */
	return;						/* quit if off-screen */

    /* do homogeneous division of screen position and texture position */
    for (i=0; i<p.n; i++) {
	v = &p.vert[i];
	v->sx /= v->sw;
	v->sy /= v->sw;
	v->sz /= v->sw;
	v->u /= v->sw;
	v->v /= v->sw;
	v->q = 1./v->sw;
    }
    /*
     * previously we ignored q (assumed q=1), henceforth ignore sw (assume sw=1)
     * Interpolate sx, sy, sz, nx, ny, nz, u, v, q in poly_scan
     */
    p.mask &= ~POLY_MASK(sw);
    p.mask |= POLY_MASK(q);

    poly_print("scan converting the polygon", &p);
    poly_scan(&p, &win, pixelproc);	/* scan convert! */
}

static void pixelproc(x, y, pt)		/* called at each pixel by poly_scan */
int x, y;
Poly_vert *pt;
{
    int sz, u, v, inten;
    double len, nor[3], diff, spec;

    sz = pt->sz;
    if (sz < zbuffer_read(x, y)) {
	len = sqrt(pt->nx*pt->nx + pt->ny*pt->ny + pt->nz*pt->nz);
	nor[0] = pt->nx/len;		/* unitize the normal vector */
	nor[1] = pt->ny/len;
	nor[2] = pt->nz/len;
	shade(nor, &diff, &spec);	/* compute specular and diffuse coeffs*/
	u = pt->u/pt->q;		/* do homogeneous div. of texture pos */
	v = pt->v/pt->q;
	inten = texture_read(u, v)*diff + LIGHT_INTEN*spec;
	image_write(x, y, inten);
	zbuffer_write(x, y, sz);
    }
}

/* mx4_transform: transform 4-vector p by matrix m yielding q: q = p*m */

mx4_transform(px, py, pz, pw, m, qxp, qyp, qzp, qwp)
double px, py, pz, pw, m[4][4], *qxp, *qyp, *qzp, *qwp;
{
    *qxp = px*m[0][0] + py*m[1][0] + pz*m[2][0] + pw*m[3][0];
    *qyp = px*m[0][1] + py*m[1][1] + pz*m[2][1] + pw*m[3][1];
    *qzp = px*m[0][2] + py*m[1][2] + pz*m[2][2] + pw*m[3][2];
    *qwp = px*m[0][3] + py*m[1][3] + pz*m[2][3] + pw*m[3][3];
}
