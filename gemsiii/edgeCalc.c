/******************************************************************************
EDGE AND BIT-MASK CALCULATIONS FOR ANTI-ALIASING
Russell C.H. Cheng, University of Wales, Cardiff, 21 Jan 1992.

This routine calculates the geometry of the overlap of the edges of a polygon
with square pixels, using a fast alternating Bresenham error-update technique.

Entry:
 Pointer to a structure (defined below) giving details of the polygon.
 Pointers to precalculated bitmasks.

Exit:
 The routine outputs the overlap information pixel by pixel via resultproc().
 The arguments are: the pixel position, position of the endpoints of the
 overlapping edge, the pixeltype, and the edgetype.

 In this implementation resultproc() uses this information to calculate the
 positions of the leftmost and rightmost pixels on each scanline, and the
 bitmasks of overlap of the polygon with each pixel.
 
Implementation Details:
 (i)  The procedure resultproc() accesses external pointers: int * xl,
      int * xr,  aabufftype * aabuffptr, short int * fragptr. These are
      described in resultproc(). These should be globally defined.
 (ii) edge_calculate() accesses polygonal information via a pointer
      to the structure:
         typedef struct SCRNPOLYGON
            {
            int numvert;         number of vertices 
            float xcoord[10];    coordinates of each vertex 
            float ycoord[10];    in clockwise order 
            int imax;            subscript of vertex with largest y-coord 
            int imin;            subscript of vertex with smallest y-coord
            } SCRNPOLYGON;
******************************************************************************/

#define LARGE 10000000.0
#define TRUE 1
#define FALSE 0

/*==  The following two structures are included here to enable compilation,
      they should normally be defined earlier for access by the main routine ==*/

typedef struct  {
        unsigned short mask[4][5][5][5][5];
        } aabufftype;

typedef struct  {
        int numvert;      /* number of vertices */
        float xcoord[10]; /* coordinates of each vertex */
        float ycoord[10]; /* in clockwise order */
        int imax;         /* subscript of vertex with largest y-coord */
        int imin;         /* subscript of vertex with smallest y-coord */ 
        } SCRNPOLYGON;


#define NN 0 /* The different types of overlap possible in a pixel. */
#define LX 1 /* See Fig.2 in the main text. */
#define LY 2
#define RX 3
#define RY 4
#define MX 5
#define MY 6
#define SX 7
#define SY 8
#define TX 9
#define TY 10

#define HRES 768 /* horizontal resolution */

int rl[11][5] = {  /* Array used to select pixeltype/edgetype combinations */
 0,  0, 0, 1, -1,  /* at which an xl or xr value should be set.     */
-1,  0, 1, 1, -1,  /*  1 indicates where xr is set  */
 0,  0, 0, 0, -1,  /* -1 indicates where xl is set  */
 1, -1, 0, 1, -1,
 0,  0, 0, 1,  0,
 0, -1, 1, 1, -1,
 0,  0, 0, 0,  0,
 0,  0, 0, 1,  0,
 0, -1, 0, 0,  0,
 0,  0, 0, 0, -1,
 0,  0, 1, 0,  0,
};


void edge_calculator(scrnfaceptr)
SCRNPOLYGON * scrnfaceptr;

