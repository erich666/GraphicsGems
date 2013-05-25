/*****************************************************************************
*
*	contour.c
*
*	Author:	Tim Feldman
*		Island Graphics Corporation
*		modified 6/13/97 by Michael Beregov <michael@beregov.spb.ru>:
*			correction to Freeman chain termination (contours through
*			the same starting point were not producing the full contour)
*
*	Vectorizes the outline of an elevation contour in a set of sampled
*	data.  Uses Freeman chain encoding.
*
*****************************************************************************/

/*****************************************************************************
*
*	Include files
*
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

/*****************************************************************************
*
*	Constants
*
*****************************************************************************/

/***	these are the coordinates of the edges of the
	rectangular array of sample points		***/

#define X_MIN	0
#define X_MAX	7
#define Y_MIN	0
#define Y_MAX	7

/*****************************************************************************
*
*	Structure definitions
*
*****************************************************************************/

/***	a Vector is one link in a simple chain that follows
	the edge of a contour from sample point to sample point	***/

struct vector
{
	short		dir;
	struct vector *	next;
};

typedef struct vector	Vector;

/***	these are the 'dir' values in a Vector:

	0	right
	1	right and up
	2	up
	3	left and up
	4	left
	5	left and down
	6	down
	7	right and down		***/

/*****************************************************************************
*
*	Global data
*
*****************************************************************************/

/***	this points to the first member in the Freeman chain of vectors	***/

Vector *	chain;

/***	these are the coordinates of the starting point
	of the Freeman chain, in the array of sample points	***/

int	start_x;
int	start_y;

/***	this is the elevation of the contour to be outlined
	in the array of sample points				***/

int	elev;

/***	this is the array of elevation samples for this example	***/

int	sample[8][8] =
{
	100, 100, 100, 100, 100, 100, 100,   0,
	100, 100, 200, 200, 200, 200, 100, 100,
	100, 200, 200, 250, 255, 200, 100, 100,
	100, 200, 200, 250, 200, 200, 100, 100,
	100, 200, 200, 250, 200, 200, 100, 100,
	100, 200, 100, 200, 100, 200, 200, 100,
	100, 200, 100, 100, 100, 200, 200, 200,
	100, 100, 100, 100, 100, 100, 100, 200
};

/*****************************************************************************
*
*	Procedures
*
*****************************************************************************/

/*****************************************************************************
*
*	in_cont(x, y)
*
*	Determines whether the sample point at 'x, y' is within the contour
*	being outlined.  Points outside of the array of samples are not
*	in the contour.
*
*	Returns 0 if the point is not in the contour.
*	Returns 1 if the point is     in the contour.
*
*****************************************************************************/

short
in_cont(x, y)

int	x;
int	y;

{
	if ( (x < X_MIN) || (x > X_MAX) || (y < Y_MIN) || (y > Y_MAX) )
		return(0);

	if (sample[x][y] == elev)
		return(1);
	else
		return(0);
}

/*****************************************************************************
*
*	probe(x, y, dir, new_x, new_y)
*
*	Checks a sample neighboring 'x, y' to see if it is in the contour
*	being outlined.  'dir' specifies which neighboring sample to check.
*	'new_x, new_y' always get the coordinates of the neighbor.
*
*	Returns 0 if the neighbor is not in the contour.
*	Returns 1 if the neighbor is     in the contour.
*
*****************************************************************************/

short
probe(x, y, dir, new_x, new_y)

int	x;
int	y;
int	dir;
int *	new_x;
int *	new_y;

{
	/***	figure out coordinates of neighbor	***/

	if ( (dir < 2) || (dir > 6) )
		++x;

	if ( (dir > 2) && (dir < 6) )
		--x;

	if ( (dir > 0) && (dir < 4) )
		++y;

	if (dir > 4)
		--y;

	/***	always return the new coordinates	***/

	*new_x = x;
	*new_y = y;

	/***	determine if the new sample point is in the contour	***/

	return(in_cont(x, y));
}

/*****************************************************************************
*
*	neighbor(x, y, last_dir, new_x, new_y)
*
*	Finds a neighbor of the sample at 'x, y' that is in the same
*	contour.  Always follows the contour in a clockwise direction.
*	'last_dir' is the direction that was used to get to 'x, y'
*	when it was found.  'new_x, new_y' always get the coordinates
*	of the neighbor.
*
*	This procedure should only be called for a sample that has at
*	least one neighbor in the same contour.
*
*	Returns the direction to the neighbor.
*
*****************************************************************************/

int
neighbor(x, y, last_dir, new_x, new_y)

int	x;
int	y;
int	last_dir;
int *	new_x;
int *	new_y;

