/****************************************************************************
  FAST LINEAR COLOR RENDERER
  Russell C.H. Cheng, University of Wales,  Cardiff 21 Jan 1992

  The renderer assumes true 24-bit color with a linear memory frame-buffer
  of 4-byte integers, lowest byte is r, then g, then b, with the top byte
  not used.

  The renderer precalculates and stores the merged 24-bit rgb x-offsets.
  This avoids a separate interpolation for each of the r, g and b variables
  for every pixel.

  Entry:
  For clarity the required input variables are passed to the renderer
  as arguments, Details of each variable are given in the listing.  In a
  general program it will be more compact to hold these variables in a
  structure and pass a pointer to this.

  Exit:
  The renderer outputs directly to the designated frame-buffer.
****************************************************************************/

#define HRES 768  /*  horizontal resolution, adjust this as necessary */
#define VRES 512  /*  vertical resolution, adjust this as necessary   */

void FastLinearRend( xmin,xmax,ymin,ymax, xleft,xright, r0,g0,b0,z0, x0,y0,
             dr_by_dx, dr_by_dy, dg_by_dx, dg_by_dy, db_by_dx, db_by_dy,
             dz_by_dx, dz_by_dy, screen_buffer_ptr, z_buffer_ptr)

int xmin,     /*  The extent of the polygon being rendered:                 */
    xmax,     /*  xmin and xmax give the x-values of the leftmost and       */
    ymin,     /*  rightmost pixels of the polygon, and ymin and ymax the    */
    ymax;     /*  largest and smallest y-values encountered in the polygon  */

int * xleft,   /* Pointers to arrays holding the x-value of the leftmost    */
    * xright;  /* and rightmost pixel occupied by the polygon on each       */
               /* scanline. The values xleft[ymin],..., xleft[ymax], and    */
               /* xright[ymin],..., xright[ymax] should all be set on entry */
               /* The arrays themselves in the main program should be each  */
               /* of size VRES                                              */

float r0,g0,b0; /* Base rgb values of the polygon at the pixel position     */
                /* x0, y0; each in the range 0.0 - 255.0 */

float z0;       /* Base z-distance of the polygon at pixel position x0, y0  */

int x0,y0;      /* Position of base pixel, this need not be a pixel of the */
                /* polygon, but is typically a vertex or the origin (0,0)  */

float  dr_by_dx, dr_by_dy,   /*  r, g, b color and       */
       dg_by_dx, dg_by_dy,   /*  z-distance increments   */
       db_by_dx, db_by_dy,   /*  in the x and y          */
       dz_by_dx, dz_by_dy;   /*  directions              */
  
int *  screen_buffer_ptr;    /* Pointer to the frame buffer base address   */
                             /* The frame buffer is assumed to be linear   */
                             /* memory, comprising VRES horizontal lines   */
                             /* with HRES pixels per line                  */

float *  z_buffer_ptr;       /* Pointer to the z-buffer base address */
                             /* The buffer-size is HRES*VRES         */

{
      float r,g,b,z,         /* current r g b z values */
            r1,g1,b1,z1;     /* r g b z values at position x0, y */

      float dx,dy;           /* offsets of current pixel position */
                             /* from that of base pixel           */

      int x,y;               /* current pixel position            */

      int *  screen_buffer;  /* pointer to current frame-buffer position  */
      float *  z_buffer;     /* pointer to current z-buffer position      */

      int col;               /*  This holds the 24-bit rgb color of the */
                             /*  leftmost pixel of the current scanline */

      int rgboffset[HRES];   /*  This array holds the merged     */
                             /*  24-bit rgb x-direction offsets  */

      /* find r1, g1, b1, z1, the rgbz value at (x0,ymin) */
      dy = (ymin - y0);
      r1 = r0 + dr_by_dy * dy;
      g1 = g0 + dg_by_dy * dy;
      b1 = b0 + db_by_dy * dy;
      z1 = z0 + dz_by_dy * dy;

      /* find the merged 24-bit rgb x-offset values along a scanline  */
      for (x = xmin; x <= xmax; x++) {
          dx = (x - x0);
          r = dr_by_dx * dx;
          g = dg_by_dx * dx;
          b = db_by_dx * dx;
          rgboffset[x] = ((int)r) + (((int)g)<<8) + (((int)b)<<16);
      }
    
      /* now go through each scanline */
      for (y = ymin; y<= ymax; y++)  {
          /* for each scanline, find the 24-bit color value at (x0,y) */
          col = ((int)r1) + (((int)g1)<<8) + (((int)b1)<<16);

          /* and find the z value at (xleft[y],y) */
          dx  = xleft[y] - x0;
          z   = z1 + dz_by_dx * dx;

          /* then render the scanline */
          screen_buffer = screen_buffer_ptr + y*HRES;
          z_buffer = z_buffer_ptr + y*HRES;
          for (x = xleft[y]; x<=xright[y]; x++) {
              if ( z < z_buffer[x] ) {
                   screen_buffer[x] = col + rgboffset[x];
                   z_buffer[x] = z;
              }
              z += dz_by_dx;
          }

          /* and increment the r1 g1 b1 z1 value ready for the next y */
          r1 += dr_by_dy;
          g1 += dg_by_dy;
          b1 += db_by_dy;
          z1 += dz_by_dy;
      }

}
