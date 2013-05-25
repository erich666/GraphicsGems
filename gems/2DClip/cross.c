
/*
 * file cross.c:
 *	calculate the intersections
 */
#include	<math.h>
#include	"GraphicsGems.h"
#include	"line.h"
#include	"box.h"
/*
 * cross_calc:
 *
 *	PURPOSE
 *	calculate the intersections between the polygon
 *	stored in poly and the linesegment stored in l
 *	and put the intersections into psol.
 *
 *	poly	pointer to the structure containing the polygon
 *	l	pointer to the structure containing the linesegment
 *	psol	pointer to the pointer where intersections are stored
 *	nsol	current number of intersections stored
 *	nsmax	maximum storage in psol for intersections
 *		if nsol exceeds nsmax additional storage is allocated
 *
 */
cross_calc(poly, l, psol, nsol, nsmax)
CONTOUR	*poly;
SEGMENT	*l;
CLIST	**psol;
short	*nsol, nsmax;
{
	SEGMENT	*p;
	CLIST	*sol;
	double	s;
	long	x, y, a, b, c;
	int	psort(), type;

	sol = *psol;
	p = poly->_s;
	do {
/*
 * calculate the a, b and c coefficients and determine the
 * type of intersection
 */

		a = (p->_to._y - p->_from._y)*(l->_to._x - l->_from._x) -
		    (p->_to._x - p->_from._x)*(l->_to._y - l->_from._y);
		b = (p->_from._x - l->_from._x)*(l->_to._y - l->_from._y) -
		    (p->_from._y - l->_from._y)*(l->_to._x - l->_from._x);
		c = (p->_from._x - l->_from._x)*(p->_to._y - p->_from._y) -
		    (p->_from._y - l->_from._y)*(p->_to._x - p->_from._x);
		if(a == 0)
			type = (b == 0) ? COINCIDE : PARALLEL;
		else {
			if(a > 0) {
				if((b >= 0 && b <= a) &&
				   (c >= 0 && c <= a))
					type = CROSS;
				else
					type = NO_CROSS;
			}
			else {
				if((b <= 0 && b >= a) &&
				   (c <= 0 && c >= a))
					type = CROSS;
				else
					type = NO_CROSS;
			}
		}
/*
 * process the intersection found
 */
		switch(type) {
			case NO_CROSS: case PARALLEL:
				break;

			case CROSS:
				if(b == a || c == a || c == 0)
					break;
				if(b == 0 && 
				   p_where(&(p->_prev->_from), &(p->_to), l) >= 0)
					break;
				s = (double)b/(double)a;
				if(l->_from._x == l->_to._x)
					x = l->_from._x;
				else
					x = p->_from._x + 
					   (int)((p->_to._x - p->_from._x)*s);
				if(l->_from._y == l->_to._y)
					y = l->_from._y;
				else
					y = p->_from._y + 
					   (int)((p->_to._y - p->_from._y)*s);

				if(*nsol == nsmax) {
					nsmax *= 2;
					*psol = sol = (CLIST *) realloc(sol, 							nsmax*sizeof(CLIST)); 
				}
				sol[*nsol]._p._x = x;
				sol[*nsol]._p._y = y;
				sol[*nsol]._type = STD;
				*nsol += 1;
				break;

			case COINCIDE:
				if(p_where(&(p->_prev->_from),
					 &(p->_next->_to), l) > 0)
					break;
				if(l->_from._x != l->_to._x) {
					if((MAX(l->_from._x, l->_to._x) <
					    MIN(p->_from._x, p->_to._x)  ) ||
					   (MIN(l->_from._x, l->_to._x) >
					    MAX(p->_from._x, p->_to._x))   )
						break;
					if(MIN(l->_from._x, l->_to._x) <
					   MIN(p->_from._x, p->_to._x) ) {
						if(*nsol == nsmax) {
							nsmax *= 2;
							*psol = sol = (CLIST *) realloc(sol, 								nsmax*sizeof(CLIST));
						}
						sol[*nsol]._p._x = p->_from._x;
						sol[*nsol]._p._y = p->_from._y;
						sol[*nsol]._type = DELAY;
						*nsol += 1;
					}
					if(MAX(l->_from._x, l->_to._x) >
					   MAX(p->_from._x, p->_to._x) ) {
						if(*nsol == nsmax) {
							nsmax *= 2;
							*psol = sol = (CLIST *) realloc(sol, 								nsmax*sizeof(CLIST));
						}
						sol[*nsol]._p._x = p->_to._x;
						sol[*nsol]._p._y = p->_to._y;
						sol[*nsol]._type = DELAY;
						*nsol += 1;
					}
				}
				else {

					if((MAX(l->_from._y, l->_to._y) <
					    MIN(p->_from._y, p->_to._y)  ) ||
					   (MIN(l->_from._y, l->_to._y) >
					    MAX(p->_from._y, p->_to._y)) )
						break;
					if(MIN(l->_from._y, l->_to._y) <
					   MIN(p->_from._y, p->_to._y) ) {
						if(*nsol == nsmax) {
							nsmax *= 2;
							*psol = sol = (CLIST *) realloc(sol, 								nsmax*sizeof(CLIST));
						}
						sol[*nsol]._p._x = p->_from._x;
						sol[*nsol]._p._y = p->_from._y;
						sol[*nsol]._type = DELAY;
						*nsol += 1;
					}
					if(MAX(l->_from._y, l->_to._y) >
					   MAX(p->_from._y, p->_to._y) ) {
						if(*nsol == nsmax) {
							nsmax *= 2;
							*psol = sol = (CLIST *) realloc(sol, 								nsmax*sizeof(CLIST));
						}
						sol[*nsol]._p._x = p->_to._x;
						sol[*nsol]._p._y = p->_to._y;
						sol[*nsol]._type = DELAY;
						*nsol += 1;
					}
				}
				break;
		}
		p = p->_next;
	} while(p != poly->_s);
	qsort(sol, *nsol, sizeof(CLIST), psort);
}