{
void resultproc();

float delx,dely,delymp,dx_by_dy,dy_by_dx;
int edgetype;
float ex;                 /* Bresenham x-error variables */
float exend, exstt, ex0;  /*   ''                        */
float ey;                 /* Bresenham y-error variables */
float eyend, eystt, ey0;  /*   ''                        */
int firstleftedgetype;    /* edgetype of first left edge encountered*/
int firstrightedge;       /* flag showing when 1st right-edge is reached */
float hpix = 1.0;         /* Pixel edgelength */
int iv, ivm1, ivp1;       /* work variables holding array subscripts */
float largemp;            /* work variable used in slope calculation */
int lastleftedgetype;     /* edgetype of the last left-edge */
int toggle;               /* switch-flag, 1=odd, 2=even edgetypes */
float xstt,xend;          /* x-positions of start and end points of edge */
int xpix, xpend, xpstt;   /* current, edge-start & edge-end pixel x-coords */ 
float ystt,yend;          /* y-positions of start and end points of edge */
int ypix, ypend, ypstt;   /* current, edge-start & edge-end pixel y-coords */ 

/*====     Process each edge of the left-hand side of polygon      ====*/
iv = scrnfaceptr->imin;               /* Find subscripts of first      */
ivp1 = (iv+1) % scrnfaceptr->numvert; /* left edge and the             */
xend=scrnfaceptr->xcoord[iv];  yend=scrnfaceptr->ycoord[iv]; /* coords */
xpend = (int)xend; ypend = (int)yend;    /* of its starting point.     */
firstleftedgetype = 0;

while(iv!=scrnfaceptr->imax) {
   xpix  = xpend; ypix = ypend;               /* Get the coords of the    */
   xstt= xend; ystt= yend;                    /* starting pixel of edge   */
   xpstt = xpend; ypstt = ypend;              /* and the coords of the    */
   xpend=(int)(xend= scrnfaceptr->xcoord[ivp1]);              /* end      */
   ypend=(int)(yend= scrnfaceptr->ycoord[ivp1]);              /* pixel.   */
   exend= xend-xpend; eyend= yend-ypend; /* Bresenham errors at end pixel.*/

   if(xstt<xend) {                 /*--   Left: Edgetype 1    --*/
      if(firstleftedgetype==0) {   /* If at first edge,         */
         firstleftedgetype = 1;    /* make a note of its type.  */
         resultproc(xpix,ypix, 0.0,0.0,0.0,0.0, LX,0);
               /* Special call equivalent to xl[ypix] = xpix    */
         }
      edgetype=1; toggle = 1;      /* Set edgetype and          */
      goto gamma;                  /* go to main edge-processor.*/
      }
   else {                          /*--   Left: Edgetype 4    --*/
      if(firstleftedgetype==0)     /* If at first edge, make    */
         firstleftedgetype = 4;    /* a note of its type.       */
      edgetype=4; toggle = 2;      /* Set edgetype and          */
      goto gamma;                  /* go to main edge-processor.*/
      }

edge_end_l:   /* The main edge-processor returns here when the edge is done*/
   iv = ivp1;          /* Set subscripts for the next left edge. */
   ivp1= (iv+1) % scrnfaceptr->numvert;  /* '' */
   }

/*===  Make a note of the edgetype of the last edge on the left. ====*/
lastleftedgetype = edgetype; 
          
/*==== Process each edge of the right-hand side of polygon. ====*/
iv = scrnfaceptr->imin;                       /* Find subscripts of first  */
ivm1= (iv+scrnfaceptr->numvert-1) % scrnfaceptr->numvert; /* right edge &  */
xend=scrnfaceptr->xcoord[iv];  yend=scrnfaceptr->ycoord[iv]; /* the coords */
xpend = (int)xend; ypend = (int)yend;             /* of its starting point.*/

firstrightedge = TRUE;

while(iv!=scrnfaceptr->imax) {
   xpix  = xpend; ypix  = ypend;                /* get coords of the       */
   xstt  = xend;    ystt  = yend;               /* starting pixel of edge  */
   xpstt = xpend; ypstt = ypend;                /*        and              */
   xpend=(int)(xend= scrnfaceptr->xcoord[ivm1]);   /* the coords of        */
   ypend=(int)(yend= scrnfaceptr->ycoord[ivm1]);   /* the end pixel.       */
   exend= xend-xpend; eyend= yend-ypend; /* Bresenham errors at end pixel. */

   if(xstt<xend) { /*--- Right: Edgetype 3 ---*/
      /*-- Correct for bottom-most pixel bitmask, if necessary. --*/
      if(firstrightedge == TRUE) {
         firstrightedge = FALSE;
         if(firstleftedgetype==1) {
            exstt=(xstt-xpstt);   eystt=(ystt-ypstt);
            resultproc( xpix,ypix, exstt, eystt, exstt, eystt, MY, 4); 
            }
          }
      edgetype=3; toggle = 1;  /* Set the current edgetype and   */
      goto gamma;              /* go to the main edge-processor. */
      }
   else {     /*--- Right: Edgetype 2 ---*/
      /*-- Correct for bottom-most pixel bitmask, if necessary.  --*/
      if(firstrightedge==TRUE) {
         firstrightedge = FALSE;
         if(firstleftedgetype==4) {
            exstt=(xstt-xpstt);   eystt=(ystt-ypstt);
            resultproc(xpix,ypix, exstt, eystt, exstt, eystt, MY, 3);
            }
            resultproc(xpix,ypix, 0.0,0.0,0.0,0.0, RX,0);
                      /* Special call equivalent to xr[ypix] = xpix. */
         }
      edgetype=2;  toggle = 2;  /* Set the current edgetype. */
      goto gamma;               /* Go to main edge-processor.*/
      }

edge_end_r:   /* The main edge-processor returns here once edge is done */
   iv =  ivm1;         /* set subscripts for next right-edge */
   ivm1= (iv+scrnfaceptr->numvert-1) % scrnfaceptr->numvert;
   }

/*=== do the final vertex bitmask correction, if necessary ===*/
if(lastleftedgetype==4 && edgetype==2)
   resultproc(xpend,ypend, exend, eyend, exend, eyend, MY, 1);
else if(lastleftedgetype==1 && edgetype==3)
   resultproc(xpend,ypend, exend, eyend, exend, eyend, MY, 2);
  
return;

/*=== Routine ends here, extracted code follows ======================

The following segment is the main edge-processor. It identifies the
coords xpix,ypix of each pixel intersected by the edge, and the type
of intersection (see Fig. 2 in the main text). All four edgetypes
are handled using the switches 'edgetype' and 'toggle'

Two Bresenham 'error' quantities are used to identify the pixels:

 ex: the x-distance from the left boundary of the pixel to the
 intercept of the edge with the upper boundary of the pixel.
 
 ey: the y-distance from the right boundary of the pixel to the
 intercept of the edge with the right boundary of the pixel.

Each pixel is found from the previous one by updating either ex or
ey and then testing its value to identify the type of intersection.

ex0, ey0 are initially the values of ex, ey at the starting pixel of
the edge, but subsequently hold the previous values as ex,ey are
updated.

=====================================================================*/

gamma:

/*=== If the edge lies entirely in one pixel, record this and finish. ===*/
if( (xpix==xpend) && (ypix==ypend) ) {
   resultproc(xpix,ypix, xstt-xpstt,ystt-ypstt, exend,eyend, NN, edgetype); 
   switch(edgetype) {
      case 3: case 2: goto edge_end_r;
      case 4: case 1: goto edge_end_l;
      }
   } 

/*=== If not, calculate the slope of the edge. ===*/
delx = xend-xstt; dely = yend-ystt;
if(toggle==1) { /* edgetype 1 or 3 */
   delymp =  dely;  largemp = LARGE;
   }
else { /* edgetype 2 or 4 */
   delymp = -dely;  largemp = -LARGE; 
   }

if(delx!=0)   dy_by_dx=delymp/delx; 
else {
   dx_by_dy = 0;  dy_by_dx = LARGE;
   }
if(dely!=0)   dx_by_dy=delx/dely;
else {
   dy_by_dx = 0; dx_by_dy = largemp;
   }

/*=== Look at first pixel. ===*/
ex0= xstt-xpstt;  ey0= ystt-ypstt; /* Set Bresenham error variables */
ex = ex0 + dx_by_dy*(hpix-ey0);

if(toggle==1) {    /* edgetype 1 or 3 */
   ey = ey0 + dy_by_dx*(hpix-ex0);
   if(ex < hpix) { /* Depending on the size of ex, the pixel is of type LX */
      resultproc(xpix, ypix, ex0, ey0, ex, hpix, LX, edgetype); goto alpha;
      }
   else {          /* or of type LY. */
      resultproc(xpix, ypix, ex0, ey0, hpix, ey, LY, edgetype); goto beta;
      }
   }
else {             /* edgetype 2 or 4 */
   ey = ey0 + dy_by_dx*ex0;
   if(ex > 0) {    /* Depending on the sign of ex, the pixel is of type RX */
      resultproc(xpix, ypix, ex0, ey0, ex, hpix, RX, edgetype); goto alpha;
      }
   else {          /* or of type RY. */
      resultproc(xpix, ypix, ex0, ey0,  0.0, ey, RY, edgetype); goto beta;
      }
   }

alpha: /*== The upper boundary just crossed; now at new y-level ==*/
ypix++;
ey -= hpix;

if(toggle==1) { /* edgetype 1 or 3 */
   /* If at last pixel of edge, finish off edge. */
   if( (ypix >= ypend) && (xpix >= xpend) ) {
      resultproc(xpend,ypend, ex, 0.0, exend, eyend, RX, edgetype);
      if(edgetype==1)   goto edge_end_l;
      else              goto edge_end_r;
      }
   }
else {          /* edgetype 2 or 4 */
   /* If at last pixel of edge, finish off edge. */
   if( (ypix >= ypend) && (xpix <= xpend) ) {
      resultproc(xpend, ypend, ex, 0.0, exend, eyend, LX, edgetype);
      if(edgetype==4)   goto edge_end_l;
      else              goto edge_end_r;
      }
   }

/*=== Not yet at end of edge. Update ex and carry on. ===*/
ex0 = ex;   ex += dx_by_dy;
if(toggle==1) {    /* edgetype 1 or 3 */
   if(ex < hpix) { /* Depending on the size of ex, pixel is of type MX */
      resultproc(xpix, ypix, ex0, 0.0, ex, hpix, MX, edgetype); goto alpha;
      }
   else {          /* or of type SY. */
      resultproc(xpix, ypix, ex0, 0.0, hpix, ey, SY, edgetype); goto beta;
      }
   }
else {             /* edgetype 2 or 4 */
   if(ex > 0) {    /* Depending on the sign of ex, pixel is of type MX */
      resultproc(xpix, ypix, ex0, 0.0, ex, hpix, MX, edgetype); goto alpha;
      }
   else {          /* or of type TY. */
      resultproc(xpix, ypix, ex0, 0.0, 0.0,  ey, TY, edgetype); goto beta;
      }
   }

beta:  /*== Just crossed a vertical pixel boundary; now at new x-level ==*/
if(toggle==1) {    /* edgetype 1 or 3 */
   xpix++;
   ex -= hpix;
   /* If at last pixel of edge, finish off edge. */
   if( (ypix>=ypend) && (xpix>=xpend) ) {
      resultproc(xpend, ypend,  0.0, ey, exend, eyend, RY, edgetype);
      if(edgetype==3)   goto edge_end_r;
      else              goto edge_end_l;
      }
   } 
else {             /* edgetype 2 or 4 */
   xpix--;
   ex += hpix;
   /* If at last pixel of edge, finish off edge. */
   if( (xpix <= xpend) && (ypix >= ypend) ) {
      resultproc(xpend, ypend, hpix, ey, exend, eyend, LY, edgetype);
      if(edgetype==4)   goto edge_end_l;
      else              goto edge_end_r;
      }
   }

/*=== Not yet at end of edge. Update ey, and carry on. ===*/
ey0 = ey;   ey += dy_by_dx;
if(toggle==1) {    /* edgetype 1 or 3 */
   if(ey < hpix) { /* Depending on size of ey, pixel is of type MY */
      resultproc(xpix, ypix,   0.0, ey0, hpix, ey, MY, edgetype); goto beta;
      }
   else {          /* or of type SX. */
      resultproc(xpix, ypix,   0.0, ey0, ex, hpix, SX, edgetype); goto alpha;
      }
    }
else {             /* edgetype 2 or 4 */
   if(ey < hpix) { /* Depending on size of ey, pixel is of type MY */
      resultproc(xpix, ypix,  hpix, ey0,  0.0, ey, MY, edgetype); goto beta;
      }
   else {          /* or of type TX. */
      resultproc(xpix, ypix,  hpix, ey0, ex, hpix, TX, edgetype); goto alpha;
      }
   }
}

