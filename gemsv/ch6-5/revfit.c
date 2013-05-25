/*
 *  revfit.c : edge reconstruction and the inverse process.
 */
#include <stdio.h>
#include "revfit.h"

#ifndef abs
#define abs(a)	((a)>=0 ? (a) : -(a) )
#endif

#define HalfSUBPIXRES   (SUBPIXRES/2)
#define ESTABLISHED     127
#define MAXRUN          2000          /* max no of pixel edges in a line */

extern DrawPixelEdge(int x, int y, int V_H); /* a user supplied function */
                                             /* for drawing a PixelEdge  */

/**********************************************************************\
 * typedef's for sub-pixel resolution pixel edges and gradient bounds *
\**********************************************************************/
typedef struct {
   int x1,y1;   /* from (coordinates multiplied by sub-pixel resolution) */
   int x2,y2;   /* to   (coordinates multiplied by sub-pixel resolution) */
} Pedge;

typedef struct {
   int ly,lx;   /* lower limit */
   int uy,ux;   /* upper limit */
} Bound;

#define MidX(e) (((e).x1+(e).x2)/2)     /* midpt coordinates of a Pedge    */
#define MidY(e) (((e).y1+(e).y2)/2)
#define Is_Horizontal(d) (abs(d)==HRZ)  /* a horizontal direction? (1, -1) */
#define Is_Vertical(d)   (abs(d)==VRT)  /* a vertical direction?  (2, -2)  */
#define against(a,b) (!((a)+(b)))       /* whether two directions are opp. */
#define Bound_OK(b) (slopecmp((b).uy,(b).ux,(b).ly,(b).lx))
#define WithinBound(dy,dx,b) (slopecmp((dy),(dx),(b).ly,(b).lx) &&\
                              slopecmp((b).uy,(b).ux,(dy),(dx)))

/***************************************************************************
 *   Get_Pedge(): Returns a pointer to the current Pedge from the list el. *
 *                 The position of the cursor of list is not modified.     *
 *                 Returns NULL if no more edges in the list.              *
 *                 Coordinates multiplied by sub-pixel resolution.         *
 ***************************************************************************/
static Pedge *Get_Pedge(Edgelist el) {
 static Pedge e;
 int dir;
   if (el.current>=el.Nedges) return NULL;
   if (Is_Horizontal(dir=(el.list[el.current].dir))) {
    e.y1=e.y2=el.list[el.current].y*SUBPIXRES + HalfSUBPIXRES;
    e.x1=el.list[el.current].x*SUBPIXRES
          - (dir>0 ? HalfSUBPIXRES : -HalfSUBPIXRES);
    e.x2=e.x1 + (dir>0 ? SUBPIXRES : -SUBPIXRES);
 }
 else {
    e.x1=e.x2=el.list[el.current].x*SUBPIXRES + HalfSUBPIXRES;
    e.y1=el.list[el.current].y*SUBPIXRES
        - (dir>0 ? HalfSUBPIXRES : -HalfSUBPIXRES);
    e.y2=e.y1 + (dir>0 ? SUBPIXRES : -SUBPIXRES);
 }
 return &e;
} /* Get_Pedge() */

/************************************************************************
 *      forward(): Update the cursor of the list to the next edge.      *
 ************************************************************************/
#define forward(el) (((el).current)++)

/*************************************************************************
 *      backward(): Move back the cursor of the list one place so that   *
 *                  the previous edge can be visited again.              *
 *************************************************************************/
#define backward(el) (((el).current)--)

/************************************************\
 *      wayof(): return a direction.            *
\************************************************/
/* the directions no.s are chosen s.t. d1==-d2 if d1,d2 are opp. */
static int wayof(Pedge e)  {
 int d=e.x2-e.x1;
   return d ? d/SUBPIXRES                  /* 1 or -1 for horizontal edge */
            : (e.y2 - e.y1)/HalfSUBPIXRES; /* 2 or -2 for vertical edge   */
} /* wayof() */

