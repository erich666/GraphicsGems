/* A Digital Dissolve Effect
by Mike Morton
from "Graphics Gems", Academic Press, 1990

user must provide copy() function.
*/

/*
 * Code fragment to advance from one element to the next.
 *
 * int reg;				/* current sequence element
 * reg = 1;				/* start in any non-zero state
 * if (reg & 1)				/* is the bottom bit set?
 * 	reg = (reg >>1) ^ MASK;		/* yes: toss out 1 bit; XOR in mask
 * else reg = reg >>1;			/* no: toss out 0 bit 
 */

int randmasks[32];	/* Gotta fill this in yourself. */

dissolve1 (height, width)	/* first version of the dissolve 								/* algorithm */
	int height, width;	/* number of rows, columns */
{
	int pixels, lastnum;	/* number of pixels; */
				/* last pixel's number */
	int regwidth;		/* "width" of sequence generator */
	register long mask;	/* mask to XOR with to*/
					/* create sequence */
	register unsigned long element; 
					/* one element of random sequence */
	register int row, column;
					/* row and column numbers for a pixel */

	  /* Find smallest register which produces enough pixel numbers */
	 pixels = height * width; /* compute number of pixels */
							/* to dissolve */
	 lastnum = pixels-1;	/* find last element (they go 0..lastnum) */
	 regwidth = bitwidth ((unsigned int)lastnum); /* how wide must the */
					/* register be? */
	 mask = randmasks [regwidth];	/* which mask is for that width? */

	 /* Now cycle through all sequence elements. */

	  element = 1;	/* 1st element (could be any nonzero) */


	  do {
	    row = element / width;	/* how many rows down is this pixel? */
	    column = element % width;	/* and how many columns across? */
	    if (row < height)	/* is this seq element in the array? */
	      copy (row, column);	/* yes: copy the (r,c)'th pixel */

	    /* Compute the next sequence element */
	    if (element & 1)		/* is the low bit set? */
	      element = (element >>1)^mask;	/* yes: shift value, */
						/* XOR in mask */
	    else element = (element >>1);	/* no: just shift the value */
	 } while (element != 1);		/* loop until we return  */
						/* to original element */
	 copy (0, 0);		/* kludge: the loop doesn't produce (0,0) */
}						/* end of dissolve1() */



int bitwidth (N)	/* find "bit-width" needed to represent N */
	unsigned int N;	/* number to compute the width of */
{
	 int width = 0;	/* initially, no bits needed to represent N */
	 while (N != 0) {	/* loop 'til N has been whittled down to 0 */
	    N >>= 1;		/* shift N right 1 bit (NB: N is unsigned) */
	    width++;		/* and remember how wide N is */
	  }			/* end of loop shrinking N down to nothing */
	  return (width);	/* return bit positions counted */

}						/* end of bitwidth() */



dissolve2 (height, width)	/* fast version of the dissolve algorithm */
	int height, width;	/* number of rows, columns */
{
	int rwidth, cwidth;	/* bit width for rows, for columns */
	int regwidth;		/* "width" of sequence generator */
	register long mask;	/* mask to XOR with to create sequence */
	register int rowshift;	/* shift distance to get row  */
							/* from element */
	register int colmask; /* mask to extract column from element */
	register unsigned long element; /* one element of random */ 								    /* sequence */
	register int row, column;    /* row and column for one pixel */


	  /* Find the mask to produce all rows and columns. */

	rwidth = bitwidth ((unsigned int)height); /* how many bits needed for height? */
	cwidth = bitwidth ((unsigned int)width);  /* how many bits needed for width? */
	regwidth = rwidth + cwidth; /* how wide must the register be? */
	mask = randmasks [regwidth]; /* which mask is for that width? */

 /* Find values to extract row and col numbers from each element. */
	rowshift = cwidth; /* find dist to shift to get top bits (row) */
	colmask = (1<<cwidth)-1;	/* find mask to extract  */
						/* bottom bits (col) */

	  /* Now cycle through all sequence elements. */

	element = 1;	/* 1st element (could be any nonzero) */
	do {
		row = element >> rowshift; /* find row number for this pixel */
		column = element & colmask; /* and how many columns across? */
		if ((row < height)	/* does element fall in the array? */
			&& (column < width)) /* ...must check row AND column */
		copy (row, column); /* in bounds: copy the (r,c)'th pixel */

	    /* Compute the next sequence element */
		if (element & 1)		/* is the low bit set? */
		element = (element >>1)^mask; /* yes: shift value, /*
						/* XOR in mask */
		else element = (element >>1); /* no: just shift the value */
	} while (element != 1); 	/* loop until we return to */
					/*  original element */

	copy (0, 0);		/* kludge: element never comes up zero */
}					/* end of dissolve2() */