{
int	n;
int	new_dir;

	/***	figure out where to start looking for a neighbor --
		always look ahead and to the left of the last direction

		if the last vector was 0
		then start looking at  1

		if the last vector was 1
		then start looking at  3

		if the last vector was 2
		then start looking at  3

		if the last vector was 3
		then start looking at  5

		if the last vector was 4
		then start looking at  5

		if the last vector was 5
		then start looking at  7

		if the last vector was 6
		then start looking at  7

		if the last vector was 7
		then start looking at  1	***/

	if (last_dir & 0x01)
	{
		/***	last dir is odd --
			add 2 to it		***/

		new_dir = last_dir + 2;
	}
	else
	{
		/***	last dir is even --
			add 1 to it		***/

		new_dir = last_dir + 1;
	}

	/***	keep new_dir in the range 0 through 7	***/

	if (new_dir > 7)
		new_dir -= 8;

	/***	probe the neighbors, looking for one on the edge	***/

	for (n = 0; n < 8; n++)
	{
		if (probe(x, y, new_dir, new_x, new_y))
		{
			/***	found the next clockwise edge neighbor --
				its coordinates have already been
				stuffed into new_x, new_y		***/

			return(new_dir);
		}
		else
		{
			/***	check the next clockwise neighbor	***/

			if (--new_dir < 0)
				new_dir += 8;
		}
	}
	/***	should never exit routine by this way!	***/
}

/*****************************************************************************
*
*	build()
*
*	Builds a Freeman chain of vectors describing the edge of the
*	contour with elevation 'elev'.  Always follows the contour
*	in a clockwise direction.  Uses 'start_x, start_y' as tentative
*	starting point; modifies them to hold coordinates of first point
*	in chain.
*
*	Returns 0 if unsuccessful.
*	Returns 1 if   successful.
*
*****************************************************************************/

short
build()

{
int		x;
int		y;
int		new_x;
int		new_y;
int		dir;
int		last_dir;
Vector *	new;
Vector *	prev;

	/***	go left in the starting row until out of the contour	***/

	while (in_cont(start_x, start_y))
	{
		--start_x;
	}

	/***	move back right one point, to the leftmost edge
		in the contour, in that row			***/

	start_x++;

	/***	create the head of the chain	***/

	chain = (Vector *)NULL;
	prev = (Vector *)NULL;

	/***	check if the starting point
		has no neighbors in the contour --
		the starting direction to check is arbitrary	***/

	x = start_x;
	y = start_y;

	dir = 0;

	for ( ; ; )
	{
		if (probe(x, y, dir, &new_x, &new_y))
		{
			/***	found a neighbor in that direction
				(its coordinates are in new_x, new_y
				but we don't use them here)		***/

			break;
		}

		/***	try next direction	***/

		if (++dir == 8)
		{
			/***	starting point has no neighbors --
				make the chain one vector long	***/
			
			/***	allocate storage for the vector	***/

			if ( (chain = (Vector *)malloc(sizeof(Vector))) == NULL)
			{
				printf("Insufficient memory available.\n");
				return(0);
			}

			/***	fill in the vector --
				the direction is arbitrary,
				since the point is isolated	***/

			chain->dir = 0;
			chain->next = (Vector *)NULL;

			return(1);
		}
	}

	/***	get ready to follow the edge --
		since we are at the left edge,
		force initial probe to be to upper left
		by initializing last_dir to 1		***/

	last_dir = 1;

	/***	follow the edge clockwise, building a Freeman chain	***/

	for ( ; ; )
	{
		/***	get the next point on the edge
			and the vector to it		***/

		dir = neighbor(x, y, last_dir, &new_x, &new_y);

		/***	maybe done with contour ***/

		if ( (x == start_x) && (y == start_y) && (chain != NULL) )
			if (dir == chain->dir)
				return(1);

		/***	allocate storage for the new vector	***/

		if ( (new = (Vector *)malloc(sizeof(Vector))) == NULL)
		{
			printf("Insufficient memory available.\n");
			return(0);
		}

		/***	fill in the new vector	***/

		new->dir = dir;
		new->next = (Vector *)NULL;

		if (prev)
		{
			/***	add it to the existing chain	***/

			prev->next = new;
		}
		else
		{
			/***	it is the first vector in the chain	***/

			chain = new;
		}

		/***	else get ready to continue following the edge	***/

		prev = new;
		x = new_x;
		y = new_y;
		last_dir = dir;
	}
}

/*****************************************************************************
*
*	report()
*
*	Follows the Freeman chain of vectors describing the edge of the
*	contour with elevation 'elev'.  Reports the elevation, start point,
*	direction vectors, and the number of vectors in the chain.
*
*****************************************************************************/

void
report()

{
Vector *	p;
int		n;

	printf("Elevation = %d\n", elev);
	printf("Start point (x, y) = %d, %d\n", start_x, start_y);

	p = chain;
	n = 0;

	while (p)
	{
		printf("%d\n", p->dir);
		p = p->next;
		++n;
	}

	if (n > 1)
		printf("%d vectors in the chain.\n", n);
	else
		printf("1 vector in the chain.\n");
}

/*****************************************************************************
*
*	main()
*
*	Describes the outline of an elevation contour in the sampled data.
*
*	Returns  0 if   successful.
*	Returns -1 if unsuccessful.
*
*****************************************************************************/

int
main()

{
	/***	get the elevation of the contour to follow
		and get a starting point within the contour --

		they are given explicitly in this example, but
		in a real application the user would provide them,
		or they would be found algorithmically			***/

	elev = 200;
	start_x = 3;
	start_y = 2;

	/***	follow the edge of the contour,
		building a Freeman chain of vectors		***/

	if (build())
	{
		/***	report the results	***/

		report();
		return(0);
	}
	else
	{
		/***	failed	***/

		return(-1);
	}
}