/**************************************************************************
 * slopecmp(): True if grad vector of the 1st is on the counter-clockwise *
 *             side of the 2nd one                                        *
 **************************************************************************/
static int slopecmp(int dy1,int dx1, int dy2,int dx2)  {
   return (long)dx2*dy1 > (long)dx1*dy2;
} /* slopecmp() */

/***************************************************************************\
* calcbound(): calc the bounds (the pair of gradient limits) for the Pedge  *
\***************************************************************************/
void calcbound(int dominantdir, Pedge e, int Sx, int Sy,
               Bound* b, IntPoint2 *gradU, IntPoint2 *gradL) {
/* gradU and gradL shall be filled with the gradients just within the limits */
 int dy,dx;
   if (Is_Horizontal(dominantdir)) { /* horizontal dominant direction */
      b->uy = (e.y1+e.y2+SUBPIXRES)/2-Sy;
      b->ux = (e.x1+e.x2)/2          -Sx;
      b->ly = (e.y1+e.y2-SUBPIXRES)/2-Sy;
      gradU->x = gradL->x = b->lx = b->ux;
      gradU->y = b->uy-1; gradL->y = b->ly+1;
   }
   else { /* up or down dominant direction */
      b->uy = (e.y1+e.y2)/2          -Sy;
      b->ux = (e.x1+e.x2+SUBPIXRES)/2-Sx;
      gradU->y = gradL->y = b->ly = b->uy;
      b->lx = (e.x1+e.x2-SUBPIXRES)/2-Sx;
      gradU->x = b->ux-1; gradL->x = b->lx+1;
   }
   if (!Bound_OK(*b)) {    /* swaps the bounds if necessary */
    IntPoint2 p;
      dx=b->ux;    dy=b->uy;
      b->ux=b->lx; b->uy=b->ly;
      b->lx=dx;    b->ly=dy;
      p=*gradU; *gradU=*gradL; *gradL=p;
   }
} /* calcbound() */

/******************************************************************************
 * fitlines() : The reversible straight line edge reconstruction routine      *
 ******************************************************************************/
