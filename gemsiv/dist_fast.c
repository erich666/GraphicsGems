/*
 * C code from the article
 * "Fast Linear Approximations of Euclidean Distance in Higher Dimensions"
 * by Yoshikazu Ohashi, yoshi@cognex.com
 * in "Graphics Gems IV", Academic Press, 1994
 */


/*
 *	2-D Euclidean distance approximation
 *	c1 = 123/128, c2 = 51 /128 and max(e) = 4.0 %
 *
 */

int veclen2 (ix,iy)
     int	ix,iy;
{
  int	t;

  ix= (ix<0 ? -ix : ix);	/* absolute values */
  iy= (iy<0 ? -iy : iy);

  if(ix<iy)			/* swap ix and iy if (ix < iy) */
  {				/* See Wyvill (G1, 436)	       */
    ix=ix^iy;
    iy=ix^iy;
    ix=ix^iy;
  }

  t = iy + (iy>>1);

  return (ix - (ix>>5) - (ix>>7)  + (t>>2) + (t>>6));
}

/*
 *	3-D Euclidean distance approximation
 *	c1 = 15/16 , c2 = c3 = 3/8 and max(e) = 7.7 %
 *
 */

int veclen3 (ix,iy,iz)
     int ix,iy,iz;
{
  int	t;

  ix= (ix<0 ? -ix : ix);	/* absolute values */
  iy= (iy<0 ? -iy : iy);
  iz= (iz<0 ? -iz : iz);

  if(ix<iy)			/* needs only two comparisons */
  {
    ix=ix^iy;
    iy=ix^iy;
    ix=ix^iy;
  }

  if(ix<iz)
  {
    ix=ix^iz;
    iz=ix^iz;
    ix=ix^iz;
  }
  /* now ix is the largest */

  t = iy + iz;

  return (ix - (ix>>4) + (t>>2) + (t>>3));

}
