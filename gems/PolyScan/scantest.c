/*
 * scantest.c: use poly_scan() for Gouraud shading and z-buffer demo.
 * Given the screen space X, Y, and Z of N-gon on command line,
 * print out all pixels during scan conversion.
 * This code could easily be modified to actually read and write pixels.
 *
 * Paul Heckbert	Dec 1989
 */

#include <stdio.h>
#include <math.h>
#include "poly.h"

#define XMAX 1280	/* hypothetical image width */
#define YMAX 1024	/* hypothetical image height */

#define FRANDOM() ((rand()&32767)/32767.)   /* random number between 0 and 1 */

void pixelproc();

main(ac, av)
int ac;
char **av;
{
    int i;
    Poly p;
    static Window win = {0, 0, XMAX-1, YMAX-1};	/* screen clipping window */

    if (ac<2 || ac != 2+3*(p.n = atoi(av[1]))) {
	fprintf(stderr, "Usage: scantest N X1 Y1 Z1 X2 Y2 Z2 ... XN YN ZN\n");
	exit(1);
    }
    for (i=0; i<p.n; i++) {
	p.vert[i].sx = atof(av[2+3*i]);	/* set screen space x,y,z */
	p.vert[i].sy = atof(av[3+3*i]);
	p.vert[i].sz = atof(av[4+3*i]);
	p.vert[i].r = FRANDOM();	/* random vertex colors, for kicks */
	p.vert[i].g = FRANDOM();
	p.vert[i].b = FRANDOM();
    }
    /* interpolate sx, sy, sz, r, g, and b in poly_scan */
    p.mask = POLY_MASK(sx) | POLY_MASK(sy) | POLY_MASK(sz) |
	POLY_MASK(r) | POLY_MASK(g) | POLY_MASK(b);

    poly_print("scan converting the polygon", &p);
    
    poly_scan(&p, &win, pixelproc);	/* scan convert! */
}

static void pixelproc(x, y, point)	/* called at each pixel by poly_scan */
int x, y;
Poly_vert *point;
{
    printf("pixel (%d,%d) screenz=%g rgb=(%g,%g,%g)\n",
	x, y, point->sz, point->r, point->g, point->b);

    /*
     * in real graphics program you could read and write pixels, e.g.:
     *
     *	if (point->sz < zbuffer_read(x, y)) {
     *	    image_write_rgb(x, y, point->r, point->g, point->b);
     *	    zbuffer_write(x, y, point->sz);
     *  }
     */
}