int fitlines(Edgelist el, boolean Pretest, boolean TryAllEndPts,
             IntPoint2 *lines, int MaxNLine) {
/*----------------------------------------------------------------------------*
 * el          : The supplied list of PixelEdges.
 * Pretest     : 1=perform pre-test on each pixel edge, i.e., stop as soon as
 *                 a valid end pt cannot be found on a pixel edge.
 *               0=Allows stepping back.
 * TryAllEndPts: 1=Try all possible end-pts, 0=Use the one closest to mid-pt.
 * lines[]     : A preallocated array to be filled with end pts of fitted lines
 *               Note: Coordinates of the end pts are multiplied by SUBPIXRES.
 * MaxNLine    : The size of the lines[] array.
 *----------------------------------------------------------------------------*/
 int i,linescount,startp,Nendpt,Nstartpt,NPedges,Nbound;          /* counters */
 int Sx,Sy,Ex,Ey,  Ux,Uy,Lx,Ly,  maindir,trnsvrse,dnow,  ndir,dir[3];
 flag breaktrace, starttrace;                                     /* flags    */
 int currentsave, bestpt, maxlen, bestpt_currentsave, bestpt_Nendpt;
 IntPoint2 startpts[SUBPIXRES],endlist[SUBPIXRES],bestpt_endlist[SUBPIXRES];
 Pedge Pedgehistory[MAXRUN],e,last,*nextp,estartsave,bestpt_last;
 Bound bound[MAXRUN];

 el.current=0;                          /* set cursor to the first edge */
 e = *Get_Pedge(el);                    /* first edge                   */
 Sx = MidX(e);
 Sy = MidY(e);

 if (!TryAllEndPts)  {
   lines[0].x = Sx;                     /* record the 1st starting pt.  */
   lines[0].y = Sy;
   linescount=1;
 }
 else  {
  flag hori = Is_Horizontal(wayof(e));
   Nstartpt=0;
   startpts[0].x = Sx;
   startpts[0].y = Sy;
   for (i=1;i<HalfSUBPIXRES;i++) { /* the list of possible init. starting pts */
     startpts[Nstartpt  ].x =  hori ? Sx-i : Sx;
     startpts[Nstartpt++].y = !hori ? Sy+i : Sy;
     startpts[Nstartpt  ].x =  hori ? Sx-i : Sx;
     startpts[Nstartpt++].y = !hori ? Sy+i : Sy;
   }
   startp=0;  /* counter for the list of possible starting pts (startpts[]) */
   bestpt_currentsave=currentsave=el.current;   /* save these for rewinding */
   estartsave=e;
   maxlen=bestpt=-1;                    /* no best starting pt (bestpt) yet */
   linescount=0;
 } /* if (!TryAllEndPts) .. else .. */

 for (starttrace=TRUE;;) {              /* loop for all PixelEdges */
  if (starttrace) {                     /* beginning of a new line segment  */
   dir[0]=wayof(e);   ndir=1;           /* no.of distinct directions so far */
   starttrace=0;  breaktrace=0;
   Pedgehistory[0]=e;                   /* the first Pedge traced */
   NPedges=1;                           /* reset the counters     */
   Nbound=0; 
  } /* if (starttrace) */

  last=e;
  forward(el);                          /* go on to the next PixelEdge */
  if ((nextp=Get_Pedge(el))!=NULL) {    /* get a new Pedge             */
   Pedgehistory[NPedges++]=*nextp;
   e=*nextp;
   dnow=wayof(e);                       /* direction of the current edge     */
  }

  if (nextp==NULL || ndir==ESTABLISHED){ /* maindir and trnsvrse established */
   Bound b;
   IntPoint2 gradU,gradL;
   flag lowerupdated, upperupdated;

   if (nextp!=NULL) {
    calcbound(maindir,e,Sx,Sy,&b,&gradU,&gradL);

    bound[Nbound]=bound[Nbound-1];

    lowerupdated=upperupdated=FALSE;
    if (slopecmp(bound[Nbound-1].uy,bound[Nbound-1].ux,
                 b.uy,b.ux)) {          /* update the upper limit */
     bound[Nbound].uy=b.uy;
     bound[Nbound].ux=b.ux;
     upperupdated=TRUE;
    }
    if (slopecmp(b.ly,b.lx,
                 bound[Nbound-1].ly,
                 bound[Nbound-1].lx)) { /* update the lower limit */
     bound[Nbound].ly=b.ly;
     bound[Nbound].lx=b.lx;
     lowerupdated=TRUE;
    }
   } /* if (nextp!=NULL) */

   if (nextp==NULL ||                                 /* no more PixelEdge */
       (dnow!=trnsvrse && dnow!=maindir)    ||        /* U-turn            */
       (dnow==trnsvrse && dnow==wayof(last)) ||       /* 2 trnsvrse edges  */
       !Bound_OK(bound[Nbound]) ||                    /* not within limits */
       (Pretest &&   /* if Pretest, check if there is any pt within limits */
        ((lowerupdated && !WithinBound(gradU.y,gradU.x,bound[Nbound])) ||
         (upperupdated && !WithinBound(gradL.y,gradL.x,bound[Nbound]))))) {
              /* now we shall calculate the starting pt for the next trace */
    for (;;) {/* loop until the end-point lies within the gradient limits  */
     int dx,dy,tmp;        flag exact,EndptOK;

     Ex=MidX(last); Ey=MidY(last);
     if (Nbound==0) { /* i.e. first few PixelEdges. therefore mid-pt is ok  */
      if (TryAllEndPts){
       endlist[0].x=Ex; endlist[0].y=Ey;
       Nendpt=1;
      }
      break;          /* end pt found */
     }

     b = bound[Nbound-1];

     dx= Ex - Sx;             /* the slope of the mid-pt of the last Pedge  */
     dy= Ey - Sy;

     if (TryAllEndPts && el.current-currentsave>maxlen) {
     /* find all possible end pts only if length longer than maxlen so far  */
       int h,addy,addx;

       if (abs(maindir)==1) { addy=1; addx=0; } else {addy=0; addx=1;}
       if (WithinBound(dy,dx,b))  {                  /* check mid-pt first  */
        endlist[0].x=Ex; endlist[0].y=Ey; Nendpt=1;
       }
       else Nendpt=0;
       for (h=1; h<SUBPIXRES/2; h++) {               /* offset from mid-pt  */
        if (WithinBound(dy+addy*h,dx+addx*h,b)) {
         endlist[Nendpt  ].x = Ex + addx*h;
         endlist[Nendpt++].y = Ey + addy*h;
        }
        else if (WithinBound(dy-addy*h,dx-addx*h,b)) {
         endlist[Nendpt  ].x = Ex - addx*h;
         endlist[Nendpt++].y = Ey - addy*h;
        }
       } /* for (h) */
       Ex=endlist[0].x; Ey=endlist[0].y;
       EndptOK = Nendpt>0;
     }
     else { /* TryAllEndPts==FALSE. just calc the pt closest to the mid-pt  */
      if (!slopecmp(dy,dx,b.ly,b.lx)) {
       /*
        * dy dx is equal or below the lower limit.
        * i.e. the slope just above the lower limit should be taken.
        * if the lower gradient limit hits exactly on a sub-pixel res point,
        *  the truncation of the integer division has done part of the job.
        */
       if (Is_Horizontal(maindir)) {
        tmp= dx*b.ly;   exact= (dx==0 || tmp%b.lx==0);
        Ey = tmp/b.lx + Sy + (b.lx>0 ? (b.ly>0 ? 1      : exact)
                                     : (b.ly>0 ? -exact : -1   ));
       }
       else {
        tmp= dy*b.lx;   exact= (dy==0 || tmp%b.ly==0);
        Ex = tmp/b.ly + Sx + (b.ly>0 ? (b.lx>0 ? -exact : -1   )
                                     : (b.lx>0 ? 1      : exact));
       }
       EndptOK = Pretest || WithinBound(Ey-Sy,Ex-Sx,b);
      }
      else if (!slopecmp(b.uy,b.ux,dy,dx)) {
       /*
        * dy dx is equal or above the upper limit.
        * i.e. the slope just below the upper limit should be taken.
        * if the upper gradient limit hits exactly on a sub-pixel res point,
        *  the truncation of the integer division has done part of the job.
        */
       if (Is_Horizontal(maindir)) {
        tmp= dx*b.uy;   exact= (tmp%b.ux==0);
        Ey = tmp/b.ux + Sy + (b.ux>0 ? (b.uy>0 ? -exact :-1    )
                                     : (b.uy>0 ? 1      : exact));
       }
       else {
        tmp= dy*b.ux;   exact= (tmp%b.uy==0);
        Ex = tmp/b.uy + Sx + (b.uy>0 ? (b.ux>0 ? 1      : exact)
                                     : (b.ux>0 ? -exact :-1    ));
       }
       EndptOK = Pretest || WithinBound(Ey-Sy,Ex-Sx,b);
      }
      else       /* dy,dx is within the limits. i.e. mid-point is taken. */
      EndptOK=1;
     } /* if (TryAllEndPts)..else.. */

     if (EndptOK) break;    /* if Pretest is TRUE, EndptOK always TRUE */
     else  {    /* no valid end-point can be found, step back one edge */
      backward(el);
      last = Pedgehistory[--NPedges-2];
      Nbound--;
     }
    } /* for (;;) */                    /* until a valid end pt is found */
    breaktrace=TRUE;                    /* one line segment found.       */
   }
   else {                               /* limits not crossed over yet   */
    Nbound++;                           /* one more new valid bound      */
    continue;                           /* continue to get another Pedge */
   } /* if (various trace breaking conditions) */
  } /* if (nextp==NULL || ndir==ESTABLISHED) */
  else {  /* i.e. dominant and trnsvrse direction not yet established */
    breaktrace = FALSE;
    if (ndir<3) {
     for (i=0;i<ndir;i++) {             /* compare with previous dir's   */
      if (against(dnow,dir[i])) {       /* there is a `U' turn ...       */
       breaktrace = TRUE;               /* therefore an early stop       */
       Ex=MidX(last);  Ey=MidY(last);
       if (TryAllEndPts) {
         endlist[0].x=Ex; endlist[0].y=Ey;
         Nendpt=1;
       } /* if (TryAllEndPts) */
      } /* for () */
     }
     if (ndir<2 || dnow!=dir[1] || dir[0]!=dir[1]) {
      dir[ndir]=dnow;
      ndir++;
     }
    }

    if (ndir==3)                /* now we can establish the directions... */
    {                           /*   _       | */
     if (dir[0]!=dir[1]) {      /* _|   or  _| */
      maindir=dir[2];                                             /*    | */
      if (dir[1]==dir[2]) {                                       /*   _| */
       trnsvrse=dir[0];         /* the 1st dir is the trnsvrse dir        */
       if (Is_Horizontal(maindir)) {
        Ux = Lx = MidX(e) - Sx;
        Uy = (Ly = e.y1-Sy-HalfSUBPIXRES) +SUBPIXRES;
       }
       else {
        Uy = Ly = MidY(e) - Sy;
        Ux = (Lx = e.x1-Sx-HalfSUBPIXRES) +SUBPIXRES;
       }
      }
      else {                                                      /*   _  */
       trnsvrse=dir[1];                                           /* _|   */
       if (Is_Horizontal(maindir)) {
        Lx = Ux = MidX(e)-Sx;
        Ly = (Uy = MidY(e)+HalfSUBPIXRES-Sy) -SUBPIXRES;
       }
       else {
        Ly = Uy = MidY(e)-Sy;
        Lx = (Ux = MidX(e)+HalfSUBPIXRES-Sx) -SUBPIXRES;
       }
      }
     }
     else {                                                   /* __.....| */
      maindir=dir[0];
      trnsvrse=dir[2];
      if (Is_Horizontal(maindir)) {
       Lx = e.x1 + (maindir>0 ? -HalfSUBPIXRES : HalfSUBPIXRES) - Sx;
       Ux = Lx + (maindir>0 ? SUBPIXRES : -SUBPIXRES);
       Uy = Ly = MidY(e) - Sy;
      }
      else {
       Ly = e.y1 + (maindir>0 ? -HalfSUBPIXRES : HalfSUBPIXRES) - Sy ;
       Uy = Ly + (maindir>0 ? SUBPIXRES : -SUBPIXRES);
       Ux = Lx = MidX(e) - Sx;
      }
     }
     if (slopecmp(Ly,Lx,Uy,Ux)) {       /* swap the grad limits if necessary */
      bound[0].uy=Ly; bound[0].ux=Lx;   /* Ly Lx larger */
      bound[0].ly=Uy; bound[0].lx=Ux;
     }
     else {
      bound[0].uy=Uy; bound[0].ux=Ux;   /* Uy Ux larger */
      bound[0].ly=Ly; bound[0].lx=Lx;
     }
     Nbound=1;                          /* first bound established */
     ndir = ESTABLISHED;
    } /* if (ndir==3) */
  } /* if (ndir==ESTABLISHED)...else... */
                                                 /*--------------------------*/
  if (breaktrace) {                              /*     one line ended       */
                                                 /*--------------------------*/
   backward(el);    /* last pixel edge shall be the start of another line.   */

   if (TryAllEndPts) {
    if (maxlen < (el.current-currentsave))  {    /* longer than the longest  */
     maxlen = el.current-currentsave;            /* longest distance so far  */
     bestpt_last=last;                           /* save the last edge       */
     bestpt=startp;                              /* update the best pt so far*/
     bestpt_currentsave=el.current;              /* save the cursor for el   */
     for (i=0; i<Nendpt; i++) bestpt_endlist[i]=endlist[i]; /* save end pts  */
     bestpt_Nendpt=Nendpt;                       /* save the no. of end pts  */
    }
    startp++;                            /* next starting pt in startpts[]   */
    if (startp >= Nstartpt) {            /* all starting pts have been tried */
     currentsave=el.current=bestpt_currentsave;     /* save the ending pos   */
     estartsave=e=bestpt_last;                      /* save the ending Pedge */
     lines[linescount++] = startpts[bestpt];        /* record the best pt    */
     if (linescount>=MaxNLine) return -1;           /* too many lines        */
     if (bestpt_currentsave>=el.Nedges-1) {         /* no more Pixel edges ? */
      lines[linescount++]=bestpt_endlist[0];        /* record end pt as well */
      return linescount>=MaxNLine ? -1 : linescount;/* done                  */
     }

     Nstartpt=bestpt_Nendpt;      /* use the list of end pts as starting pts */
     for (i=0; i<bestpt_Nendpt; i++) startpts[i]=bestpt_endlist[i];

     startp=0;                    /* consider the first one in the new list  */
     Sx=startpts[0].x; Sy=startpts[0].y;
     maxlen=bestpt=-1;            /* reset maxlen and bestpt to undefined    */
    }
    else { /* i.e. startp<Nstartpt.  try next starting point                 */
     Sx=startpts[startp].x; Sy=startpts[startp].y; /* next starting pt       */
                                                   /* rewind and start again */
     el.current=currentsave;
     e=last=estartsave;
    } /* if (startp>=Nstartpt) ... else ... */
   }
   else { /* i.e. TryAllEndPts==FALSE.  simply start at the end pt again     */
    Sx=Ex; Sy=Ey; e=last;
    lines[linescount].x=Ex; lines[linescount++].y=Ey;
    if (linescount>=MaxNLine) return -1;           /* too many lines         */
    if (el.current>=el.Nedges-1) return linescount;/* no more Pedges, done   */
   }
   starttrace=TRUE;                                /* start again            */
  } /* if (breaktrace) */
 } /* for (starttrace=TRUE;;) infinite loop */
} /* fitlines() */