/*===========================================================================
This illustrates how the output from the edge calculator can be used
The routine modifies externally defined arrays fragptr[], xl[] and xr[] (which
are accessed via global pointers).
It uses a global pointer to recognize precalculated bitmasks held in a
structure.
===========================================================================*/

void resultproc(xpix,ypix,x1,y1,x2,y2,pixtype,edgetype)
int xpix,ypix;           /* position of pixel to which information applies */
float x1,y1,x2,y2;       /* position of the two endpoints of edge intersecting
                            the polygon */
int pixtype;             /* type of overlap in this pixel (NN,LX,LY,...) */
int edgetype;            /* type of edge (1,2,3 or 4)         */

{
extern aabufftype * aabuffptr;     /* pointer to a structure in which the
            precomputed bitmasks are held. Their format is given in the text.*/
extern unsigned short * fragptr; /* pointer to linear array holding bitmasks
            of the overlaps of the polygon with pixels, assuming HRES
            pixels per scanline. On entry each entry should be set to 0xffff */
extern int * xl, * xr;          /* pointers to arrays storing positions of the
          left and right edges of the polygon: xl[y] and xr[y] give the
          x-position of the leftmost and rightmost pixels of the polygon on
          scanline y */
static float scale = 3.999; /* this should be set to just less than the ratio
            of pixel to subpixel widths. The value 3.999 is for the case where
            each pixel is 4x4 subpixels */
int ij,ix1,ix2,iy1,iy2;

ij  = xpix + HRES*ypix;
ix1 = x1*scale;  iy1 = y1*scale;
ix2 = x2*scale;  iy2 = y2*scale;

/*-- adjust current bitmask by bitwise ANDing it with computed bitmask --*/
if(edgetype>0)
   fragptr[ij] &= aabuffptr->mask[edgetype] [ix1] [iy1] [ix2] [iy2];

/*-- record the leftmost and rightmost pixel positions, xl[y] and xr[y]
     of the polygon on each scanline y --*/
if      (rl[pixtype][edgetype]== 1)  xr[ypix]=xpix;
else if (rl[pixtype][edgetype]==-1)  xl[ypix]=xpix;

return ;
}
