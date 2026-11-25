/* 
   Fast Circle-Rectangle Intersection Checking
   by Clifford A. Shaffer
   from "Graphics Gems", Academic Press, 1990
   */

#include "GraphicsGems.h"

boolean Check_Intersect(Box2 *R, Point2 *C, double Rad)

	/* Return TRUE iff rectangle R intersects circle with centerpoint C and
	   radius Rad. */
{
	double Rad2;
	double xmin, xmax, ymin, ymax;

	Rad2 = Rad * Rad;
	/* Translate coordinates, placing C at the origin. */
	/* (Use locals so the caller's rectangle is not modified.) */
	xmax = R->max.x - C->x;  ymax = R->max.y - C->y;
	xmin = R->min.x - C->x;  ymin = R->min.y - C->y;

	if (xmax < 0) 			/* R to left of circle center */
		if (ymax < 0) 		/* R in lower left corner */
			return ((xmax * xmax + ymax * ymax) < Rad2);
		else if (ymin > 0) 	/* R in upper left corner */
			return ((xmax * xmax + ymin * ymin) < Rad2);
		else 					/* R due West of circle */
			return(ABS(xmax) < Rad);
	else if (xmin > 0)  	/* R to right of circle center */
		if (ymax < 0) 	/* R in lower right corner */
			return ((xmin * xmin + ymax * ymax) < Rad2);
		else if (ymin > 0)  	/* R in upper right corner */
			return ((xmin * xmin + ymin * ymin) < Rad2);
		else 				/* R due East of circle */
			return (xmin < Rad);
	else				/* R on circle vertical centerline */
		if (ymax < 0) 	/* R due South of circle */
			return (ABS(ymax) < Rad);
		else if (ymin > 0)  	/* R due North of circle */
			return (ymin < Rad);
		else 				/* R contains circle centerpoint */
			return(TRUE);
} 	
