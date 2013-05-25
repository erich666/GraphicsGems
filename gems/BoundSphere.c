/*
An Efficient Bounding Sphere
by Jack Ritter
from "Graphics Gems", Academic Press, 1990
*/

/* Routine to calculate near-optimal bounding sphere over    */
/* a set of points in 3D */
/* This contains the routine find_bounding_sphere(), */
/* the struct definition, and the globals used for parameters. */
/* The abs() of all coordinates must be < BIGNUMBER */
/* Code written by Jack Ritter and Lyle Rains. */

#include <stdio.h>
#include <math.h>
#include "GraphicsGems.h"

#define BIGNUMBER 100000000.0  		/* hundred million */

/* GLOBALS. These are used as input and output parameters. */
 
struct Point3Struct caller_p,cen;
double rad;
int NUM_POINTS;

/* Call with no parameters. Caller must set NUM_POINTS */
/* before calling. */
/* Caller must supply the routine GET_iTH_POINT(i), which loads his */
/* ith point into the global struct caller_p. (0 <= i < NUM_POINTS). */
/* The calling order of the points is irrelevant. */
/* The final bounding sphere will be put into the globals */
/* cen and rad. */


find_bounding_sphere()
{
register int i;
register double dx,dy,dz;
register double rad_sq,xspan,yspan,zspan,maxspan;
double old_to_p,old_to_p_sq,old_to_new;
struct Point3Struct xmin,xmax,ymin,ymax,zmin,zmax,dia1,dia2;


/* FIRST PASS: find 6 minima/maxima points */
xmin.x=ymin.y=zmin.z= BIGNUMBER; /* initialize for min/max compare */
xmax.x=ymax.y=zmax.z= -BIGNUMBER;
for (i=0;i<NUM_POINTS;i++)
	{
	GET_iTH_POINT(i); /* load global struct caller_p with */
					/* his ith point. */
	if (caller_p.x<xmin.x)
		xmin = caller_p; /* New xminimum point */
	if (caller_p.x>xmax.x)
		xmax = caller_p;
	if (caller_p.y<ymin.y)
		ymin = caller_p;
	if (caller_p.y>ymax.y)
		ymax = caller_p;
	if (caller_p.z<zmin.z)
		zmin = caller_p;
	if (caller_p.z>zmax.z)
		zmax = caller_p;
	}
/* Set xspan = distance between the 2 points xmin & xmax (squared) */
dx = xmax.x - xmin.x;
dy = xmax.y - xmin.y;
dz = xmax.z - xmin.z;
xspan = dx*dx + dy*dy + dz*dz;

/* Same for y & z spans */
dx = ymax.x - ymin.x;
dy = ymax.y - ymin.y;
dz = ymax.z - ymin.z;
yspan = dx*dx + dy*dy + dz*dz;

dx = zmax.x - zmin.x;
dy = zmax.y - zmin.y;
dz = zmax.z - zmin.z;
zspan = dx*dx + dy*dy + dz*dz;

/* Set points dia1 & dia2 to the maximally separated pair */
dia1 = xmin; dia2 = xmax; /* assume xspan biggest */
maxspan = xspan;
if (yspan>maxspan)
	{
	maxspan = yspan;
	dia1 = ymin; dia2 = ymax;
	}
if (zspan>maxspan)
	{
	dia1 = zmin; dia2 = zmax;
	}


/* dia1,dia2 is a diameter of initial sphere */
/* calc initial center */
cen.x = (dia1.x+dia2.x)/2.0;
cen.y = (dia1.y+dia2.y)/2.0;
cen.z = (dia1.z+dia2.z)/2.0;
/* calculate initial radius**2 and radius */
dx = dia2.x-cen.x; /* x component of radius vector */
dy = dia2.y-cen.y; /* y component of radius vector */
dz = dia2.z-cen.z; /* z component of radius vector */
rad_sq = dx*dx + dy*dy + dz*dz;
rad = sqrt(rad_sq);

/* SECOND PASS: increment current sphere */

for (i=0;i<NUM_POINTS;i++)
	{
	GET_iTH_POINT(i); /* load global struct caller_p  */
					/* with his ith point. */
	dx = caller_p.x-cen.x;
	dy = caller_p.y-cen.y;
	dz = caller_p.z-cen.z;
	old_to_p_sq = dx*dx + dy*dy + dz*dz;
	if (old_to_p_sq > rad_sq) 	/* do r**2 test first */
		{ 	/* this point is outside of current sphere */
		old_to_p = sqrt(old_to_p_sq);
		/* calc radius of new sphere */
		rad = (rad + old_to_p) / 2.0;
		rad_sq = rad*rad; 	/* for next r**2 compare */
		old_to_new = old_to_p - rad;
		/* calc center of new sphere */
		cen.x = (rad*cen.x + old_to_new*caller_p.x) / old_to_p;
		cen.y = (rad*cen.y + old_to_new*caller_p.y) / old_to_p;
		cen.z = (rad*cen.z + old_to_new*caller_p.z) / old_to_p;
		/* Suppress if desired */
		printf("\n New sphere: cen,rad = %f %f %f   %f",
			cen.x,cen.y,cen.z, rad);
		}	
	}
} 			 /* end of find_bounding_sphere()  */
