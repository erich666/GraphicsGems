/*  FILENAME: AALines.c  [revised 17 AUG 90]

    AUTHOR:  Kelvin Thompson

    DESCRIPTION:  Code to render an anti-aliased line, from
      "Rendering Anti-Aliased Lines" in _Graphics_Gems_.

      This is almost exactly the code printed on pages 690-693
      of _Graphics_Gems_.  An overview of the code is on pages
      105-106.

    LINK WITH:
      AALines.h -- Shared tables, symbols, etc.
      AAMain.c -- Calling code for this subroutine.
      AATables.c -- Initialize lookup tables.
*/


#include "AALines.h"

#define SWAPVARS(v1,v2) ( temp=v1, v1=v2, v2=temp )

#define FIXMUL(f1,f2) \
  ( \
  (((f1&0x0000ffff) * (f2&0x0000ffff)) >> 16) + \
  ((f1&0xffff0000)>>16) * (f2&0x0000ffff) + \
  ((f2&0xffff0000)>>16) * (f1&0x0000ffff) + \
  ((f1&0xffff0000)>>16) * (f2&0xffff0000) \
  )

/* HARDWARE ASSUMPTIONS:
/*    * 32-bit, signed ints
/*    * 8-bit pixels, with initialized color table
/*    * pixels are memory mapped in a rectangular fashion */

/* FIXED-POINT DATA TYPE */
#ifndef FX_FRACBITS
  typedef int FX;
# define FX_FRACBITS 16  /* bits of fraction in FX format */
# define FX_0        0   /* zero in fixed-point format */
#endif

/* ASSUMED MACROS:
/*   SWAPVARS(v1,v2) -- swaps the contents of two variables
/*   PIXADDR(x,y) -- returns address of pixel at (x,y)
/*   COVERAGE(FXdist) -- lookup macro for pixel coverage 
        given perpendicular distance; takes a fixed-point
        integer and returns an integer in the range [0,255]
/*   SQRTFUNC(FXval) -- lookup macro for sqrt(1/(1+FXval^2))
        accepts and returns fixed-point numbers
/*   FIXMUL(FX1,FX2) -- multiplies two fixed-point numbers
        and returns the product as a fixed-point number   */

/* BLENDING FUNCTION:
/*  'cover' is coverage -- in the range [0,255]
/*  'back' is background color -- in the range [0,255] */
#define BLEND(cover,back) ((((255-(cover))*(back))>>8)+(cover))

/* LINE DIRECTION bits and tables */
#define DIR_STEEP  1  /* set when abs(dy) > abs(dx) */
#define DIR_NEGY   2  /* set whey dy < 0 */


/* pixel increment values 
/*   -- assume PIXINC(dx,dy) is a macro such that:
/*   PIXADDR(x0,y0) + PIXINC(dx,dy) = PIXADDR(x0+dx,y0+dy)  */
static int adj_pixinc[4] = 
      { PIXINC(1,0), PIXINC(0,1), PIXINC(1,0), PIXINC(0,-1) };
static int diag_pixinc[4] = 
      { PIXINC(1,1), PIXINC(1,1), PIXINC(1,-1), PIXINC(1,-1) };
static int orth_pixinc[4] = 
      { PIXINC(0,1), PIXINC(1,0), PIXINC(0,-1), PIXINC(1,0) };

/* Global 'Pmax' is initialized elsewhere.  It is the
   "maximum perpendicular distance" -- the sum of half the
   line width and the effective pixel radius -- in fixed format */
FX Pmax;


/***************  FUNCTION ANTI_LINE  ***************/

void Anti_Line ( X1, Y1, X2, Y2 )
int X1, Y1, X2, Y2;
{
int 	Bvar, 	/* decision variable for Bresenham's */
    	Bainc,   /* adjacent-increment for 'Bvar' */
    	Bdinc;   /* diagonal-increment for 'Bvar' */
FX 		Pmid,  	/* perp distance at Bresenham's pixel */
   		Pnow,  	/* perp distance at current pixel (ortho loop) */
   		Painc, 	/* adjacent-increment for 'Pmid' */
   		Pdinc, 	/* diagonal-increment for 'Pmid' */
   		Poinc; 	/* orthogonal-increment for 'Pnow'--also equals 'k' */
char 	*mid_addr,   /* pixel address for Bresenham's pixel */
     	*now_addr;   /* pixel address for current pixel */
int 	addr_ainc,   /* adjacent pixel address offset */
    	addr_dinc,   /* diagonal pixel address offset */
    	addr_oinc;   /* orthogonal pixel address offset */
int dx,dy,dir;    	/* direction and deltas */
FX slope;			/* slope of line */
int temp;

/* rearrange ordering to force left-to-right */
if 	( X1 > X2 )
  	{ SWAPVARS(X1,X2);  SWAPVARS(Y1,Y2); }

/* init deltas */
dx = X2 - X1;  /* guaranteed non-negative */
dy = Y2 - Y1;


/* calculate direction (slope category) */
dir = 0;
if ( dy < 0 )   { dir |= DIR_NEGY;  dy = -dy; }
if ( dy > dx )  { dir |= DIR_STEEP; SWAPVARS(dx,dy); }

/* init address stuff */
mid_addr = PIXADDR(X1,Y1);
addr_ainc = adj_pixinc[dir];
addr_dinc = diag_pixinc[dir];
addr_oinc = orth_pixinc[dir];

/* perpendicular measures */
slope =  (dy << FX_FRACBITS) / dx;
Poinc = SQRTFUNC( slope );
Painc = FIXMUL( slope, Poinc );
Pdinc = Painc - Poinc;
Pmid = FX_0;

/* init Bresenham's */
Bainc = dy << 1;
Bdinc = (dy-dx) << 1;
Bvar = Bainc - dx;

do
  	{
  	/* do middle pixel */
  	*mid_addr = BLEND( COVERAGE(abs(Pmid)), *mid_addr );

  	/* go up orthogonally */
  	for (
      	Pnow = Poinc-Pmid,  now_addr = mid_addr+addr_oinc;
      	Pnow < Pmax;
      	Pnow += Poinc,      now_addr += addr_oinc
      	)
       *now_addr = BLEND( COVERAGE(Pnow), *now_addr );

  	/* go down orthogonally */
  	for (
      	Pnow = Poinc+Pmid,  now_addr = mid_addr-addr_oinc;
      	Pnow < Pmax;
      	Pnow += Poinc,      now_addr -= addr_oinc
      	)
       *now_addr = BLEND( COVERAGE(Pnow), *now_addr );


  	/* update Bresenham's */
  	if ( Bvar < 0 )
    	{
    	Bvar += Bainc;
    	mid_addr = (char *) ((int)mid_addr + addr_ainc);
    	Pmid += Painc;
    	}
  	else
    	{
    	Bvar += Bdinc;
    	mid_addr = (char *) ((int)mid_addr + addr_dinc);
    	Pmid += Pdinc;
    	}

  	--dx;
  	} while ( dx >= 0 );
}