/*
 * p_where
 *
 *	PURPOSE
 *	determine position of point p1 and p2 relative to
 *	linesegment l. 
 *	Return value
 *	< 0	p1 and p2 lie at different sides of l
 *	= 0	one of both points lie on l
 *	> 0	p1 and p2 lie at same side of l
 *
 *	p1	pointer to coordinates of point
 *	p2	pointer to coordinates of point
 *	l	pointer to linesegment
 * 
 */
p_where(p1, p2, l)
POINT	*p1, *p2;
SEGMENT	*l;
{
	long	dx, dy, dx1, dx2, dy1, dy2, p_1, p_2;

	dx  = l->_to._x - l->_from._x;
	dy  = l->_to._y - l->_from._y;
	dx1 = p1->_x - l->_from._x;
	dy1 = p1->_y - l->_from._y;
	dx2 = p2->_x - l->_to._x;
	dy2 = p2->_y - l->_to._y;
	p_1 = (dx*dy1 - dy*dx1);
	p_2 = (dx*dy2 - dy*dx2);
	if(p_1 == 0 || p_2 == 0)
		return(0);
	else {
		if((p_1 > 0 && p_2 < 0) || (p_1 < 0 && p_2 > 0))
			return(-1);
		else
			return(1);
	}
}


/*
 * p_inside
 *
 *	PURPOSE
 *	determine if the point stored in pt lies inside
 *	the polygon stored in p
 *	Return value:
 *	FALSE	pt lies outside p
 *	TRUE	pt lies inside  p
 *
 *	p	pointer to the polygon
 *	pt	pointer to the point
 */	
boolean	p_inside(p, pt)
CONTOUR	*p;
POINT	*pt;
{
	SEGMENT	l;
	CLIST	*sol;
	short	nsol = 0, nsmax = 2, reduce = 0, i;
	boolean	on_contour(), odd;
	
	l._from._x = p->_minx-2;
	l._from._y = pt->_y;
	l._to._x   = pt->_x;
	l._to._y   = pt->_y;
	sol = (CLIST *) calloc(2, sizeof(CLIST));
	cross_calc(p, &l, &sol, &nsol, nsmax);
	for(i=0; i<nsol-1; i++)
		if(sol[i]._type == DELAY && sol[i+1]._type == DELAY)
			reduce++;
	free(sol);
	odd = (nsol - reduce) & 0x01;
	return(odd ? !on_contour(p, pt) : FALSE);
}

/*
 * function used for sorting
 */
psort(p1, p2)
CLIST	*p1, *p2;
{
	if(p1->_p._x != p2->_p._x)
		return(p1->_p._x - p2->_p._x);
	else 
		return(p1->_p._y - p2->_p._y);
}


/*
 * on_contour
 *
 *	PURPOSE
 *	determine if the point pt lies on the
 *	contour p.
 *	Return value
 *	TRUE 	point lies on contour
 *	FALSE	point lies not on contour
 *
 *	p	pointer to the polygon structure
 *	pt	pointer to the point
 */
boolean	on_contour(p, pt)
CONTOUR	*p;
POINT	*pt;
{
	SEGMENT	*sp;
	long	dx1, dy1, dx2, dy2;

	sp = p->_s;
	do {
		if((pt->_x >= MIN(sp->_from._x, sp->_to._x)) &&
		   (pt->_x <= MAX(sp->_from._x, sp->_to._x))    ) { 
			dx1 = pt->_x - sp->_from._x;
			dx2 = sp->_to._x - pt->_x;
			dy1 = pt->_y - sp->_from._y;
			dy2 = sp->_to._y - pt->_y;
			if(dy1*dx2 == dy2*dx1)
				return(TRUE);
		}
		sp = sp->_next;
	} while(sp != p->_s);
	return(FALSE);
}
 
