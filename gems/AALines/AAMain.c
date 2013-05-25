/*  FILENAME:  AAMain.c  [revised 17 AUG 90]

    AUTHOR:  Kelvin Thompson

    DESCRIPTION:  Calling routine for anti-aliased line renderer.
      This routine calls the line renderer to draw a single
      anti-aliased line into a small frame buffer.  The
      routine then dumps the frame buffer to a Utah RLE file
      'anti.rle'.

    LINK WITH:
      utah.h -- Definitions for friendly Utah RLE front end.
      AALines.h -- Shared tables, symbols, etc. for renderer.
      AALines.c -- Rendering code.
      AATables.c -- Table initialization.
*/

#include <stdio.h>
#include <math.h>
#include "AALines.h"
#include "utah.h"



main ( argc, argv )
int argc;
char *argv[];
{
int i;
char *scanptr;
int x1,y1,x2,y2;

/* initialize frame buffer and look-up tables */
Anti_Init();

/* set line endpoints */
x1 =  2;  y1 =  2;
x2 = 25;  y2 = 55;

/* render anti-aliased line to a frame buffer */
Anti_Line( x1,y1, x2,y2 );


/* The code below dumps the frame buffer to a Utah RLE file.
** It should be pretty easy to rewrite so that it dumps to
** any other kind of output file...or straight to a display
** device.  The frame buffer is an array of characters
** starting at 'fbuff' with size 'xpix' by 'ypix'.  */

  {
  /* thanks to A.T. Campbell for the friendly front end */
  UTAH_FILE *picout;
  picout = utah_write_init( "anti.rle", xpix, ypix );
  if ( picout == NULL )
    { perror("anti.rle");  exit(1); }
  for ( i=0; i<ypix; i++ )
    {
    scanptr = &fbuff[i*xpix];
    utah_write_rgb( picout, scanptr, scanptr, scanptr );
    }
  utah_write_close(picout);
  }
}
