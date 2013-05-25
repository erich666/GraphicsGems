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
 * x11.c - Show images on X11 Window Systems.
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Sat Mar 14 1992
 * Copyright (c) 1992, Raul Rivero
 *
 */

#include "lug.h"
#include "lugfnts.h"


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#define BORDER_WIDTH    0

Display         *dpy;
Window          wnd;
GC              gc;
Colormap        Xcmap;
int             screen;

show_bitmap_x11( name, inbitmap )
char *name;
bitmap_hdr *inbitmap;
{
  int colors, levels, depth;
  bitmap_hdr outbitmap;
  bitmap_hdr *bitmap;
  int cmapref[256], cmap_flag[256];
  XSetWindowAttributes wattr;
  long wmask;
  XSizeHints whints;
  XEvent event;
  XExposeEvent *exp;
  XComposeStatus statusio;
  int totalsize = inbitmap->xsize * inbitmap->ysize;
  byte *ptr, *iptr, *end;
  XImage *Ximagebuffer;
  byte *Xbitmap;
  int loop;
  double gamma = 1.0;
  int used_colors;
  Cursor normal_pointer, wait_pointer;

  /* Connect with X11 */
  if ( (dpy = (Display *) XOpenDisplay(getenv("DISPLAY")) )== NULL )
    error( 15 );

  /* Get some information */
  screen = DefaultScreen( dpy );
  gc = DefaultGC( dpy, screen );
  depth = DisplayPlanes( dpy, screen );
  Xcmap = DefaultColormap( dpy, screen );
  XSetBackground( dpy, gc, BlackPixel( dpy, screen ) );
  XSetForeground( dpy, gc, WhitePixel( dpy, screen ) );

  /*
   * We know # of colors of X11 display so we need adjust
   * our # of colors to this.
   */
  if ( inbitmap->depth > depth ) {
    colors = 1 << depth;
    levels = compute_levels( colors );
    used_colors = Xdither_image( inbitmap, &outbitmap,
                                 levels, gamma, cmap_flag );
    bitmap = &outbitmap;
  }else {
    bitmap = inbitmap;
    used_colors = set_cmap_flags( inbitmap, cmap_flag );
  }

  /*
   * We have a color map and we wanna use it but preserving the
   * current X11 cmap.
   */
  setXcmap( bitmap, cmapref, cmap_flag );

  /*
   * We wanna predefine ...
   */
  whints.flags = PSize | PMinSize | PMaxSize;
  whints.x = whints.y = 1;
  whints.width = whints.min_width = whints.max_width = bitmap->xsize;
  whints.height = whints.min_height = whints.max_height = bitmap->ysize;
  wattr.event_mask = ExposureMask | ButtonPressMask;
  wattr.border_pixel = WhitePixel( dpy, screen );
  wattr.background_pixel = BlackPixel( dpy, screen );
  wmask = CWBackPixel | CWBorderPixel | CWEventMask;
  normal_pointer = XCreateFontCursor( dpy, XC_crosshair );
  wait_pointer   = XCreateFontCursor( dpy, XC_coffee_mug );

  /*
   * We have all what we wanna, so ... DO IT !!!
   */
  wnd = XCreateWindow( dpy, RootWindow( dpy, screen ),
                       0, 0, bitmap->xsize, bitmap->ysize,
                       BORDER_WIDTH, depth,
                       InputOutput,
                       CopyFromParent,
                       wmask,
                       &wattr );
  XStoreName( dpy, wnd, name );
  XSetNormalHints( dpy, wnd, &whints );
  XDefineCursor( dpy, wnd, wait_pointer);
  /* XSetInput( dpy, wnd, ExposureMask | ButtonPressMask ); */

  /*
   * Allocate memory for the image and copy it with new
   * references ( to X11 colormap ).
   */
  ptr = Xbitmap = (byte *) Malloc( totalsize );
  iptr = bitmap->r;
  end = iptr + totalsize;
  while ( iptr < end ) {
    *ptr++ = cmapref[ *iptr++ ];
  }
  Ximagebuffer = XCreateImage( dpy, DefaultVisual( dpy, screen ), depth,
                               ZPixmap, 0, Xbitmap,
                               bitmap->xsize, bitmap->ysize,
                               8, 0 );
  XMapWindow( dpy, wnd );

  /*
   * We left a process controlling the window.
   */
  XDefineCursor( dpy, wnd, normal_pointer );
  loop = 1;
  do {
    XNextEvent( dpy, &event );
    switch( event.type ) {
        case Expose    :  XDefineCursor( dpy, wnd, wait_pointer );
                          exp = (XExposeEvent *) &event;
                          XPutImage( dpy, wnd, gc,
                                     Ximagebuffer,
                                     exp->x, exp->y,
                                     exp->x, exp->y,
                                     exp->width, exp->height );
                          XDefineCursor( dpy, wnd, normal_pointer );
                          break;
        case ButtonPress: loop = ( event.xbutton.button != Button3 );
                          break;
    }
  }while( loop );

  /* End of this window's life ( ooopps ! ) */
  XUndefineCursor( dpy, wnd );
  XUnmapWindow( dpy, wnd );
  XDestroyImage( Ximagebuffer );
  XDestroyWindow( dpy, wnd );
  XCloseDisplay( dpy );

}

