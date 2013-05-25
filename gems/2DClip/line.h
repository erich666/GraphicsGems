
/*
Two-Dimensional Clipping: A Vector Based Approach
by Hans Spoelder and Fons Ullings
from "Graphics Gems", Academic Press, 1990
*/


/*
 * file line.h
 * 	contains major definitions for the clipping routines
 *
 */
#define	NFAC		10		/* discrete measure	*/
	
#define	SCALE		(1 << NFAC)	/* 1024 points/cm	*/
#define	TO_INT(X)	((int)((X)*SCALE))
#define	TO_FLT(X)	(((float)(X))/SCALE)

#define	COINCIDE		1		/* what do the lines do	*/
#define	PARALLEL		2
#define	CROSS		3
#define	NO_CROSS		4

#define	STD			0		/* crossing types	*/
#define	DELAY		1

#define	CLIP_NORMAL	1

typedef	struct	{		/* holds a point	*/
	long	_x;			/* holds x coordinate	*/
	long	_y;			/* holds y coordinate	*/
} POINT;

typedef	struct	{		/* holds a cross point	*/
	POINT	_p;			/* holds the solution	*/
	short	_type;		/* more information	*/
} CLIST;
	
struct	segment	{			/* holds a segment	*/
	POINT	_from;			/* start coordinates	*/
	POINT	_to;			/* stop coordinates	*/
	struct	segment	*_next;
	struct	segment	*_prev;
};


#define	SEGMENT		struct segment

struct	contour {			/* holds a contour	*/
	short	_no;			/* contour counter	*/
	short	_status;			/* holds information	*/
	short	_cnt;			/* number of elements	*/
	SEGMENT	*_s;			/* the segments		*/
	struct	contour *_next;	/* linked list		*/
	long	_minx;			/* coordinates of box	*/
	long	_miny;
	long	_maxx;
	long	_maxy;
};

#define	CONTOUR		struct contour

#define	ACTIVE		01		/* polygon attributes	*/
#define	NORMAL		02

#define	SET_ON(p)	((p)->_status |=  ACTIVE)
#define	SET_NORMAL(p)	((p)->_status |= NORMAL)

#define	SET_OFF(p)	((p)->_status &= ~ACTIVE)
#define	SET_INVERSE(p)	((p)->_status &= ~NORMAL)

#define	IS_ON(p)	((p)->_status & ACTIVE)
#define	IS_NORMAL(p)	((p)->_status & NORMAL)

extern	CONTOUR	*CL;

CONTOUR	*get_contour_ptr();

extern	short	C_COUNT;
 
