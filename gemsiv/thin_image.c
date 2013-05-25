/*
 * C code from the article
 * "Efficient Binary Image Thinning using Neighborhood Maps"
 * by Joseph M. Cychosz, 3ksnn64@ecn.purdue.edu
 * in "Graphics Gems IV", Academic Press, 1994
 */

#include <stdio.h>

typedef unsigned char	Pixel;		/* Pixel data type		*/

typedef struct	{			/* Image control structure	*/
	short	Hres;			/*   Horizontal resolution (x)	*/
	short	Vres;			/*   Vertical	resolution (y)	*/
	int	Size;			/*   Image size (bytes)		*/
	Pixel	*i;			/*   Image array		*/
	Pixel	*p[1];			/*   Scanline pointer array	*/
					/*   Pixel (x,y) is given by	*/
					/*   image->p[y][x]		*/
}	Image;


/* ---- ThinImage - Thin binary image. -------------------------------- */
/*									*/
/*	Description:							*/
/*	    Thins the supplied binary image using Rosenfeld's parallel	*/
/*	    thinning algorithm.						*/
/*									*/
/*	On Entry:							*/
/*	    image = Image to thin.					*/
/*									*/
/* -------------------------------------------------------------------- */


				/* Direction masks:			*/
				/*   N	   S	 W     E		*/
static	int	masks[]		= { 0200, 0002, 0040, 0010 };

/*	True if pixel neighbor map indicates the pixel is 8-simple and	*/
/*	not an end point and thus can be deleted.  The neighborhood	*/
/*	map is defined as an integer of bits abcdefghi with a non-zero	*/
/*	bit representing a non-zero pixel.  The bit assignment for the	*/
/*	neighborhood is:						*/
/*									*/
/*				a b c					*/
/*				d e f					*/
/*				g h i					*/

static	unsigned char	delete[512] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

void	ThinImage	(image)

	Image		*image;		/* Image control structure	*/

{
	int		xsize, ysize;	/* Image resolution		*/
	int		x, y;		/* Pixel location		*/
	int		i;		/* Pass index			*/
	int		pc	= 0;	/* Pass count			*/
	int		count	= 1;	/* Deleted pixel count		*/
	int		p, q;		/* Neighborhood maps of adjacent*/
					/* cells			*/
	Pixel		*qb;		/* Neighborhood maps of previous*/
					/* scanline			*/
	int		m;		/* Deletion direction mask	*/

	xsize = image->Hres;
	ysize = image->Vres;
	qb    = (Pixel *) malloc (xsize*sizeof(Pixel));
	qb[xsize-1] = 0;		/* Used for lower-right pixel	*/

	while ( count ) {		/* Scan image while deletions	*/
	    pc++;
	    count = 0;

	    for ( i = 0 ; i < 4 ; i++ ) {

		m = masks[i];

		/* Build initial previous scan buffer.			*/

		p = image->p[0][0] != 0;
		for ( x = 0 ; x < xsize-1 ; x++ )
		    qb[x] = p = ((p<<1)&0006) | (image->p[0][x+1] != 0);

		/* Scan image for pixel deletion candidates.		*/

		for ( y = 0 ; y < ysize-1 ; y++ ) {

		    q = qb[0];
		    p = ((q<<3)&0110) | (image->p[y+1][0] != 0);

		    for ( x = 0 ; x < xsize-1 ; x++ ) {
			q = qb[x];
			p = ((p<<1)&0666) | ((q<<3)&0110) |
			    (image->p[y+1][x+1] != 0);
			qb[x] = p;
			if  ( ((p&m) == 0) && delete[p] ) {
			    count++;
			    image->p[y][x] = 0;
			}
		    }

		    /* Process right edge pixel.			*/

		    p = (p<<1)&0666;
		    if	( (p&m) == 0 && delete[p] ) {
			count++;
			image->p[y][xsize-1] = 0;
		    }
		}

		/* Process bottom scan line.				*/

		for ( x = 0 ; x < xsize ; x++ ) {
		    q = qb[x];
		    p = ((p<<1)&0666) | ((q<<3)&0110);
		    if	( (p&m) == 0 && delete[p] ) {
			count++;
			image->p[ysize-1][x] = 0;
		    }
		}
	    }

	    printf ("ThinImage: pass %d, %d pixels deleted\n", pc, count);
	}

	free (qb);
}
