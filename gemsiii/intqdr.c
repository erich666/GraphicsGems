#include	<math.h>
#include	"GraphicsGems.h"

/* ----	intqdr - Intersect a ray with a quadric surface. --------------	*/
/*									*/
/*									*/
/*	Description:							*/
/*	    Intqdr determines the intersection of a ray with a quadric	*/
/*	    surface in matrix form.					*/
/*									*/
/*	On entry:							*/
/*	    raybase = The coordinate defining the base of the		*/
/*		      intersecting ray.					*/
/*	    raycos  = The direction cosines of the above ray.		*/
/*	    base    = The location of the base of the quadric.		*/
/*	    axis    = The axis of symmetry for the quadric.		*/
/*	    radius  = The bounding radius from the axis of symmetry.	*/
/*	    Q       = A 4x4 coefficient matrix representing the quadric	*/
/*		      surface.						*/
/*									*/
/*	On return:							*/
/*	    rin     = The entering distance of the intersection.	*/
/*	    rout    = The leaving  distance of the intersection.	*/
/*									*/
/*									*/
/*	Returns:  True if the ray intersects the quadric surface.	*/
/*									*/
/* --------------------------------------------------------------------	*/


boolean	intqdr	(raybase,raycos,base,axis,radius,Q,rin,rout)

	Point3	raybase;		/* Base of the intersection ray	*/
	Vector3	raycos;			/* Direction cosines of the ray	*/
	Vector3	base;			/* Base point for the quadric	*/
	Vector3	axis;			/* Axis of symmetry		*/
	double	radius;			/* Bounding radius		*/
	Matrix4	Q;			/* Quadric coefficient matrix	*/
	double	*rin;			/* Entering distance		*/
	double	*rout;			/* Leaving  distance		*/

{
	boolean	hit;			/* True if ray intersects quad.	*/
	double	qa, qb, qc, qd, qe,	/* Coefficient matrix terms	*/
		qf, qg, qh, qi, qj;
	double	k2, k1, k0;		/* Quadric coefficients		*/
	double	disc;
	Vector3	D;			/* Ray base to quadric base	*/
	double	d;			/* Shortest distance between	*/
					/* ray and axis of symmetry	*/
	Vector3	N;


/*	Compute the perpendicular distance between the symmetry axis	*/
/*	and the ray.							*/

	V3Sub   (&base,&raybase,&D);	/* compute shortest distance	*/
	V3Cross (&raycos,&axis,&N);
	if  ((d=V3Length(&N)) == 0) {			/* if parallel	*/
	    V3Cross (&raycos,&D,&N);
	    d = fabs (V3Length(&N));
	} else {
	    V3Normalize (&N);
	    d = fabs (V3Dot(&D,&N)/d);
	}

	hit = (d <= radius);
	if  (!hit) return (hit);	/* If outside of cylinder	*/


	qa = Q.element[0][0];
	qb = Q.element[0][1] + Q.element[1][0];
	qc = Q.element[0][2] + Q.element[2][0];
	qd = Q.element[0][3] + Q.element[3][0];
	qe = Q.element[1][1];
	qf = Q.element[1][2] + Q.element[2][1];
	qg = Q.element[1][3] + Q.element[3][1];
	qh = Q.element[2][2];
	qi = Q.element[2][3] + Q.element[3][2];
	qj = Q.element[3][3];

	k2 = raycos.x * (raycos.x*qa + raycos.y*qb) +
	     raycos.y * (raycos.y*qe + raycos.z*qf) +
	     raycos.z * (raycos.z*qh + raycos.z*qc);
	k1 = raycos.x * ((raybase.x*2.*qa + raybase.y*qb + raybase.z*qc) + qd) +
	     raycos.y * ((raybase.x*qb + raybase.y*2.*qe + raybase.z*qf) + qg) +
	     raycos.z * ((raybase.x*qc + raybase.y*qg + raybase.z*2.*qh) + qi);
	k0 = raybase.x * (raybase.x*qa + raybase.y*qb + raybase.z*qc + qd) +
	     raybase.y * (               raybase.y*qe + raybase.z*qf + qg) +
	     raybase.z * (                              raybase.z*qh + qi) +
	     qj;

	disc = k1*k1 - 4.*k2*k0;
  
	hit  = (disc >= 0.0);

	if  (hit) { 				/* If ray hits quadric	*/
	    disc  =  sqrt(disc);
	    *rin  = (-k1 - disc) / (2.*k2);	/*    entering distance	*/
	    *rout = (-k1 + disc) / (2.*k2);	/*    leaving distance	*/
	}

	return (hit);
}
