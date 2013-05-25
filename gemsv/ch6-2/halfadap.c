/*==========================================================================*
 * Halftoning using Space Filling Curve with adaptive clustering and        *
 * selective precipitation                                                  *
 *                                                                          *
 * Limitation:                                                              *
 * Only process image with size 2^n x 2^n where n is positive integer.      *
 *==========================================================================*/
#include <stdio.h>
#include <stdlib.h>

unsigned char **path;	   /* space filling curve path */
/*
 * path[] is a global array storing the information to move along
 *        the space filling curve.
 * genspacefill() is a function to generate the information in path[].
 *        This function is implemented based on a gem in Graphics Gems II,
 *        Ken Musgrave, "A Peano Curve Generation Algorithm".
 * move() is a macro to move along the space filling curve using the
 *        the information stored in path[].
 */
extern genspacefill();

#define TRUE    1
#define FALSE   0
#define BLACK   255
#define WHITE   0
#define LEFT    0
#define RIGHT   1
#define UP      2
#define DOWN    3
#define END     255
#define move(x,y)  switch (path[x][y])          \
                   {                            \
                     case UP:   y++; break;     \
                     case DOWN: y--; break;     \
                     case LEFT: x--; break;     \
                     case RIGHT:x++; break;     \
                   }

/*
 * Description of parameters:
 *   picture,         2D array holding the grayscale image.
 *   out,             2D array holding the dithered image.
 *   maxclustersize,  Max cluster size, N.
 *   thresh,          Edge detection threshold T.
 *   do_sp,           Flag to switch on/off selective precipitation.
 *                    To switch off the selective precipitation,
 *                    set do_sp = FALSE.
 *   do_ac,           Flag to switch on/off adaptive clustering.
 *                    To switch off the adaptive clustering, set do_ac=FALSE
 */
void spacefilterwindow(int **picture, int **out, int maxclustersize,
                       int thresh, char do_sp, char do_ac)
{
  char edge;             /* Flag indicate sudden change detected */
  char ending; 	         /* flag indicates end of space filling curve */
  int accumulator;       /* Accumulate gray value */
  int currclustersize;   /* Record size of current cluster */
  int frontx, fronty;    /* Pointer to the front of the cluster */
  int windowx, windowy;  /* Pointer to first pixel applied with filter */
  int clusterx, clustery;/* Pointer to first pixel in current cluster */
  int windowlen;         /* Size of the moving window */
  int winsum;            /* Current moving window's sum */
  int maxsum;            /* Maximum moving window's sum recorded */
  int rightplace;        /* Position of the moving window with max sum */
  int *cluster;          /* An array hold the pixel of current cluster */
  int last, i, tempx, tempy, currx, curry;    /* temp variables */
  long filter[7] = {-1, -5, 0, 13, 0, -5, -1};  /* 1D -ve Lap. Gauss. filter */
  long convolution;      /* Convolution value in this turn */
  long lastconvolution;  /* Convolution value in last turn */
  /*
   * Description of the pointer along the space filling curve.
   *
   * clusterx,                          windowx,    currx,     frontx,
   * clustery                           windowy     curry      fronty
   *    |                                  |          |           |
   *    v                                  v          v           v
   *   +----------------------------------------------------------+
   *   |                        Cluster                           |
   *   +----------------------------------------------------------+
   *                                       |                      |
   *                                       |                      |
   *                                       |          /\          |
   *                                       |        /    \        |
   *                                       |___   /        \   ___|
   *                                       |    \/           \/   |
   *                                  -ve Laplacian of Gaussian Filter
   */

  if ((cluster=(int*)malloc(sizeof(int)*maxclustersize))==NULL)
  {
    fprintf(stderr,"not enough memory for cluster\n");
    return;
  }
  genspacefill();    /* generates the spacefilling path */
  
  convolution=0;
  currclustersize=0;
  accumulator=0;
  for (frontx=0, fronty=0, i=0 ; i<7 ; i++)
  {
    if (i<3)
    {
      cluster[currclustersize] = picture[frontx][fronty];
      accumulator += cluster[currclustersize];
      currclustersize++;
    }
    if (i==3)
    {  currx = frontx;  curry = fronty;  }
    convolution += filter[i]*(long)(picture[frontx][fronty]);
    move(frontx,fronty);  /* assume the image at least has 7 pixels */
  }
  lastconvolution = convolution;
  clusterx=0;   clustery=0;
  windowx=0;    windowy=0;
  edge=FALSE;
  ending=FALSE;

  while (TRUE)
  {
    if (do_ac) /* switch on/off adaptive clustering */
    {
      /* do convolution */
      convolution = 0;
      for (tempx=windowx, tempy=windowy, i=0 ; i<7 ; i++)
      {
        convolution += filter[i]*picture[tempx][tempy];
        move(tempx,tempy);
      }

      /* detect sudden change */
      if ( (convolution >= 0 && lastconvolution <=0 
            && abs(convolution-lastconvolution)>thresh)
         ||(convolution <= 0  && lastconvolution >=0 
            && abs(convolution-lastconvolution)>thresh)) 
        edge=TRUE; /* force output dots */
    }    

    /* Output dots if necessary */
    if (edge || currclustersize >= maxclustersize || ending)
    {
      edge=FALSE;
      
      /* Search the best position within cluster to precipitate */
      rightplace = 0;
      if (do_sp) /* switch on/off selective precipitation */
      {
        windowlen = accumulator/BLACK;
        winsum = 0;
        for (i=0; i<windowlen; i++)
          winsum += cluster[i];
        for (maxsum=winsum, last=0; i<currclustersize; i++, last++)
        {
          winsum+= cluster[i] - cluster[last];
          if (winsum > maxsum)
          {
            rightplace=last+1;
            maxsum=winsum;
          }
        }
      }

      /* Output dots */
      for (i=0 ; currclustersize!=0 ; currclustersize--, i++)
      {
        if (accumulator>=BLACK && i>=rightplace)  /* precipitates */
        {
          out[clusterx][clustery]=BLACK;
          accumulator-=BLACK;
        }
        else
          out[clusterx][clustery]=WHITE;
        move(clusterx,clustery)
      } /* for */

      if (ending)
        break;
    } /* if */

    cluster[currclustersize] = picture[currx][curry];
    accumulator += cluster[currclustersize];
    currclustersize++;
    if (path[currx][curry]==END)
      ending = TRUE;
    move(currx,curry);
    move(windowx,windowy);
    move(frontx,fronty);
  } /* while */
}
