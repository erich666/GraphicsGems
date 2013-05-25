/*
 * file clip.c
 *	contains the actual clipping routines
 */
#include	<stdio.h>
#include	"GraphicsGems.h"
#include	"line.h"

/*
 * vis_vector
 *
 *	PURPOSE
 *	actual user interface. Draws a clipped line
 *	NOTE: coordinates are given in converted LONGS!
 *
 *	xf, yf	from coordinates of vector to be drawn
 *	xt, yt	to coordinates of vector to be drawn
 */
vis_vector(xf, yf, xt, yt)
long	xf, yf, xt, yt;
{
	SEGMENT	l;

	if(xf == xt && yf == yt)
		return;
	l._from._x = xf;
	l._from._y = yf;
	l._to._x   = xt;
	l._to._y   = yt;
/*
 * start at top of list
 */
	clip(CL, &l);
}

/*
 * clip
 *
 *	PURPOSE
 *
 *	p	pointer to polygon
 *	l	pointer to line segment
 */
clip(p, l)
CONTOUR	*p;
SEGMENT	*l;
{
	SEGMENT	ss;
	CLIST	*sol;
	POINT	pt;
	boolean	up, delay, inside, p_inside(), disjunct();
	int	i;
	short	nsol, nsmax = 2;


/*
 * list exhausted do what you like
 * we want to plot
 */
	if(p == (CONTOUR *)NULL) {
		move((l->_from._x), (l->_from._y));
		cont((l->_to._x), (l->_to._y));
		return;
	}
/*
 * polygon is switched off
 * take next one
 */
	if(!IS_ON(p)) {
		clip(p->_next, l);
		return;
	}
/*
 * comparison on basis of the
 * enclosing rectangle
 */
	if(disjunct(p, l)) {
		if(!IS_NORMAL(p)) {
			clip(p->_next, l);
		}
		return;
	}
/*
 * calculate possible intersections
 */
	sol = (CLIST *) calloc(2, sizeof(CLIST));
	sol[0]._p._x = l->_from._x;
	sol[0]._p._y = l->_from._y;
	sol[0]._type = STD;
	sol[1]._p._x = l->_to._x;
	sol[1]._p._y = l->_to._y;
	sol[1]._type = STD;
	nsol = 2;
	cross_calc(p, l, &sol, &nsol, nsmax);
	pt._x = sol[0]._p._x;
	pt._y = sol[0]._p._y;

/*
 * determine status of first point
 */
	inside = p_inside(p, &pt);
	if((!inside && IS_NORMAL(p)) || (inside && !IS_NORMAL(p)))
		up = TRUE; 
	else
		up = FALSE;
	delay = FALSE;
/*
 * process list of intersections
 */
	for(i=1; i<nsol; i++) {
		if(!up) {
			ss._from._x = sol[i-1]._p._x;
			ss._from._y = sol[i-1]._p._y;
			ss._to._x = sol[i]._p._x;
			ss._to._y = sol[i]._p._y;
			clip(p->_next, &ss);
		}
		if(!delay) {
			if(sol[i]._type != DELAY)
				up = (up) ? FALSE : TRUE;
			else
				delay = TRUE;
		}
		else {
			up = (up) ? FALSE : TRUE;
			delay = FALSE;
		}
	}
	free(sol);
}

/*
 * disjunct
 *
 *	PURPOSE
 *	determine if the box enclosing the polygon 
 *	stored in p and the box enclosing the line 
 *	segment stored in l are disjunct.
 *	Return TRUE if disjunct else FALSE
 *
 *	p	points to the polygon structure
 *	l	points to the linesegment structure	
 *
 */
boolean	disjunct(p, l)
CONTOUR	*p;
SEGMENT	*l;

{
	if((MAX(l->_from._x, l->_to._x) < p->_minx) ||
	   (MIN(l->_from._x, l->_to._x) > p->_maxx) ||
           (MAX(l->_from._y, l->_to._y) < p->_miny) ||
	   (MIN(l->_from._y, l->_to._y) > p->_maxy)   )
		return(TRUE);
	else
		return(FALSE);
}

#define DEBUG
#ifdef DEBUG
move(x, y)
long	x, y;
{
	printf("(%d,%d) ->", x, y);
}

cont(x, y)
long	x, y;
{
	printf("(%d,%d)\n", x, y);
}

#endif