setXcmap( image, cmapref, cmap_flag )
bitmap_hdr *image;
int *cmapref, *cmap_flag;
{
  register int i, j;
  static XColor Xnewcmap[256];
  int Error = 0;
  color_map *cmap;
  int r, g, b;

  cmap = (color_map *) image->cmap;
  bzero( cmapref, image->colors * sizeof(int) );

  /*
   * Lets go to get our colors!.
   */
  for ( i = 0, j = 0; i < image->colors; i++ ) {
    if ( cmap_flag[i] ) {
      /* This color is used */
      Xnewcmap[j].red   = cmap[i][0] << 8;
      Xnewcmap[j].green = cmap[i][1] << 8;
      Xnewcmap[j].blue  = cmap[i][2] << 8;
      Xnewcmap[j].flags = DoRed | DoGreen | DoBlue;
      if ( XAllocColor( dpy, Xcmap, &Xnewcmap[j] ) ) {
        /* Alloc acepted */
        cmapref[i] = Xnewcmap[j].pixel;
        j++;
      }else Error = 1;
    }
  }

  if ( Error ) {
    /*
     * We got errors allocating colors, then we get the current
     * X11 cmap and aproximate those colors to nearest.
     */
    XColor *Xoldcmap;
    int num_colors;
    double distance, cd;
    int r, g, b;
    double *hue, *saturation, *level;
    double h, s, l;

    /* Get the server's number of colors */
    num_colors = DisplayCells( dpy, screen );
    /* Now get the current cmap of X11 */
    Xoldcmap = (XColor *) Malloc( sizeof(XColor) * num_colors );
    for ( i = 0; i < num_colors; i++ ) {
      Xoldcmap[i].pixel = i;
    }
    XQueryColors( dpy, Xcmap, Xoldcmap, num_colors );
    /* 
     * Ok, now we wanna the Hue value of this colors.
     */
    hue = (double *) Malloc( sizeof(double) * num_colors );
    saturation = (double *) Malloc( sizeof(double) * num_colors );
    level = (double *) Malloc( sizeof(double) * num_colors );
    for ( i = 0; i < num_colors; i++ ) {
      /* We need values scaled [0..255] */
      r = Xoldcmap[i].red   >> 8; 
      g = Xoldcmap[i].green >> 8; 
      b = Xoldcmap[i].blue  >> 8; 
      RGB_to_HSL( r, g, b, &hue[i], &saturation[i], &level[i] );
    }
    /*
     * All do it!. Lets go choose new colors.
     */  
    for ( i = 0; i < image->colors; i++ ) {
      if ( cmap_flag[i] && !cmapref[i] ) {
        /*
         * We got an error with this color, it's used by image
         * but we cannot allocated.
         */
        /* Reset the distance value ... */
        distance = 30.;  
        /* ... and convert the current color to HSL */
        RGB_to_HSL( cmap[i][0], cmap[i][1], cmap[i][2], &h, &s, &l );
        for ( j = 0; j < num_colors; j++ ) {
          cd = LUGABS( h - hue[j] ) + 
               LUGABS( s - saturation[j] ) +
               LUGABS( l - level[j] );
          if ( cd < distance ) {
            distance = cd;
            cmapref[i] = j;
          }
        }
      }
    }
    free( Xoldcmap ); 
    free( hue ); 
    free( saturation ); 
    free( level ); 
  }
}

set_cmap_flags( bitmap, cmap_flag )
bitmap_hdr *bitmap;
int *cmap_flag;
{
  register int used_colors = 0;
  register int totalcolors = bitmap->colors;
  byte *end, *ptr;

  if ( bitmap->depth > 8 )
    error( 10 );

  bzero( cmap_flag, totalcolors * sizeof(int) );

  /* Set pointers */
  ptr = bitmap->r;
  end = ptr + bitmap->xsize * bitmap->ysize;

  /*
   * Loop while not the end and we don't use
   * all posible colors.
   */
  while ( ptr < end && used_colors != totalcolors ) {
    if ( !cmap_flag[ *ptr ] ) {
      cmap_flag[*ptr]++;
      used_colors++;
    }
    ptr++;
  }

  return used_colors;
}

#define DMAP(v,x,y)             (modN[v] > dithermatrix[x][y] ? \
                                 divN[v] + 1 : divN[v])

Xdither_image(inbitmap, outbitmap, levels, gamma, cmap_flag)
bitmap_hdr *inbitmap;
bitmap_hdr *outbitmap;
int levels;
double gamma;
int *cmap_flag;
{
  register int i, j;
  int total_size;
  byte *r, *g, *b;
  byte *rout;
  int divN[256], modN[256];
  int dithermatrix[16][16];
  int col, row;
  int used_colors = 0;

  if ( levels < 2 || levels > 6 )
    error( 11 );

  if ( inbitmap->depth <= 8 )
    error( 10 );

  /* Fill new header */
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

  bzero( cmap_flag, 256 * sizeof(int) );
  /*
   * Init the cmap of the dithered image ( and other
   * parameters ).
   */
  create_dithermap( levels, gamma, outbitmap->cmap, divN, modN, dithermatrix);

  /*
   * Dither each pixels through the dither matrix.
   */
  for ( i = 0; i < outbitmap->ysize; i++ ) {
    row = i % 16;
    for ( j = 0; j < outbitmap->xsize; j++ ) {
      col = j % 16;
      *rout = DMAP(*r, col, row) + DMAP(*g, col, row) * levels +
              DMAP(*b, col, row) * levels*levels;
      if ( !cmap_flag[ *rout++ ]++ )
        used_colors++;
      r++, g++, b++;
    }
  }

  return used_colors;
}

