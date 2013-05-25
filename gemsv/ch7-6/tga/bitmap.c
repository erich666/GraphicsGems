/*
 * This software is copyrighted as noted below.  It may be freely copied,
 * modified, and redistributed, provided that the copyright notice is
 * preserved on all copies.
 *
 * There is no warranty or other guarantee of fitness for this software,
 * it is provided solely "as is".  Bug reports or fixes may be sent
 * to the author, who may or may not act on them as he desires.
 *
 * You may not include this software in a program or other software product
 * without supplying the source, or without informing the end-user that the
 * source is available for no extra charge.
 *
 * If you modify this software, you should include a notice giving the
 * name of the person performing the modification, the date of modification,
 * and the reason for such modification.
 */
/*
 * bitmap.c - manage bitmap memory 'problems'.
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Sat Jan 4 1992
 * Copyright (c) 1992, Raul Rivero
 *
 */

#include "lug.h"
#include "lugfnts.h"

allocatebitmap(image, xsize, ysize, depth, colors)
bitmap_hdr *image;
int xsize, ysize, depth, colors;
{
  int totalsize;

  if ( image->magic == LUGUSED )
    freebitmap( image );

  if ( (totalsize = xsize * ysize) <= 0 )
    error( 12 );           /* an error, a negative size ? */

  /* Fill sizes */
  image->xsize = xsize;
  image->ysize = ysize;
  image->magic = LUGUSED;

  /*
   * If colors are equal to 0 then the real information
   * are stored on detph;
   */

  if ( colors == 0 ) {
    image->depth = depth;
    image->colors = ( 1 << image->depth );
  }else {
    image->colors = colors;
    image->depth = no_bits( colors ) + 1;
  }

  image->r = (byte *) Malloc( totalsize );
  switch ( image->depth ) {
    case  8: image->cmap = (byte *) Malloc( 3 * image->colors );
             break;
    case 24: image->g = (byte *) Malloc( totalsize );
             image->b = (byte *) Malloc( totalsize );
             break;
    default: return 1;          /* an error, unkown depth */
  }

  return 0;
}

freebitmap(image)
bitmap_hdr *image;
{
  if ( image->magic != LUGUSED )
    return 0;

  image->magic = -LUGUSED;
  switch ( image->depth ) {
    case 32:    /* for the future */
    case 24:
        free( image->r );
        free( image->g );
        free( image->b );
        break;
    default:
        if ( image->depth <= 8 ) {
          free( image->r );
          free( image->cmap );
        }else {
          /* Unknown depth */
          error( 10 );
        }
        break;
  }
  return 0;
}

copy_bitmap( inbitmap, outbitmap)
bitmap_hdr *inbitmap, *outbitmap;
{
  int totalsize = inbitmap->xsize * inbitmap->ysize;

  if ( outbitmap->magic == LUGUSED )
    freebitmap( outbitmap );

  if ( inbitmap->magic != LUGUSED )
    error( 19 );

  /*
   * Fill header.
   */
  outbitmap->magic = LUGUSED;
  outbitmap->xsize = inbitmap->xsize;
  outbitmap->ysize = inbitmap->ysize;
  outbitmap->depth = inbitmap->depth;
  outbitmap->colors = inbitmap->colors;

  /*
   * Copy raster information.
   */
  outbitmap->r = (byte *) Malloc( totalsize );
  bcopy( inbitmap->r, outbitmap->r, totalsize );
  if ( outbitmap->depth > 8 ) {
    outbitmap->g = (byte *) Malloc( totalsize );
    bcopy( inbitmap->g, outbitmap->g, totalsize );
    outbitmap->b = (byte *) Malloc( totalsize );
    bcopy( inbitmap->b, outbitmap->b, totalsize );
  }else {
    outbitmap->cmap = (byte *) Malloc( 3 * outbitmap->colors );
    bcopy( inbitmap->cmap, outbitmap->cmap, 3*outbitmap->colors );
  }
}
