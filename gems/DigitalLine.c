/*
 * Digital Line Drawing
 * by Paul Heckbert
 * from "Graphics Gems", Academic Press, 1990
 */

/*
 * digline: draw digital line from (x1,y1) to (x2,y2),
 * calling a user-supplied procedure at each pixel.
 * Does no clipping.  Uses Bresenham's algorithm.
 *
 * Paul Heckbert	3 Sep 85
 */

#include "GraphicsGems.h"

digline(x1, y1, x2, y2, dotproc)
int x1, y1, x2, y2;
void (*dotproc)();
{
    int d, x, y, ax, ay, sx, sy, dx, dy;

    dx = x2-x1;  ax = ABS(dx)<<1;  sx = SGN(dx);
    dy = y2-y1;  ay = ABS(dy)<<1;  sy = SGN(dy);

    x = x1;
    y = y1;
    if (ax>ay) {		/* x dominant */
	d = ay-(ax>>1);
	for (;;) {
	    (*dotproc)(x, y);
	    if (x==x2) return;
	    if (d>=0) {
		y += sy;
		d -= ax;
	    }
	    x += sx;
	    d += ay;
	}
    }
    else {			/* y dominant */
	d = ax-(ay>>1);
	for (;;) {
	    (*dotproc)(x, y);
	    if (y==y2) return;
	    if (d>=0) {
		x += sx;
		d -= ay;
	    }
	    y += sy;
	    d += ax;
	}
    }
}
