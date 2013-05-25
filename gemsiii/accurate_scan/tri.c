#include <stdio.h>
#include <math.h>

extern void draw_point(int x, int y);

#include "fixpoint.h"

#define SWAP(_a_, _b_, _c_) { _c_ = _a_; _a_ = _b_; _b_ = _c_; }
#define min(_a_, _b_) ((_a_ < _b_) ? _a_ : _b_)
#define max(_a_, _b_) ((_a_ > _b_) ? _a_ : _b_)

struct edge {
  int ymin, ymax, xi, si;
  int r, inc, dec;
};

/* floor(x/y). Assumes y>0. */
int floor_div(int x, int y)
{
  if (x >= 0) return(x/y);
  else return((x/y) + (((x % y) == 0) ? 0 : -1));
}

/*
 *  Draws pixels in the half-open interval (x1, x2].
 */
void draw_span(int x1, int x2, int y)
{
  int x;
  for (x=x1+1; x<=x2; x++) draw_point(x, y);
}

/*
 *  Initializes the Bresenham-like scan conversion for a single edge,
 *  setting values in the structure containing the increment variables.
 */
struct edge *EdgeSetup(struct edge *e, int x0, int y0, int x1, int y1)
{
  int sf, dx = x1-x0, dy = y1-y0;

  e->ymin = y0;
  e->ymax = y1;
  
  if (dy != 0) {
	 e->si = floor_div(dx, dy);
	 e->xi = x0 + e->si;
	 sf = dx - e->si * dy;
	 e->r = 2*sf - dy;
	 e->inc = sf;
	 e->dec = sf - dy;
  }
  return(e);
}


/*
 *  Returns the intersection of edge e with the next scanline.
 */
int EdgeScan(struct edge *e)
{
  int x = e->xi;

  if (e->r >= 0) {
	 e->xi += e->si + 1;
	 e->r += e->dec;
  }
  else {
	 e->xi += e->si;
	 e->r += e->inc;
  }
  return x;
}

/*
 *  Scan-converts a triangle with integer endpoints.  Assumes the
 *  triangle's vertices are ordered so that y0 <= y1 <= y2. 
 */
void sorted_triangle(int x0, int y0, int x1, int y1, int x2, int y2)
{
  int det, yi, xmin, xmax;
  struct edge left, right;

  /* Compute handedness of triangle (points left or right) */
  /* (see Pavlidis '82, [Computer Science Press], Ch 14) */
  det = (y1-y0)*(x2-x0) - (x1-x0)*(y2-y0);

  /* Setup first pair of edges */
  if (det < 0)
	 { EdgeSetup(&left, x0, y0, x2, y2);
		EdgeSetup(&right, x0, y0, x1, y1); }
  else
	 { EdgeSetup(&left, x0, y0, x1, y1);
		EdgeSetup(&right, x0, y0, x2, y2); }

  /* Scan first pair of edges. */
  for (yi = left.ymin + 1; yi <= min(left.ymax, right.ymax); yi++) {
	 xmin = EdgeScan(&left);
	 xmax = EdgeScan(&right);
	 draw_span(xmin, xmax, yi);
  }

  /* Setup third edge */
  if (det >= 0) EdgeSetup(&left, x1, y1, x2, y2);
  else          EdgeSetup(&right, x1, y1, x2, y2);

  /* Scan remainder of triangle. */
  for (yi = max(left.ymin, right.ymin) + 1; yi <= left.ymax; yi++) {
	 xmin = EdgeScan(&left);
	 xmax = EdgeScan(&right);
	 draw_span(xmin, xmax, yi);
  }
}


/* 
 *  Scan-converts a triangle with integer endpoints.
 *  Sorts the vertices, then calls a routine to do the scan conversion.
 */
void triangle(int x0, int y0, int x1, int y1, int x2, int y2)
{
  int tmp;
  if (y0>y1) { SWAP(y0,y1,tmp); SWAP(x0,x1,tmp); }
  if (y0>y2) { SWAP(y0,y2,tmp); SWAP(x0,x2,tmp); }
  if (y1>y2) { SWAP(y1,y2,tmp); SWAP(x1,x2,tmp); }
  
  sorted_triangle(x0, y0, x1, y1, x2, y2);
}

