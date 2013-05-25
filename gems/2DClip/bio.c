
/*
 * file  bio.c
 *	contains the basic operations
 *
 */
#include	<stdio.h>
#include	"GraphicsGems.h"
#include	"line.h"

/*
 * def_contour
 *
 *	Purpose:
 *	add a contour to the list
 *	NOTE: coordinates must already be converted into longs!
 *	
 *	x	x coordinates of the end points of segments
 *	y	y coordinates of the end points of segments
 *	n	number of coordinate pairs
 *	no	contour number (id), does no have to be unique!
 *	type	type of clip operation desired CLIP_NORMAL means
 *		clip everything inside the contour
 */
def_contour(x, y, n, no, type)
long	x[], y[];
int	n, no, type;
{
	short	i;
	long	dx1, dx2, dy1, dy2;
	long	minx, miny, maxx, maxy;
	CONTOUR	*cp;
	SEGMENT	*sp, *spp;

	if((cp = CL) == (CONTOUR *)NULL) {
		cp = CL = NEWTYPE(CONTOUR);
	}
	else {
		while(cp->_next != (CONTOUR *)NULL)
			cp = cp->_next;
		i = cp->_no;
		cp = cp->_next = NEWTYPE(CONTOUR);
	}

	cp->_next = (CONTOUR *)NULL;
	cp->_no = no;
	SET_ON(cp);
	if(type == CLIP_NORMAL)
		SET_INVERSE(cp);
	else
		SET_NORMAL(cp);
	minx = miny = 1000000;
	maxx = maxy = -1;
	for(i=0; i<n; i++) {
		if(i == 0) {
			cp->_s = sp = NEWTYPE(SEGMENT);
			sp->_from._x = x[0];
			sp->_from._y = y[0];
			sp->_to._x   = x[1];
			sp->_to._y   = y[1];
		}
		else {
		/*
		 * if necessary stretch the contour
		 * and skip the point
		 */
			dx1 = sp->_to._x - sp->_from._x;
			dy1 = sp->_to._y - sp->_from._y;
			dx2 = x[(i == n-1) ? 0 : i+1] - sp->_to._x;
			dy2 = y[(i == n-1) ? 0 : i+1] - sp->_to._y;
			if(dy2*dx1 == dy1*dx2) {
				sp->_to._x = x[(i == n-1) ? 0 : i+1];
				sp->_to._y = y[(i == n-1) ? 0 : i+1];
			}
			else {
				spp = sp;
				sp = sp->_next =  NEWTYPE(SEGMENT);
				sp->_prev = spp;
				sp->_from._x = x[i];
				sp->_from._y = y[i];
				sp->_to._x = x[(i == n-1) ? 0 : i+1];
				sp->_to._y = y[(i == n-1) ? 0 : i+1];
			}
		}

/*
 * calculate the enclosing box
 */
		if(x[i] < minx) 
			minx = x[i];
		if(x[i] > maxx)
			maxx = x[i];
		if(y[i] < miny)
			miny = y[i];
		if(y[i] > maxy)
			maxy = y[i];
			
	}
	cp->_minx = minx;
	cp->_maxx = maxx;
	cp->_miny = miny;
	cp->_maxy = maxy;
	sp->_next = cp->_s;
	cp->_s->_prev = sp;
	cp->_next = (CONTOUR *)NULL;
}

/*
 * get_contour_ptr
 *
 *	PURPOSE
 *	get the pointer to a contour given its id
 *	with multiple id's first fit algorithm is
 *	used. Returns NULL in case of error.
 *
 *	no	id of contour
 */
CONTOUR	*get_contour_ptr(no)
int	no;
{
	CONTOUR	*cp;

	if((cp = CL) == (CONTOUR *)NULL) 
		return((CONTOUR *)NULL);
	else {
		while(cp != (CONTOUR *)NULL) {
			if(cp->_no == no)
				return(cp);
			else
				cp = cp->_next;
		}
		return((CONTOUR *)NULL);
	}
}


/*
 * del_contour
 *
 *	PURPOSE
 *	delete contour(s) from the list with id
 *	no
 */
del_contour(no)
int	no;
{
	CONTOUR	*cp, *cpp;
	CONTOUR	*qp = (CONTOUR *)NULL;
	SEGMENT	*sp, *spp;

	if((cp = CL) == (CONTOUR *)NULL)
		return;
	while(cp != (CONTOUR *)NULL) {
		if(cp->_no == no) {
			sp = cp->_s;
			do {
				spp = sp->_next;
				free(sp);
				sp = spp;
			} while(sp != cp->_s);
			cpp = cp->_next;
			free(cp);
			if(qp)
				qp->_next = cpp;
			else
				CL = cpp;
			cp = cpp;
		} 
		else {
			qp = cp;
			cp = cp->_next;
		}
	}
}

 