/***********************************************************************\
*               T H E    I N V E R S E    P R O C E S S                 *
\***********************************************************************/
#define divisible(a,b) ((a)%(b)==0)
#define ishori(x,y) (divisible(x,SUBPIXRES)||\
                     divisible(y+HalfSUBPIXRES,SUBPIXRES))
#define isvert(x,y) (divisible(y,SUBPIXRES)||\
                     divisible(x+HalfSUBPIXRES,SUBPIXRES))
#define sign(x) ((x)>=0 ? 1 : -1)
#define Trunc(n) ((n)/SUBPIXRES*SUBPIXRES)
static int lastx,lasty,lastdir;            /* to avoid duplicated pixel edges */

static void drawHPedge(int x, int y) {     /* draw a horizontal pixel edge    */
 if (lastx==x && lasty==y && lastdir==HRZ) /* starting edge==last ending edge */
   return;
 lastx=x; lasty=y; lastdir=HRZ;
 DrawPixelEdge(x/SUBPIXRES, y/SUBPIXRES, HRZ);    /* call the user function   */
} /* drawHPedge() */

static void drawVPedge(int x, int y) {     /* draw a vertical pixel edge      */
 if (lastx==x && lasty==y && lastdir==VRT) /* starting edge==last ending edge */
   return;
 lastx=x; lasty=y; lastdir=VRT;
 DrawPixelEdge(x/SUBPIXRES, y/SUBPIXRES, VRT);   /* call the user function    */
} /* drawVPedge() */

