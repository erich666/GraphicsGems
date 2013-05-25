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
 * hsl.c - convert rgb images to hsl images.
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Sat Feb 1 1992
 * Copyright (c) 1992, Raul Rivero
 *
 */
/*
 * RGB <--> HSL routines extracted from Graphics Gems I.
 */

#include "lug.h"
#include "lugfnts.h"

hsl_to_rgb_buffer( h, s, l, r, g, b, size )
double *h, *s, *l;
byte *r, *g, *b;
int size;
{
  while ( size-- ) {
    HSL_to_RGB( *h, *s, *l, r, g, b );
    h++, s++, l++;
    r++, g++, b++;
  }
}

rgb_to_hsl_buffer( r, g, b, h, s, l, size )
double *h, *s, *l;
byte *r, *g, *b;
int size;
{
  while ( size-- ) {
    RGB_to_HSL(*r, *g, *b, h, s, l );
    h++, s++, l++;
    r++, g++, b++;
  }
}

RGB_to_HSL( r1, g1, b1, h, s, l)
byte r1, g1, b1;
double *h, *s, *l;
{
  double v;
  double m;
  double vm;
  double r2, g2, b2;
  double r, g, b;

  r = ((double)r1) / 255.;
  g = ((double)g1) / 255.;
  b = ((double)b1) / 255.;

  v = LUGMAX( r, LUGMAX( g, b ) );
  m = LUGMIN( r, LUGMIN( g, b ) );

  if ((*l = (m + v) / 2.0) <= 0.0)
    return;
  if ((*s = vm = v - m) > 0.0) {
    *s /= (*l <= 0.5) ? (v + m ) : (2.0 - v - m);
  } else return;

  r2 = (v - r) / vm;
  g2 = (v - g) / vm;
  b2 = (v - b) / vm;

  if (r == v)
    *h = (g == m ? 5.0 + b2 : 1.0 - g2);
  else if (g == v)
         *h = (b == m ? 1.0 + r2 : 3.0 - b2);
       else
         *h = (r == m ? 3.0 + g2 : 5.0 - r2);

  *h /= 6;
}

HSL_to_RGB( h, sl, l, r, g, b)
double h, sl,l;
byte *r, *g, *b;
{
  double v;

  v = (l <= 0.5) ? (l * (1.0 + sl)) : (l + sl - l * sl);
  if (v <= 0) {
    *r = *g = *b = 0;
  }else {
    double m;
    double sv;
    int sextant;
    double fract, vsf, mid1, mid2;

    m = l + l - v;
    sv = (v - m ) / v;
    h *= 6.0;
    sextant = h;
    fract = h - sextant;
    vsf = v * sv * fract;
    mid1 = 255 * (m + vsf);
    mid2 = 255 * (v - vsf);
    v *= 255; m *= 255;
    switch (sextant) {
      case 0:
                *r = CORRECT(v); 
                *g = CORRECT(mid1); 
                *b = CORRECT(m); 
                break;
      case 1:
                *r = CORRECT(mid2); 
                *g = CORRECT(v); 
                *b = CORRECT(m); 
                break;
      case 2:
                *r = CORRECT(m); 
                *g = CORRECT(v); 
                *b = CORRECT(mid1); 
                break;
      case 3:
                *r = CORRECT(m); 
                *g = CORRECT(mid2); 
                *b = CORRECT(v); 
                break;
      case 4:
                *r = CORRECT(mid1); 
                *g = CORRECT(m); 
                *b = CORRECT(v); 
                break;
      case 5:
                *r = CORRECT(v); 
                *g = CORRECT(m); 
                *b = CORRECT(mid2); 
                break;
    }
  }
}
