/*  FILENAME: AATables.c  [revised 18 AUG 90]

    DESCRIPTION:  Initialization of lookup tables and frame buffer
      for anti-aliased line rendering demo.

    LINK WITH:
      AALines.h -- Shared variables and symbols for renderer.
      AAMain.c -- Calling routine for renderer.
      AALines.c -- Anti-aliased line rendering code.
*/

#include <math.h>
#include "AALines.h"

/* programs in this file */
extern void Anti_Init();
static void Sqrt_Init();

/* globals defined here */
char *fbuff;
UFX *sqrtfunc=0;
int sqrtcells=1024;
int sqrtshift;

/* AA sizes */
float line_r=1.0;     /* line radius */
float pix_r=SQRT_2;   /* pixel radius */
FX *coverage=0;
int covercells=128;
int covershift;




/*  ************************************
    *                                  *
    *          Anti_Init               *
    *                                  *
    ************************************

    DESCRIPTION:  Initialize everything for the anti-aliased line
      renderer in 'AALines.c':  allocate the frame buffer,
      set up lookup tables, etc.

      For hints about initializing the coverage table, see 
      "Area of Intersection: Circle and a Thick Line" on pages
      40-42 of _Graphics_Gems_.
*/

#define FLOAT_TO_CELL(flt)  ((int) ((flt) * 255.0 + 0.5))
#define MAXVAL_CELL         255

void Anti_Init ( )
{
int *thiscell;
double maxdist,nowdist,incdist;
int tablebits,radbits;
int tablecells;
static int tablesize=0;
double fnear,ffar,fcover;
double half,invR,invpiRsq,invpi,Rsq;
double sum_r;
double inv_log_2;
extern FX Pmax;

/* alloc & init frame buffer */
fbuff = (char *) malloc( xpix*ypix );
  {
  register int i;
  for ( i=xpix*ypix-1; i>=0; --i )  fbuff[i] = 0;
  }

/* init */
inv_log_2 = 1.0 / log( 2.0 );
sum_r = line_r + pix_r;
tablebits = (int) ( log((double)covercells) * inv_log_2 + 0.99 );
radbits = (int) ( log((double)sum_r) * inv_log_2 ) + 1;
covershift = FX_FRACBITS - (tablebits-radbits);

/* constants */
half = 0.5;
invR = 1.0 / pix_r;
invpi = 1.0 / PI;
invpiRsq = invpi * invR * invR;
Rsq = pix_r * pix_r;
#define FRACCOVER(d) (half - d*sqrt(Rsq-d*d)*invpiRsq - invpi*asin(d*invR))

/* allocate table */
Pmax = FLOAT_TO_FX(sum_r);
Pmax >>= covershift;
tablecells = Pmax + 2;
Pmax <<= covershift;
if ( coverage  &&  tablecells > tablesize )
  { free( coverage ); coverage = 0;  tablesize = 0; }
if ( coverage == 0 )
  {
  coverage = (FX *) malloc( tablecells * sizeof(int) );
  tablesize = tablecells;
  }

/* init for fill loops */
nowdist = 0.0;
thiscell = coverage;
incdist = sum_r / (double)(tablecells-2);

/* fill fat portion */
if ( pix_r <= line_r )
  {
  maxdist = line_r - pix_r;
  for ( ;
      nowdist <= maxdist;
      nowdist += incdist,  ++thiscell
      )
    {
    *thiscell = MAXVAL_CELL;
    }
  }

/* fill skinny portion */
else
  {
  /* loop till edge of line, or end of skinny, whichever comes first */
  maxdist = pix_r - line_r;
  if ( maxdist > line_r )
    maxdist = line_r;
  for ( ;
      nowdist < maxdist;
      nowdist += incdist,  ++thiscell
      )
    {
    fnear = line_r - nowdist;
    ffar = line_r + nowdist;
    fcover = 1.0 - FRACCOVER(fnear) - FRACCOVER(ffar);
    *thiscell = FLOAT_TO_CELL(fcover);
    }

  /* loop till end of skinny -- only run on super-skinny */
  maxdist = pix_r - line_r;
  for ( ;
      nowdist < maxdist;
      nowdist += incdist,  ++thiscell
      )
    {
    fnear = nowdist - line_r;
    ffar = nowdist + line_r;
    fcover = FRACCOVER(fnear) - FRACCOVER(ffar);
    *thiscell = FLOAT_TO_CELL(fcover);
    }
  }

/* loop till edge of line */
maxdist = line_r;
for ( ;
    nowdist < maxdist;
    nowdist += incdist,  ++thiscell
    )
  {
  fnear = line_r - nowdist;
  fcover = 1.0 - FRACCOVER(fnear);
  *thiscell = FLOAT_TO_CELL(fcover);
  }

/* loop till max separation */
maxdist = line_r + pix_r;
for ( ;
    nowdist < maxdist;
    nowdist += incdist,  ++thiscell
    )
  {
  fnear = nowdist - line_r;
  fcover = FRACCOVER(fnear);
  *thiscell = FLOAT_TO_CELL(fcover);
  }

/* finish off table */
*thiscell = FLOAT_TO_CELL(0.0);
coverage[tablecells-1] = FLOAT_TO_CELL(0.0);

Sqrt_Init();
}




/*  *******************************
    *                             *
    *       Sqrt_Init             *
    *                             *
    *******************************

    DESCRIPTION:  Initialize the lookup table for the function
      sqrt(1/(1+x^2))).  The table takes a shifted fixed-point
      value as an index and returns a fixed-point value.  Input
      values are in the range [0,1] inclusive.  The number of
      cells in the table is a power of two plus one (the extra
      cell provides an entry for an input value of exactly 1).

    GLOBALS:
      sqrtcells -- Number of cells to use in the table (must
	be set before calling this routine).  This number is
	rounded up to the nearest power of two (the global
	variable itself is unchanged).
      sqrtshift -- Bits to shift a fixed-point (FX) number
	to generate a table index.
      sqrtfunc -- Lookup table for the function.
*/

static void Sqrt_Init ( )
{
UFX *thiscell;
double nowval,incval;
int tablebits;
int tablecells;
double one;

/* init */
tablebits = (int) ( log((double)sqrtcells) / log(2.0) + 0.999 );
tablecells = (1 << tablebits) + 1;  /* one more that requested */
sqrtshift = FX_FRACBITS - tablebits;
one = 1.0;

/* allocate table */
if ( sqrtfunc )
  free( sqrtfunc );
sqrtfunc = (UFX *) malloc( tablecells * sizeof(int) );

/* init for fill loop */
incval = one / (double)(tablecells-1);  /* a negative power of two */

for (
    nowval = 0.0,      thiscell = sqrtfunc;
    nowval < 1.0;
    nowval += incval,  ++thiscell
    )
  {
  *thiscell = FLOAT_TO_FX( sqrt(one/(one+nowval*nowval)) );
  }

sqrtfunc[tablecells-1] = FLOAT_TO_FX( sqrt(0.5) );
}
