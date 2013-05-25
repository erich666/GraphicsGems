
#include	<math.h>
#include	"GraphicsGems.h"

/* ----	inttor - Intersect a ray with a torus. ------------------------	*/
/*									*/
/*									*/
/*	Description:							*/
/*	    Inttor determines the intersection of a ray with a torus.	*/
/*									*/
/*	On entry:							*/
/*	    raybase = The coordinate defining the base of the		*/
/*		      intersecting ray.					*/
/*	    raycos  = The direction cosines of the above ray.		*/
/*	    center  = The center location of the torus.			*/
/*	    radius  = The major radius of the torus.			*/
/*	    rplane  = The minor radius in the plane of the torus.	*/
/*	    rnorm   = The minor radius normal to the plane of the torus.*/
/*	    tran    = A 4x4 transformation matrix that will position	*/
/*		      the torus at the origin and orient it such that	*/
/*		      the plane of the torus lyes in the x-z plane.	*/
/*									*/
/*	On return:							*/
/*	    nhits   = The number of intersections the ray makes with	*/
/*		      the torus.					*/
/*	    rhits   = The entering/leaving distances of the		*/
/*		      intersections.					*/
/*									*/
/*	Returns:  True if the ray intersects the torus.			*/
/*									*/
/* --------------------------------------------------------------------	*/


int	inttor	(raybase,raycos,center,radius,rplane,rnorm,tran,nhits,rhits)

	Point3	raybase;		/* Base of the intersection ray	*/
	Vector3	raycos;			/* Direction cosines of the ray	*/
	Point3	center;			/* Center of the torus		*/
	double	radius;			/* Major radius of the torus	*/
	double	rplane;			/* Minor planer radius		*/
	double	rnorm;			/* Minor normal radius		*/
	Matrix4	tran;			/* Transformation matrix	*/
	int *	nhits;			/* Number of intersections	*/
	double	rhits[4];		/* Intersection distances	*/

{
	int	hit;			/* True if ray intersects torus	*/
	double	rsphere;		/* Bounding sphere radius	*/
	Vector3	Base, Dcos;		/* Transformed intersection ray	*/
	double	rmin, rmax;		/* Root bounds			*/
	double	yin, yout;
	double	rho, a0, b0;		/* Related constants		*/
	double	f, l, t, g, q, m, u;	/* Ray dependent terms		*/
	double	C[5];			/* Quartic coefficients		*/

extern	int	intsph ();		/* Intersect ray with sphere	*/
extern	int	SolveQuartic ();	/* Solve quartic equation	*/


	*nhits  = 0;

/*	Compute the intersection of the ray with a bounding sphere.	*/

	rsphere = radius + MAX (rplane,rnorm);
	hit     = intsph (raybase,raycos,center,rsphere,&rmin,&rmax);

	if  (!hit) return (hit);	/* If ray misses bounding sphere*/

/*	Transform the intersection ray					*/

	Base = raybase;
	Dcos = raycos;
	V3MulPointByMatrix  (&Base,&tran);
	V3MulVectorByMatrix (&Dcos,&tran);

/*	Bound the torus by two parallel planes rnorm from the x-z plane.*/

	yin  = Base.y + rmin * Dcos.y;
	yout = Base.y + rmax * Dcos.y;
	hit  = !( (yin >  rnorm && yout >  rnorm) ||
		  (yin < -rnorm && yout < -rnorm) );

	if  (!hit) return (hit);	/* If ray is above/below torus.	*/

/*	Compute constants related to the torus.				*/

	rho = rplane*rplane / (rnorm*rnorm);
	a0  = 4. * radius*radius;
	b0  = radius*radius - rplane*rplane;

/*	Compute ray dependent terms.					*/

	f = 1. - Dcos.y*Dcos.y;
	l = 2. * (Base.x*Dcos.x + Base.z*Dcos.z);
	t = Base.x*Base.x + Base.z*Base.z;
	g = f + rho * Dcos.y*Dcos.y;
	q = a0 / (g*g);
	m = (l + 2.*rho*Dcos.y*Base.y) / g;
	u = (t +    rho*Base.y*Base.y + b0) / g;

/*	Compute the coefficients of the quartic.			*/

	C[4] = 1.0;
	C[3] = 2. * m;
	C[2] = m*m + 2.*u - q*f;
	C[1] = 2.*m*u - q*l;
	C[0] = u*u - q*t;
	
/*	Use quartic root solver found in "Graphics Gems" by Jochen	*/
/*	Schwarze.							*/

	*nhits = SolveQuartic (C,rhits);

/*	SolveQuartic returns root pairs in reversed order.		*/
	m = rhits[0]; u = rhits[1]; rhits[0] = u; rhits[1] = m;
	m = rhits[2]; u = rhits[3]; rhits[2] = u; rhits[3] = m;

	return (*nhits != 0);
}

