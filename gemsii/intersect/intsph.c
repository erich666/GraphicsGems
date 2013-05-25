
#include	<math.h>
#include	"GraphicsGems.h"

/* ----	intsph - Intersect a ray with a sphere. -----------------------	*/
/*									*/
/*									*/
/*	Description:							*/
/*	    Intsph determines the intersection of a ray with a sphere.	*/
/*									*/
/*	On entry:							*/
/*	    raybase = The coordinate defining the base of the		*/
/*		      intersecting ray.					*/
/*	    raycos  = The direction cosines of the above ray.		*/
/*	    center  = The center location of the sphere.		*/
/*	    radius  = The radius of the sphere.				*/
/*									*/
/*	On return:							*/
/*	    rin     = The entering distance of the intersection.	*/
/*	    rout    = The leaving  distance of the intersection.	*/
/*									*/
/*	Returns:  True if the ray intersects the sphere.		*/
/*									*/
/* --------------------------------------------------------------------	*/


int	intsph	(raybase,raycos,center,radius,rin,rout)

	Point3	raybase;		/* Base of the intersection ray	*/
	Vector3	raycos;			/* Direction cosines of the ray	*/
	Point3	center;			/* Center of the sphere		*/
	double	radius;			/* Radius of the sphere		*/
	double	*rin;			/* Entering distance		*/
	double	*rout;			/* Leaving distance		*/

{
	int	hit;			/* True if ray intersects sphere*/
	double	dx, dy, dz;		/* Ray base to sphere center	*/
	double	bsq, u, disc;
	double	root;


	dx   = raybase.x - center.x;
	dy   = raybase.y - center.y;
	dz   = raybase.z - center.z;
	bsq  = dx*raycos.x + dy*raycos.y + dz*raycos.z; 
	u    = dx*dx + dy*dy + dz*dz - radius*radius;
	disc = bsq*bsq - u;
  
	hit  = (disc >= 0.0);

	if  (hit) { 				/* If ray hits sphere	*/
	    root  =  sqrt(disc);
	    *rin  = -bsq - root;		/*    entering distance	*/
	    *rout = -bsq + root;		/*    leaving distance	*/
	}
  
	return (hit);
}

