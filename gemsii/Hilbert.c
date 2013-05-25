
#include <gl/gl.h>     /* SGI Graphics Library assumed */

#define STEP_SIZE 4    /* # of pixels in each step */

long coord[2];         /* X,Y for graphics calls */

void step(long angle)
{
   while (angle > 270) angle -= 360;    /* Fold ANGLE to be 0, 90, 180, 270 */
   while (angle <   0) angle += 360;
   if      (angle == 0)   coord[0] += STEP_SIZE;  /* +X */
   else if (angle == 90)  coord[1] += STEP_SIZE;  /* +Y */
   else if (angle == 180) coord[0] -= STEP_SIZE;  /* -X */
   else if (angle == 270) coord[1] -= STEP_SIZE;  /* -Y */
   v2i(coord);                          /* Draw (poly)line to new X,Y = coord */
}

/* Recursive Hilbert-curve generation algorithm                    */
/* ORIENT is either +1 or -1...it swaps left turns and right turns */
/* ANGLE is some multiple of 90 degrees...positive or negative     */
/* LEVEL is the recursion level                                    */
/* 2^LEVEL by 2^LEVEL points will be visited in total              */

void hilbert (orient,angle,level)
long orient,*angle,level;
{
   if (level-- <= 0) return;
   *angle += orient * 90;
   hilbert(-orient,angle,level);
   step(*angle);
   *angle -= orient * 90;
   hilbert(orient,angle,level);
   step(*angle);
   hilbert(orient,angle,level);
   *angle -= orient * 90;
   step(*angle);
   hilbert(-orient,angle,level);
   *angle += orient * 90;
}

/* Recursive Peano-curve generation  */
/* Same parameters as Hilbert above  */
/* 3^LEVEL by 3^LEVEL points visited */

void peano (orient,angle,level)
long orient,*angle,level;
{
   if (level-- <= 0) return;
   peano(orient,angle,level);
   step(*angle);
   peano(-orient,angle,level);
   step(*angle);
   peano(orient,angle,level);
   *angle -= orient * 90;
   step(*angle);
   *angle -= orient * 90;
   peano(-orient,angle,level);
   step(*angle);
   peano(orient,angle,level);
   step(*angle);
   peano(-orient,angle,level);
   *angle += orient * 90;
   step(*angle);
   *angle += orient * 90;
   peano(orient,angle,level);
   step(*angle);
   peano(-orient,angle,level);
   step(*angle);
   peano(orient,angle,level);
}


void main()
{
long initial_angle;

/* Set up window on screen for 24-bit drawing */
/* This presumes SGI graphics library         */
   prefposition(192,1088,236,788);  
   foreground();
   winopen("Hilbert and Peano curves");
   RGBmode();
   gconfig();
   cpack(0x00701030);  /* Background = indigo */ 
   clear();
   cpack(0x00FFFFFF);  /* Curve = white */

/* Start polyline near bottom left corner */
   bgnline();
   coord[0] = 20;
   coord[1] = 20;
   v2i(coord);

/* Visit 128x128 points along Hilbert curve using STEP_SIZE steps, */
/* so pattern will fill 512x512 area on screen since STEP_SIZE = 4 */
   initial_angle = 0;
   hilbert(1,&initial_angle,7);

/* Start polyline to right of other curve */
   bgnline();
   coord[0] = 552;
   coord[1] = 20;
   v2i(coord);

/* Visit 81x81 points along Peano curve using STEP_SIZE steps,     */
/* so pattern will fill 324x324 area on screen since STEP_SIZE = 4 */
   initial_angle = 0;
   peano(-1,&initial_angle,4);

/* All done...admire it for 10 seconds */
   endline();
   sleep(10);
}




