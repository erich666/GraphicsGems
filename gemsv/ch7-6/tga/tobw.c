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
 * tobw.c - conversions from a color image to a bw image.
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Sat Jan 11 1992
 * Copyright (c) 1992, Raul Rivero
 *
 */

#include "lug.h"
#include "lugfnts.h"

extern int LUGverbose;

tobw(inbitmap, outbitmap)
bitmap_hdr *inbitmap;
bitmap_hdr *outbitmap;
{
  register int i;
  byte *rptr, *gptr, *bptr;
  byte *ptr;
  int totalsize;
  int directcolor;

  if ( inbitmap->magic != LUGUSED )
    error( 19 );

  VPRINTF(stderr, "Converting to b&w\n");
  directcolor = ( inbitmap->depth < 24 ? 0 : 1 );

  /*
   * Fill new header ...
   */
  outbitmap->magic = LUGUSED;
  outbitmap->xsize = inbitmap->xsize;
  outbitmap->ysize = inbitmap->ysize;
  totalsize = outbitmap->xsize * outbitmap->ysize;
  /* One component => r */
  ptr = outbitmap->r = (byte *) Malloc( totalsize );

  if ( !directcolor ) {
    /*
     * If the image has less than 24 planes then we only
     * need convert its palette.
     */
    outbitmap->depth = inbitmap->depth;
    outbitmap->colors = ( 1 << outbitmap->depth );
    outbitmap->cmap = (byte *) cmap_to_bw( inbitmap );
    /* Copy the raster information */
    bcopy( inbitmap->r, outbitmap->r, totalsize );
  }else {
    /*
     * We have a real color image.
     */
    outbitmap->depth = 8;
    outbitmap->colors = ( 1 << outbitmap->depth );
    outbitmap->cmap = (byte *) create_bw_pallete();

    rptr = inbitmap->r;
    gptr = inbitmap->g;
    bptr = inbitmap->b;

    for ( i = 0; i < inbitmap->ysize; i++ ) {
      /* Convert each line */
      rgbtobw( rptr, gptr, bptr, ptr, inbitmap->xsize );
      rptr += inbitmap->xsize;
      gptr += inbitmap->xsize;
      bptr += inbitmap->xsize;
      ptr  += inbitmap->xsize;
    }
  }
}

to_raw_bw(inbitmap, outbitmap)
bitmap_hdr *inbitmap, *outbitmap;
{
  bitmap_hdr bwbitmap;
  int totalsize = inbitmap->xsize * inbitmap->ysize;
  byte *iptr, *optr;
  color_map *level;

  if ( inbitmap->magic != LUGUSED )
    error( 19 );

  if ( isagrayscaled( inbitmap ) ) {
    VPRINTF( stderr, "Preserving the raw b&w\n");
    copy_bitmap( inbitmap, outbitmap );
    return 0;
  }

  VPRINTF( stderr, "Converting to raw b&w\n" );
  /* Convert inbitmap to bw */
  tobw( inbitmap, &bwbitmap );

  /* Fill new header */
  outbitmap->magic = LUGUSED;
  outbitmap->xsize = bwbitmap.xsize;
  outbitmap->ysize = bwbitmap.ysize;
  outbitmap->depth = bwbitmap.depth;
  outbitmap->colors = bwbitmap.colors;

  /* Get memory */
  optr = outbitmap->r = (byte *) Malloc( totalsize );
  outbitmap->cmap = (byte *) create_bw_pallete();
  level = (color_map *) bwbitmap.cmap;

  /*
   * What we wanna is a raster information which could
   * be used to represent the level of intensity.
   */
  for ( iptr = bwbitmap.r; totalsize; totalsize-- ) {
    *optr++ = level[*iptr++][0];
  }

  /* Ok. Free memory */
  freebitmap( &bwbitmap );

  return 0;
}

byte *create_bw_pallete()
{
  register int i;
  byte *buffer = (byte *) Malloc( 3 * 256 );
  byte *ptr;

  /* I'll use a pointer */
  ptr = buffer;
  for (i = 0; i < 256; i++) {
    *ptr++= (byte) i;   /* R */
    *ptr++= (byte) i;   /* G */
    *ptr++= (byte) i;   /* B */
  }

  return buffer;
}

byte *cmap_to_bw( image )
bitmap_hdr *image;
{
  register int i;
  byte r[256];
  byte g[256];
  byte b[256];
  byte *buffer, *ptrbuf;
  byte *ptr;
  byte *position;

  if ( image->depth > 8 )
    error( 11 );

  ptrbuf = buffer = (byte *) Malloc ( 3 * image->colors );
  position = (byte *) Malloc( image->colors );

  /* Split the three components */
  ptr= image->cmap;
  for (i = 0; i < image->colors; i++) {
    r[i] = *ptr++;
    g[i] = *ptr++;
    b[i] = *ptr++;
  }

  /* Convert these buffers to bw ( to position ) */
  rgbtobw( r, g, b, position, image->colors );

  /*
   * Now convert these position to a real cmap.
   */
  ptr = position;
  for (i = 0; i < image->colors; i++, position++) {
    *ptrbuf++ = *position;         /* R */
    *ptrbuf++ = *position;         /* G */
    *ptrbuf++ = *position;         /* B */
  }

  free( position );
  return buffer;
}

isagrayscaled( bitmap )
bitmap_hdr *bitmap;
{
  if ( bitmap->depth > 8 ) {
    /* A true color bitmap is NOT! a gray scaled image */
    return 0;
  }else {
    register i;
    color_map *ptr;

    /* 
     * Check if the cmap defines a gray map and its 
     * correct position.
     */
    for ( i = 0, ptr = (color_map *) bitmap->cmap; i < bitmap->colors; i++ ) {
      if ( i != ptr[i][0] || i != ptr[i][1] || i != ptr[i][2] )
        return 0;       /* ooops!, no gray mapped */
    }
    /* Yeah, we have a gray map image. */
    return 1;
  }
}

rgbtobw( r, g, b, position, size )
byte *r, *g, *b;
byte *position;
int size;
{
  /*
   * It's better use integers ( and need a division ) than
   * uses doubles ( and don't need it ).
   */
  while ( size-- )
    *position++ = (byte) ( (30*(*r++) + 59*(*g++) + 11*(*b++)) / 100);
}