/*	 The scan-conversion routines for coordinates at subpixel resolution.
 *
 *  SubEdgeSetup initializes the Bresenham-like scan conversion for a
 *  single edge (analogous to EdgeSetup).
 *
 *  Double lenght fixpoint is used to compute alpha below. Alpha
 *  is then truncated (this corresponds to quantizing to a 
 *  particular subpixel while computing the x-intersection of the edge
 *  with the scanline). It will not produces artifacts except for
 *  degenerate databases, in which adjacent polygons don't necessarily
 *  have coincident vertices (see Lathrop, Kirk, Voorhies, IEEE CG&A 10(5),
 *  1990 for a discussion).
 *
 */
struct edge *
SubEdgeSetup(struct edge *e,
				 fixpoint x0, fixpoint y0, fixpoint x1, fixpoint y1)
{

  fixpoint ymin, ymax, alpha, beta, sf, xi, si;
  fixpoint dx = x1-x0, dy = y1-y0;

  ymin = fp_floor(y0);
  ymax = fp_floor(y1);

  if ((dy != 0) && (ymin != ymax)) {

	 /* Alpha is related to x-intercept with scanline ymin  */
	 alpha = fp_trunc(fp_dbladd(fp_dblmultiply(fp_fraction(x0), dy),
										 fp_dblmultiply(dx, ymin - y0 + fp_fix(1.0))));
	 beta = fp_floor_div(alpha, dy);
	 xi = fp_floor(x0) + beta;

	 /* prevent overflow for v. small dy ('si' is only used if dy>=1) */
	 if (dy >= fp_fix(1.0)) {
		si = fp_floor_div(dx, dy);
		sf = dx - fp_multiply(si, dy); 

		/* (alpha - beta*dy) = fractional part of the x-intercept. */
		e->r = alpha - fp_multiply(beta, dy) + sf - dy; 
		e->si = fp_integer(si);
		e->inc = sf;
		e->dec = sf - dy;
	 }
	 e->xi = fp_integer(xi);
  }
  e->ymin = fp_integer(ymin);
  e->ymax = fp_integer(ymax);

  return(e);
}


/*  Like integer version, but uses SubEdgeSetup */
void subpixel_sorted_triangle(fixpoint x0, fixpoint y0,
                            fixpoint x1, fixpoint y1,
                            fixpoint x2, fixpoint y2)
{
  struct edge left, right;
  int yi, xmin, xmax;

  /* compute determinant using full precision (64 bits) */
  dblfixpoint a = fp_dblmultiply(y1-y0,x2-x0);
  dblfixpoint b = fp_dblmultiply(x1-x0,y2-y0);
  int clockwise = fp_dbllessthan(a, b);

  /* Setup first pair of edges */
  if (clockwise) {
	 SubEdgeSetup(&left, x0, y0, x2, y2);
	 SubEdgeSetup(&right, x0, y0, x1, y1);
  } else {
	 SubEdgeSetup(&left, x0, y0, x1, y1);
	 SubEdgeSetup(&right, x0, y0, x2, y2);
  }
  
  /* Scan first pair of edges. */
  for (yi = left.ymin + 1; yi <= min(left.ymax, right.ymax); yi++) {
	 xmin = EdgeScan(&left);
	 xmax = EdgeScan(&right);
	 draw_span(xmin, xmax, yi);
  }

  /* Setup third edge */
  if (!clockwise) SubEdgeSetup(&left, x1, y1, x2, y2);
  else            SubEdgeSetup(&right, x1, y1, x2, y2);

  /* Scan remainder of triangle. */
  for (yi = max(left.ymin, right.ymin) + 1; yi <= left.ymax; yi++) {
	 xmin = EdgeScan(&left);
	 xmax = EdgeScan(&right);
	 draw_span(xmin, xmax, yi);
  }
}

/* Like integer version. */
void subpixel_triangle(fixpoint x0, fixpoint y0,
							  fixpoint x1, fixpoint y1,
							  fixpoint x2, fixpoint y2)
{
  fixpoint tmp;
  if (y0>y1) { SWAP(y0,y1,tmp); SWAP(x0,x1,tmp); }
  if (y0>y2) { SWAP(y0,y2,tmp); SWAP(x0,x2,tmp); }
  if (y1>y2) { SWAP(y1,y2,tmp); SWAP(x1,x2,tmp); }
  
  subpixel_sorted_triangle(x0, y0, x1, y1, x2, y2);
}


