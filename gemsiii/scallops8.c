/*----------------------   main() - test_scallops.c   ------------------------
* This main program tests the scallop (i.e. an envelope of circles) generating
*  function by drawing filled circles with a "fill_circle" function and then
*  using an "erode" function to reduce them to their outlines or scallops.
*  It allocates a small frame buffer and asks the user to choose the number
*  of circles desired, their centers, and their radii.  The circles are drawn
*  and filled in sequence.  A simple "printf" to the standard output displays
*  the upper left corner of the frame buffer.  The "erode" function is then
*  applied to the frame buffer, leaving only the outlines of the filled
*  circles (scallops) with their interiors refilled by an erosion replacement
*  value.  Again the upper left corner of the frame buffer is displayed with
*  the results of the eroding the filled circles to scallops.
* When the erosion replacement value is the same as the background, only the
*  outlines (the value used for circle filling) will be shown.
*
*	Author:		Eric Furman
*			General Dynamics/ Convair Division
*			San Diego, California
*/

#include <stdio.h>

#define  XMIN		0		/* limits of the frame buffer */
#define  XMAX		63
#define  YMIN		0
#define  YMAX		63
#define  STRIDE		64	 /* width of frame buffer (XMAX - XMIN + 1) */

#define  MIN(j,k)  (j)<(k) ? (j) : (k)
#define  MAX(j,k)  (j)>(k) ? (j) : (k)

unsigned char  *Pt_Frame;	  /* Global pointer to start of frame buffer */


main()
{
  int  xc, yc, r, ny, nc, k, j;
  unsigned char  val_fill, val_erode;
  void  fill_circle(), erode();


/* Values (i.e. VLT index) for filling the circles and erosion replacement. */
  val_fill = 8;
  val_erode = 1;

/* Allocate space initialized to zero for the test frame buffer:  */
  ny = YMAX - YMIN + 1;			/* number of rasters */
  Pt_Frame = (unsigned char *)calloc(STRIDE * ny, sizeof(unsigned char));

/* Obtain user input and loop through circles filling each one. */
  printf(" Number of circles: ");
  scanf("%d", &nc);				/* try:  3 */
   for(k=0; k<nc; k++) {
    printf(" center (x,y) and radius:  ");
    scanf("%d%d%d", &xc, &yc, &r);	   /* try:  5 5 6,  15 12 7,  28 6 5 */

/* Check for total clipping before filling circle:  */
     if(xc+r < XMIN || xc-r > XMAX || yc+r < YMIN || yc-r > YMAX) {
      printf("Circle x,y,r:  %d  %d  %d  total clip.\n", xc, yc, r);
     }
     else {
      fill_circle(xc, yc, r, val_fill);
     }
   }			/* end for loop through circles */

/* Print the Upper Left corner of the buffer after filling circles:  */
  printf("UL corner of filled buffer.\n");
   for(k=0; k<23; k++) {
    printf("%2d: ",k);
     for(j=0; j<35; j++) {
      printf("%2d", *(Pt_Frame + j + k * STRIDE));
     }
    printf("\n");
   }

  erode(val_fill, val_erode);		/* erode the filled circles */

/* Print the Upper Left corner of the buffer after eroding filled circles:  */
  printf("UL corner of eroded buffer.\n");
   for(k=0; k<23; k++) {
    printf("%2d: ",k);
     for(j=0; j<35; j++) {
      printf("%2d", *(Pt_Frame + j + k * STRIDE));
     }
    printf("\n");
   }

  free((char*) Pt_Frame);		/* free the frame buffer memory */
}
/*---------------------------   fill_circle()   ----------------------------
* A midpoint circle generating/filling algorithm using second order partial
*   differences to compute cartesian increments.  Given a circles center
*   point (xc,yc), its radius (r), and a fill value (value);  this will
*   draw the filled circle.
*/

void fill_circle(xc, yc, r, value)
  int  xc, yc, r;				/* center & radius */
  unsigned char  value;				/* filling value */
{
  int x, y, d, de, dse ;
  void  raster_fill();

  x = 0;
  y = r;			/* initialization */
  d = 1 - r;
  de = 3;
  dse = -2 * r + 5;
   while(y >= x)  {	/* thru 2nd octant, others handled in raster_fill() */
     if(d < 0) {		/* only move +x in octant 2 */
      d += de;
      dse += 2;
      raster_fill(xc, yc, y, x, value);     /* rasters in octants 1-4 & 5-8 */
     }
     else {			/* move +x and -y in octant 2 */
      d += dse;
      dse += 4;
      raster_fill(xc, yc, y, x, value);     /* rasters in octants 1-4 & 5-8 */
      raster_fill(xc, yc, x, y, value);     /* rasters in octants 2-3 & 6-7 */
      y --;
     }
    de += 2;
    x++;
   }					/* end while(y >= x) */
}
/*------------------------   raster_fill()   ---------------------------------
* Rasters filling for octants 2 to 3 and 6 to 7.  x and y will be reversed in
*   the calling sequence for octants 1 to 4 and 5 to 8.
* Requires the global pointer to a frame buffer (Pt_Frame), the defined
*   parameters for the frame extents (XMIN, XMAX, YMIN, YMAX), and the defined
*   parameter for the frame width (STRIDE).
* Called by the fill_circle function.	
*/

