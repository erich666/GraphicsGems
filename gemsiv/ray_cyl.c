/*
 * ANSI C code from the article
 * "Intersecting a Ray with a Cylinder"
 * by Joseph M. Cychosz and Warren N. Waggenspack, Jr.,
 * (3ksnn64@ecn.purdue.edu, mewagg@mewnw.dnet.lsu.edu)
 * in "Graphics Gems IV", Academic Press, 1994
 */

#include	"GraphicsGems.h"
#include	<math.h>

/* ---- intcyl - Intersect a ray with a cylinder. --------------------- */
/*									*/
/*									*/
/*	Description:							*/
/*	    Intcyl determines the intersection of a ray with a		*/
/*	    cylinder.							*/
/*									*/
/*	On entry:							*/
/*	    raybase = The base point of the intersecting ray.		*/
/*	    raycos  = The direction cosines of the above ray. (unit)	*/
/*	    base    = The base location of the cylinder.		*/
/*	    axis    = The axis of symmetry for the cylinder.  (unit)	*/
/*	    radius  = The radius of the cylinder.			*/
/*									*/
/*	On return:							*/
/*	    in	    = The entering distance of the intersection.	*/
/*	    out	    = The leaving  distance of the intersection.	*/
/*									*/
/*	Returns:  True if the ray intersects the cylinder.		*/
/*									*/
/*	Note:	  In and/or out may be negative indicating the		*/
/*		  cylinder is located behind the origin of the ray.	*/
/*									*/
/* -------------------------------------------------------------------- */

#define HUGE		1.0e21		/* Huge value			*/


boolean intcyl	(raybase,raycos,base,axis,radius,in,out)

	Point3		*raybase;	/* Base of the intersection ray */
	Vector3		*raycos;	/* Direction cosines of the ray */
	Point3		*base;		/* Base of the cylinder		*/
	Vector3		*axis;		/* Axis of the cylinder		*/
	double		radius;		/* Radius of the cylinder	*/
	double		*in;		/* Entering distance		*/
	double		*out;		/* Leaving distance		*/

{
	boolean		hit;		/* True if ray intersects cyl	*/
	Vector3		RC;		/* Ray base to cylinder base	*/
	double		d;		/* Shortest distance between	*/
					/*   the ray and the cylinder	*/
	double		t, s;		/* Distances along the ray	*/
	Vector3		n, D, O;
	double		ln;
const	double		pinf = HUGE;	/* Positive infinity		*/


	RC.x = raybase->x - base->x;
	RC.y = raybase->y - base->y;
	RC.z = raybase->z - base->z;
	V3Cross (raycos,axis,&n);

	if  ( (ln = V3Length (&n)) == 0. ) {	/* ray parallel to cyl	*/
	    d	 = V3Dot (&RC,axis);
	    D.x	 = RC.x - d*axis->x;
	    D.y	 = RC.y - d*axis->y;
	    D.z	 = RC.z - d*axis->z;
	    d	 = V3Length (&D);
	    *in	 = -pinf;
	    *out =  pinf;
	    return (d <= radius);		/* true if ray is in cyl*/
	}

	V3Normalize (&n);
	d    = fabs (V3Dot (&RC,&n));		/* shortest distance	*/
	hit  = (d <= radius);

	if  (hit) {				/* if ray hits cylinder */
	    V3Cross (&RC,axis,&O);
	    t = - V3Dot (&O,&n) / ln;
	    V3Cross (&n,axis,&O);
	    V3Normalize (&O);
	    s = fabs (sqrt(radius*radius - d*d) / V3Dot (raycos,&O));
	    *in	 = t - s;			/* entering distance	*/
	    *out = t + s;			/* exiting  distance	*/
	}

	return (hit);
}


/* ---- clipobj - Clip object with plane pair. ------------------------ */
/*									*/
/*									*/
/*	Description:							*/
/*	    Clipobj clips the supplied infinite object with two		*/
/*	    (a top and a bottom) bounding planes.			*/
/*									*/
/*	On entry:							*/
/*	    raybase = The base point of the intersecting ray.		*/
/*	    raycos  = The direction cosines of the above ray. (unit)	*/
/*	    bot	    = The normal and perpendicular distance of the	*/
/*		      bottom plane.					*/
/*	    top	    = The normal and perpendicular distance of the	*/
/*		      top plane.					*/
/*	    objin   = The entering distance of the intersection with	*/
/*		      the object.					*/
/*	    objout  = The exiting  distance of the intersection with	*/
/*		      the object.					*/
/*									*/
/*	On return:							*/
/*	    objin   = The entering distance of the intersection.	*/
/*	    objout  = The exiting  distance of the intersection.	*/
/*	    surfin  = The identifier for the entering surface.		*/
/*	    surfout = The identifier for the leaving surface.		*/
/*									*/
/*	Returns:  True if the ray intersects the bounded object.	*/
/*									*/
/* -------------------------------------------------------------------- */

#define		SIDE	0		/* Object surface		*/
#define		BOT	1		/* Bottom end-cap surface	*/
#define		TOP	2		/* Top	  end-cap surface	*/

typedef struct	{			/* Plane: ax + by + cz + d = 0	*/
	double	a ,b ,c, d;
}	Plane;


boolean clipobj		(raybase,raycos,bot,top,objin,objout,surfin,surfout)

	Point3		*raybase;	/* Base of the intersection ray */
	Vector3		*raycos;	/* Direction cosines of the ray */
	Plane		*bot;		/* Bottom end-cap plane		*/
	Plane		*top;		/* Top end-cap plane		*/
	double		*objin;		/* Entering distance		*/
	double		*objout;	/* Exiting  distance		*/
	int		*surfin;	/* Entering surface identifier	*/
	int		*surfout;	/* Exiting  surface identifier	*/

{
	double	dc, dw, t;
	double	in, out;		/* Object  intersection dists.	*/

	*surfin = *surfout = SIDE;
	in  = *objin;
	out = *objout;

/*	Intersect the ray with the bottom end-cap plane.		*/

	dc = bot->a*raycos->x  + bot->b*raycos->y  + bot->c*raycos->z;
	dw = bot->a*raybase->x + bot->b*raybase->y + bot->c*raybase->z + bot->d;

	if  ( dc == 0.0 ) {		/* If parallel to bottom plane	*/
	    if	( dw >= 0. ) return (FALSE);
	} else {
	    t  = - dw / dc;
	    if	( dc >= 0.0 ) {			    /* If far plane	*/
		if  ( t > in && t < out ) { out = t; *surfout = BOT; }
		if  ( t < in  ) return (FALSE);
	     } else {				    /* If near plane	*/
		if  ( t > in && t < out ) { in	= t; *surfin  = BOT; }
		if  ( t > out ) return (FALSE);
	    }
	}

/*	Intersect the ray with the top end-cap plane.			*/

	dc = top->a*raycos->x  + top->b*raycos->y  + top->c*raycos->z;
	dw = top->a*raybase->x + top->b*raybase->y + top->c*raybase->z + top->d;

	if  ( dc == 0.0 ) {		/* If parallel to top plane	*/
	    if	( dw >= 0. ) return (FALSE);
	} else {
	    t  = - dw / dc;
	    if	( dc >= 0.0 ) {			    /* If far plane	*/
		if  ( t > in && t < out ) { out = t; *surfout = TOP; }
		if  ( t < in  ) return (FALSE);
	     } else {				    /* If near plane	*/
		if  ( t > in && t < out ) { in	= t; *surfin  = TOP; }
		if  ( t > out ) return (FALSE);
	    }
	}

	*objin	= in;
	*objout = out;
	return (in < out);
}
