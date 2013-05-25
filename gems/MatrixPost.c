/*
Efficient Post-Concatenation of Transformation Matrices
by Joseph M. Cychosz
from "Graphics Gems", Academic Press, 1990
*/

#include	<math.h>
#include	"GraphicsGems.h"


/*   	M4xform.c - Basic 4x4 transformation package.
 *									 
 *	Description:							 
 *	    M4xform.c contains a collection of routines used to perform  
 *	    direct post-concatenated transformation operations.
 *		 
 *									 
 *	Contents:							 
 *	    M4RotateX		Post-concatenate a x-axis rotation.	 
 *	    M4RotateY		Post-concatenate a y-axis rotation.	 
 *	    M4RotateZ		Post-concatenate a z-axis rotation.	 
 *	    M4Scale			Post-concatenate a scaling.		 
 *	    M4Translate		Post-concatenate a translation.		 
 *	    M4ZPerspective	Post-concatenate a z-axis perspective	 
 *						transformation.				 
 *									 
 *	Externals:							 
 *	    cos, sin.											 
 */ 
	 


/*  	M4RotateX - Post-concatenate a x-axis rotation matrix.  */

Matrix4	*M4RotateX	(m,a)
		Matrix4	*m;			/* Current transformation matrix*/
		double	a;			/* Rotation angle		*/
{
		double	c, s;
		double	t;
		int		i;


		c = cos (a);	s = sin (a);

		for (i = 0 ; i < 4 ; i++) {
	     	t = m->element[i][1];
	     	m->element[i][1] = t*c - m->element[i][2]*s;
	     	m->element[i][2] = t*s + m->element[i][2]*c;
		}
		return (m);
}


/* 	M4RotateY - Post-concatenate a y-axis rotation matrix.  */

Matrix4		*M4RotateY	(m,a)
		Matrix4	*m;			/* Current transformation matrix*/
		double	a;			/* Rotation angle		*/
{
		double	c, s;
		double	t;
		int		i;

		c = cos (a);	s = sin (a);

		for (i = 0 ; i < 4 ; i++) {
	    	 t = m->element[i][0];
	    	 m->element[i][0] = t*c + m->element[i][2]*s;
	    	 m->element[i][2] = m->element[i][2]*c - t*s;
		}
		return (m);
}


/*   	M4RotateZ - Post-concatenate a z-axis rotation matrix.   */

Matrix4	*M4RotateZ	(m,a)
		Matrix4	*m;			/* Current transformation matrix*/
		double	a;			/* Rotation angle		*/
{
		double	c, s;
		double	t;
		int		i;

		c = cos (a);	s = sin (a);

		for (i = 0 ; i < 4 ; i++) {
	    	 t = m->element[i][0];
	    	 m->element[i][0] = t*c - m->element[i][1]*s;
	    	 m->element[i][1] = t*s + m->element[i][1]*c;
		}
		return (m);
}



/*   	M4Scale	- Post-concatenate a scaling.   */

Matrix4	*M4Scale	(m,sx,sy,sz)
		Matrix4	*m;			/* Current transformation matrix */
		double	sx, sy, sz;	/* Scale factors about x,y,z */
{
		int		i;

		for (i = 0 ; i < 4 ; i++) {
	    	 m->element[i][0] *= sx;
	    	 m->element[i][1] *= sy;
	    	 m->element[i][2] *= sz;
		}
		return (m);
}


/*   	M4Translate - Post-concatenate a translation.   */

Matrix4	*M4Translate	(m,tx,ty,tz)
		Matrix4	*m;			/* Current transformation matrix */
		double	tx, ty, tz;	/* Translation distance */
{
		int		i;

		for (i = 0 ; i < 4 ; i++) {
	    	 m->element[i][0] += m->element[i][3]*tx;
	    	 m->element[i][1] += m->element[i][3]*ty;
	    	 m->element[i][2] += m->element[i][3]*tz;
		}
		return (m);
}


		/* M4ZPerspective  Post-concatenate a perspective   	*/		/*transformation.									*/
		/*				                  			      */
		/* Perspective is along the z-axis with the eye at +z.   */

 
Matrix4	*M4ZPerspective	(m,d)
		Matrix4	*m;			/* Current transformation matrix  */
		double	d;			/* Perspective distance		*/
{
		int		i;
		double	f = 1. / d;

		for (i = 0 ; i < 4 ; i++) {
	    	 m->element[i][3] += m->element[i][2]*f;
	    	 m->element[i][2]  = 0.;
		}
		return (m);
}
	
 