void raster_fill(xc, yc, x, y, value)
  int  xc, yc, x, y;
  unsigned char  value;
{
  int  xl, xr, yt, yb;
  register int  k;
  register unsigned char  *ptb;

  xr = MAX(xc + x, XMIN);    		 /* raster(s) x limits */
  xl = MIN(xc - x, XMAX);

/* if raster segment on frame in x, get y values of top and bottom raster */
   if(xr >= xl) {
    yt = yc + y;
    yb = yc - y;

/* if raster segment on frame in y, point to its start and fill segment */
     if(yt <= YMAX && yt >= YMIN) {
      ptb = Pt_Frame + yt * STRIDE + xl;
       for(k=xl; k<=xr; k++)  *ptb++ = value;
     }
     if(yb != yt && yb <= YMAX && yb >= YMIN) { /* if on & not same raster */
      ptb = Pt_Frame + yb * STRIDE + xl;
       for(k=xl; k<=xr; k++)  *ptb++ = value;
     }
   }				/* end of if(xr >= xl) */
}
/*--------------------------   erode()   ------------------------------------
* Erode the filled circles to their envelope (scallops) only.  Erases existing
*   val_filled pixels to val_erode when all 4 nearest neighbor pixels are also 
*   set to val_filled.
* Requires the value used to fill the circles (val_filled) and a value to
*   which their interiors will be eroded (val_erode).
* Also requires the global pointer to a frame buffer (Pt_Frame), the defined
*   parameters for the frame extents (XMIN, XMAX, YMIN, YMAX), and the defined
*   parameter for the frame width (STRIDE).
*/

void erode(val_filled, val_erode)
  unsigned char  val_filled, val_erode;    /* filled and erode to values */
{
  int  k, j;
  unsigned char  *buf0, *buf1, *buf2, *b0, *b1, *b2;
  int  n;
  unsigned char  val4, *ptl, *ptr, *ptb, *ptt, *ptc, *ptf;

  val4 = val_filled * 4;	    /* for nearest neighbor sum test below */

/* Allocate space for working raster buffers were erosion test/evaluation will
*  be performed.  These include a one pixel border beyond the frame buffer. */
  buf0 = b0 = (unsigned char*)malloc((STRIDE+2) * sizeof(unsigned char));
  buf1 = b1 = (unsigned char*)malloc((STRIDE+2) * sizeof(unsigned char));
  buf2 = b2 = (unsigned char*)malloc((STRIDE+2) * sizeof(unsigned char));
  ptt = buf0;
  ptc = buf1;		    /* set center, top, and bottom buffer pointers */
  ptb = buf2;

/* Initial buffer values to drawing value for one pixel border at top: */
   for(k=0; k<STRIDE+2; k++)  *ptt++ = *ptc++ = *ptb++ = val_filled;

/* Initialize first working raster buffer from the Frame buffer */
  ptf = Pt_Frame;
  ptb = buf2 + 1;
   for(k=0; k<STRIDE; k++)  *ptb++ = *ptf++;

/* Working through the Frame buffer raster by raster with line counter n:  */
   for(k=YMIN, n=0; k<=YMAX; k++, n++) {
    ptf = buf0;
    buf0 = buf1;
    buf1 = buf2;		/* roll pointers for circular buffering */
    buf2 = ptf;
    ptb = buf2 + 1;				/* "new" buffer line to fill */
     if(k == YMAX) {		     /* if below frame, use filled values */
      for(j=0; j<STRIDE+2; j++)  *ptb++ = val_filled;
     }
     else {		     /* else on frame buffer, so load from frame */
      ptf = Pt_Frame + (n+1) * STRIDE;
      for(j=0; j<STRIDE; j++)  *ptb++ = *ptf++;      /* add next raster */
     }

/* Doing erosion by moving across the three raster buffers by pointers to
* the nearest four neighbors, the center pixel, and the output raster:  */
    ptl = buf1;					/* left of test point */
    ptr = buf1 + 2;				/* right */
    ptt = buf0 + 1;				/* top */
    ptb = buf2 + 1;				/* bottom */
    ptc = buf1 + 1;				/* center */
    ptf = Pt_Frame + n * STRIDE;		/* output */
     for(j=0; j<STRIDE; j++)  {	    /* across buffers pixel by pixel */
       if(*ptc++ == val_filled) {	    /* is center pixel value filled */
         if((*ptl + *ptr + *ptt + *ptb) == val4)  *ptf = val_erode;
       }
      ptl++;  ptr++;   ptt++;   ptb++;   ptf++;     /* increment pointers */
     }				   /* end for j loop across raster buffers */
   }				/* end for k loop thru frame buffer rasters */

  free((char*)b0);
  free((char*)b1);
  free((char*)b2);
}
/*------------------------------------------------------------------------*/
