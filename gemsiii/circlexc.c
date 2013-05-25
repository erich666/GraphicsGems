#include <math.h>
#include <stdlib.h>             /* for qsort */

#define START 0
#define END 1

#define QUAD1  90.0
#define QUAD2 180.0
#define QUAD3 270.0
#define QUAD4 360.0
#define FACTOR 57.29577951

typedef struct {
   float angle;
   int type;
} intsct_st;

int compare(intsct_st *, intsct_st *);  /* used by qsort */

/*
 * clip_circle:
 *   clips a circle with center (Xc,Yc) and radius R to the box
 *   given by clip_bnds.
 *   the function return value indicates the number of segments
 *   after clipping.
 *   the start and end angle values of the visible segments are
 *   returned in ang_st and ang_en.
 */

int clip_circle( float Xc, float Yc,  /* center of the circle */
                 float R,             /* radius of the circle */
                 float clip_bnds[],   /* clip boundary: 
                                         [left, upper, right, lower] */
                 float ang_st[],      /* start angles for visible arcs */
                 float ang_en[] )     /* end angles for visible arcs */
{
   float alpha, beta, gamma, delta;
   float circle_bnds[4];
   int i, n, num_sector; 
   int overlap;
   float d;
   intsct_st intsct[20];
   float prev;

   /*
    * find the bounding box of the circle
    */
   circle_bnds[0] = Xc + R;
   circle_bnds[1] = Yc + R;
   circle_bnds[2] = Xc - R;
   circle_bnds[3] = Yc - R;

   /*
    * do a bounding box check to see if the circle is completely 
    * clipped out
    */
   if (circle_bnds[2] > clip_bnds[0] ||
       circle_bnds[0] < clip_bnds[2] ||
       circle_bnds[3] > clip_bnds[1] ||
       circle_bnds[1] < clip_bnds[3])
      return 0;

   alpha=beta=gamma=delta=0;
   n = 0;

   if( circle_bnds[0] > clip_bnds[0] )
   /*
    * the right boundary is crossed
    */
   {
      d = (float) ( clip_bnds[0] - Xc );
      alpha = (int) FACTOR*acos(d/R);

      intsct[n].angle = 0; intsct[n++].type = START;
      intsct[n].angle = alpha; intsct[n++].type = END;
      intsct[n].angle = QUAD4-alpha; intsct[n++].type = START;
      intsct[n].angle = QUAD4; intsct[n++].type = END;
   }

   if( circle_bnds[1] > clip_bnds[1] )
   /*
    * the upper boundary is crossed
    */
   {
      d = (float) ( clip_bnds[1] - Yc);
      beta = (int) FACTOR*acos(d/R);

      if ( (QUAD1-beta) < 0 )
      {
         intsct[n].angle = QUAD4+QUAD1-beta; intsct[n++].type = START;
         intsct[n].angle = QUAD4; intsct[n++].type = END;
         intsct[n].angle = 0; intsct[n++].type = START;
      }
      else
      {
         intsct[n].angle = QUAD1-beta; intsct[n++].type = START;
      }
      intsct[n].angle = QUAD1+beta; intsct[n++].type = END;
   }

   if( circle_bnds[2] < clip_bnds[2] )
   /*
    * the left boundary is crossed
    */
   {
      d = (float) ( Xc - clip_bnds[2] );
      gamma = (int) FACTOR*acos(d/R);

      intsct[n].angle = QUAD2-gamma; intsct[n++].type = START;
      intsct[n].angle = QUAD2+gamma; intsct[n++].type = END;
   }

   if( circle_bnds[3] < clip_bnds[3] )
   /*
    * the lower boundary is crossed
    */
   {
      d = (float) ( Yc - clip_bnds[3] );
      delta = (int) FACTOR*acos(d/R);

      intsct[n].angle = QUAD3-delta; intsct[n++].type = START;
      if ( (QUAD3+delta) > QUAD4 )
      {
         intsct[n].angle = QUAD4; intsct[n++].type = END;
         intsct[n].angle = 0; intsct[n++].type = START;
         intsct[n].angle = QUAD3+delta-QUAD4; intsct[n++].type = END;
      }
      else
      {
         intsct[n].angle = QUAD3+delta; intsct[n++].type = END;
      }
   }
   /*
    * the complete circle is visible if n = 0
    */
   if (n == 0 )
   {
      ang_st[0] = 0;
      ang_en[0] = QUAD4;
      return 1;
   }
   /*
    * Sort all events in increasing order of angles
    */
   qsort ((void*)intsct, (size_t) n, sizeof(intsct_st), compare);

   /*
    * Extract the visible sectors
    */
   num_sector = 0; overlap = 0; prev = 0;

   for (i=0; i<n; i++)
   {
      if (overlap == 0)
         if (intsct[i].angle > prev)
         {
            ang_st[num_sector] = prev;
            ang_en[num_sector++] = intsct[i].angle;
         }

      if (intsct[i].type == START)
         overlap++;
      else
         overlap--;

      prev = intsct[i].angle;
   }

   if (prev < QUAD4)
   {
      ang_st[num_sector] = prev;
      ang_en[num_sector++] = QUAD4;
   }
   return num_sector;
}

int compare(intsct_st *a, intsct_st *b)
{
   if (a->angle < b->angle)
      return -1;
   else if (a->angle == b->angle)
      return 0;
   else
      return 1;
}