/**************************************************************************
 * makejaggedline(): A modified Bresenham's mid-point algorithm. Based on *
 *    the code from the original Graphics Gem.  Neither the starting pt   *
 *    nor the ending pt need to be at the mid-pt of a pixel edge.          *
 *    The decision variable has been scaled by SUBPIXRES and preloaded    *
 *    with the offset from a `proper' starting pt, i.e. the mid-pt of the  *
 *    first pixel edge pointing to the dominant direction.                *
 **************************************************************************/
static makejaggedline(int x1, int y1, int x2, int y2) {
 int d, x, y, ax, ay, sx, sy, dx, dy, finaltrnsvrse;

 dx = x2-x1;  ax = abs(dx)*SUBPIXRES;  sx = sign(dx)*SUBPIXRES;
 dy = y2-y1;  ay = abs(dy)*SUBPIXRES;  sy = sign(dy)*SUBPIXRES;
                                                           /*============*/
 if (ax>ay)                                                /* x dominant */
 {                                                         /*============*/
  if (isvert(x1,y1)) /* 1st edge is trnsvrse. skip to the mid-pt  */
  {                  /* of the next dominant dir edge.            */
   y=Trunc(y1 + HalfSUBPIXRES) + sy/2;
   x=Trunc(x1) + HalfSUBPIXRES + sx/2;
   drawVPedge(x-sx/2,y-sy/2);            /* draw the skipped edge */
  }
  else { /* 1st edge is dominant. shift to the mid-pt */
   x=Trunc(x1 + HalfSUBPIXRES);
   y=Trunc(y1) + HalfSUBPIXRES;
  }
  /* preload decision var `d' with offset x-x1, y-y1. (if any) */
  d = ay - (ax>>1) + ay*(x-x1)/sx - ax*(y-y1)/sy;
  for (;;)   {
   drawHPedge(x,y);
   if (abs(x-x2) < HalfSUBPIXRES) return; /* final edge is a dominant one */
   x += sx;
   finaltrnsvrse = dx>0 ? x>x2: x<x2;
   if (d>0 || finaltrnsvrse)  {        /* if the final edge is a trnsvrse */
    drawVPedge(x-sx/2,y+sy/2);         /* one, draw it before stopping    */
    y += sy;
    d -= ax;
   }
   if (finaltrnsvrse) return;
   d += ay;
  } /* for (;;) */
 }                                                 /*============*/
 else                                              /* y dominant */
 {                                                 /*============*/
  if (ishori(x1,y1)) /* 1st edge trnsvrse. skip to the mid-pt  */
  {                  /* of the next dominant dir edge          */
   x=Trunc(x1 + HalfSUBPIXRES) + sx/2;
   y=Trunc(y1) + HalfSUBPIXRES + sy/2;
   drawHPedge(x-sx/2, y-sy/2);        /* draw the skipped edge */
  }
  else { /* 1st edge is dominant. shift to the mid-pt */
   x=Trunc(x1) + HalfSUBPIXRES;
   y=Trunc(y1 + HalfSUBPIXRES);
  }
  /* preload decision var `d' with offset x-x1, y-y1 (if any) */
  d = ax - (ay>>1) + ax*(y-y1)/sy - ay*(x-x1)/sx;
  for (;;) {
   drawVPedge(x,y);
   if (abs(y-y2) < HalfSUBPIXRES) return; /* final edge is a dominant one */
   y += sy;
   finaltrnsvrse = dy>0 ? y>y2 : y<y2;
   if (d>0 || finaltrnsvrse)  {         /* if the final one is a trnsvrse */
    drawHPedge(x+sx/2, y-sy/2);         /* one, draw it before stopping.  */
    x += sx;
    d -= ay;
   }
   if (finaltrnsvrse) return;
   d += ax;
  } /* for (;;) */
 } /* if (ax>ay)... else ...*/
} /* makejaggedline() */

/***************************************************************************
 * linestojagged(): reconstruct a sequence of pixel edges from given lines *
 *                by calling the makejaggedline() function.                *
 ***************************************************************************/
void linestojagged(int Nlines, IntPoint2 *lines) {
 int from_x, from_y, i;
 lastdir=0;
 for (from_x=lines[0].x, from_y=lines[0].y, i=1; i<Nlines; i++) {
  makejaggedline(from_x,from_y,lines[i].x,lines[i].y);
  from_x=lines[i].x;    from_y=lines[i].y;
 }
} /* linetojagged() */
