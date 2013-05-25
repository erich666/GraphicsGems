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
 * dither.c - dithering on images.
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Tue Feb 11 1992
 * Copyright (c) 1992, Raul Rivero
 *
 */

#include "lug.h"
#include "lugfnts.h"

#define DMAP(v,x,y)             (modN[v] > dithermatrix[x][y] ? \
                                 divN[v] + 1 : divN[v])

void create_dithermap();
void create_square();

dither_image(inbitmap, outbitmap, levels, gamma)
bitmap_hdr *inbitmap;
bitmap_hdr *outbitmap;
int levels;
double gamma;
{
  register int i, j;
  int total_size;
  byte *r, *g, *b;
  byte *rout;
  int divN[256], modN[256];
  int dithermatrix[16][16];
  int col, row;

  if ( inbitmap->magic != LUGUSED )
    error( 19 );

  if ( levels < 2 || levels > 6 )
    error( 11 );

  if ( inbitmap->depth <= 8 )
    error( 10 );

  /* Fill new header */
  outbitmap->magic = LUGUSED;
  outbitmap->xsize = inbitmap->xsize;
  outbitmap->ysize = inbitmap->ysize;
  total_size = outbitmap->xsize * outbitmap->ysize;
  outbitmap->depth = no_bits( levels*levels*levels ) + 1;
  outbitmap->colors = ( 1 << outbitmap->depth );
  outbitmap->cmap = (byte *) Malloc( 3 * outbitmap->colors );
  rout = outbitmap->r = (byte *) Malloc( total_size );

  /* Pointers to in image */
  r = inbitmap->r;
  g = inbitmap->g;
  b = inbitmap->b;

  /*
   * Init the cmap of the dithered image ( and other
   * parameters ).
   */
  create_dithermap( levels, gamma, (color_map *) outbitmap->cmap, divN, modN, dithermatrix);

  /*
   * Dither each pixels across the dither matrix.
   */
  for ( i = 0; i < outbitmap->ysize; i++ ) {
    row = i % 16;
    for ( j = 0; j < outbitmap->xsize; j++ ) {
      col = j % 16;
      *rout++ = DMAP(*r, col, row) + DMAP(*g, col, row) * levels +
                DMAP(*b, col, row) * levels*levels;
      r++, g++, b++;
    }
  }
}

/*
 * Functions for RGB color dithering.
 *
 * Author:      Spencer W. Thomas
 *              Computer Science Dept.
 *              University of Utah
 * Date:        Mon Feb  2 1987
 * Copyright (c) 1987, University of Utah
 *
 * From Graphics Gems II and Utah Raster Toolkit.
 *
 * The functions name have been changed ( dithermap --> create_... and
 * make_square --> create_square ) due to problems with the RLE header
 * definitions.
 *
 */

static int magic4x4[4][4] =  {
         0, 14,  3, 13,
        11,  5,  8,  6,
        12,  2, 15,  1,
         7,  9,  4, 10
};

void
create_dithermap( levels, gamma, rgbmap, divN, modN, magic )
int levels;
double gamma;
byte rgbmap[][3];
int divN[256];
int modN[256];
int magic[16][16];
{
  double N;
  register int i;
  int levelsq, levelsc;
  byte gammamap[256];

  for ( i = 0; i < 256; i++ ) 
    gammamap[i] = (byte) (.5 + 255. * pow( i / 255., 1./gamma )); 

  levelsq = levels*levels;      /* squared */
  levelsc = levels*levelsq;     /* and cubed */

  N = 255. / (levels - 1.);    /* Get size of each step */

  /*
   * Set up the color map entries.
   */
  for(i = 0; i < levelsc; i++) {
    rgbmap[i][0] = gammamap[(int)(.5 + (i%levels) * N)];
    rgbmap[i][1] = gammamap[(int)(.5 + ((i/levels)%levels) * N)];
    rgbmap[i][2] = gammamap[(int)(.5 + ((i/levelsq)%levels) * N)];
  }

  create_square( N, divN, modN, magic );
}

void
create_square( N, divN, modN, magic )
double N;
int divN[256];
int modN[256];
int magic[16][16] ;
{
  register int i, j, k, l;
  double magicfact;

  for ( i = 0; i < 256; i++ ) {
    divN[i] = (int)(i / N);
    modN[i] = i - (int)(N * divN[i]);
  }
  modN[255] = 0;                /* always */

  /*
   * Expand 4x4 dither pattern to 16x16.  4x4 leaves obvious patterning,
   * and doesn't give us full intensity range (only 17 sublevels).
   *
   * magicfact is (N - 1)/16 so that we get numbers in the matrix from 0 to
   * N - 1: mod N gives numbers in 0 to N - 1, don't ever want all
   * pixels incremented to the next level (this is reserved for the
   * pixel value with mod N == 0 at the next level).
   */
  magicfact = (N - 1) / 16.;
  for ( i = 0; i < 4; i++ )
    for ( j = 0; j < 4; j++ )
      for ( k = 0; k < 4; k++ )
        for ( l = 0; l < 4; l++ )
          magic[4*k+i][4*l+j] =
                        (int)(0.5 + magic4x4[i][j] * magicfact +
                        (magic4x4[k][l] / 16.) * magicfact);
}
