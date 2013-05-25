#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "chainCode.h"
#include "pt2.h"

/* DEFINITION OF THE CONSTANTS */

#define CONTOUR 'c'
#define VISITED 'v'
#define BLACK '1'
#define WHITE '0'

/* DEFINITION OF THE MACROS */

#define PIX(a,b) ((b) * f_size.x + (a))
#define PIX2(a,b) ((b) * size->x + (a))
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#define MAX(x,y) ((x)>(y) ? (x) : (y))


/*************************************************************/
/*                                                           */
/* This is the main function. It receives as a parameter a   */
/* bitmap image of size 'size' and outputs a chain code.     */ 
/* The following constraints are placed on the bitmap:       */
/* + Each pixel is encoded as a char.                        */
/* + Only white (0) and black (1) pixels are taken into      */
/*   account.                                                */
/* + The shape to encode should have no holes and should be  */
/*   in a single piece.                                      */
/*                                                           */
/*************************************************************/

chainCode* encode(pt2 *size, char *bitmap)
{
static pt2    contour_dir[8] = {{ 1, 0},
                { 0,-1},
                {-1, 0},
                { 0, 1},
                { 1,-1},
                {-1,-1},
                {-1, 1},
                { 1, 1}};
chainCode *code1,
          code4;
char *fatmap,
     direction_code[8] = {'0','2','4','6','1','3','5','7'};
int i,j,u,v,
    flag,
    d, distance,
    last_dir;
pt2 pixel,
    test_pixel,
    start_pixel,
    f_size,
    bbox[2] = {{INT_MAX, INT_MAX},
               {-INT_MAX, -INT_MAX}};

/* CREATE AN EMPTY CHAIN CODE TO RETURN THE RESULT */
code1 = new chainCode();

/* RESCAN THE BITMAP AT A GREATER RESOLUTION (4x4 GREATER) */
/* ADD TWO BLANK LINES TO THE LEFT, RIGHT, TOP AND BOTTOM  */
/* OF THE FATMAP. THESE COULD BE NECESSARY TO AVOID THE    */
/* CONTOUR TO BE DRAWN OUTSIDE OF THE BOUNDS OF THE MATRIX */

f_size.x = 2 + SCALE*size->x + 2;
f_size.y = 2 + SCALE*size->y + 2;
fatmap = malloc(f_size.x * f_size.y * sizeof(char));
for (i=0; i<f_size.x * f_size.y; i++)
    fatmap[i] = WHITE;
for (j=0; j<size->y; j++)
    for (i=0; i<size->x; i++)
        if (bitmap[PIX2(i,j)] == BLACK)
            for(v=0; v<SCALE; v++)
                for(u=0; u<SCALE; u++)
                    fatmap[PIX(2+4*i+u, 2+4*j+v)] = BLACK;

/* GENERATE THE CONTOUR OF THE BITMAP USING 4 SUCCESSIVE */
/* PASSES: FOR EACH DIRECTION, WE SCAN EACH LINE UNTIL   */
/* WE REACH A BLACK PIXEL: THE PIXEL JUST BEFORE IT IS A */
/* CONTOUR PIXEL                                         */

/* PASS 1: LEFTWARDS */
for (j=0; j<f_size.y; j++)
    for(i=1; i<f_size.x; i++)
            if (fatmap[PIX(i,j)] == BLACK){
                if (flag == 0) {
                        fatmap[PIX(i-1, j)] = CONTOUR;
                        flag = 1;
            }
        }
            else
                    flag = 0;

/* PASS 2: RIGHTWARDS */
for (j=0; j<f_size.y; j++)
    for (i=f_size.x - 1; i>=0; i--)
            if (fatmap[PIX(i,j)] == BLACK){
                if (flag == 0) {
                        fatmap[PIX(i+1, j)] = CONTOUR;
                        flag = 1;
            }
        }
            else
                    flag = 0;

/* PASS 3: DOWNWARDS */
flag = 0;
for (i=0; i<f_size.x; i++)
    for (j=0; j<f_size.y; j++)
            if (fatmap[PIX(i,j)] == BLACK){
                if (flag == 0) {
                        fatmap[PIX(i, j-1)] = CONTOUR;
                        flag = 1;
            }
        }
            else
                    flag = 0;

/* PASS 4: UPWARDS */
flag = 0;
for (i=0; i<f_size.x; i++)
    for (j=f_size.y - 1; j>=0; j--)
            if (fatmap[PIX(i,j)] == BLACK){
                if (flag == 0) {
                        fatmap[PIX(i, j+1)] = CONTOUR;
                        flag = 1;
            }
        }
            else
                    flag = 0;


/* COMPUTE THE BOUNDING BOX OF THE CHARACTER (L,T,R,B) */
for (j=0; j<f_size.y; j++)
    for(i=1; i<f_size.x; i++)   
        if (fatmap[PIX(i,j)]==CONTOUR){
            bbox[0].x = MIN(i, bbox[0].x);
            bbox[0].y = MIN(j, bbox[0].y);
            bbox[1].x = MAX(i, bbox[1].x);
            bbox[1].y = MAX(j, bbox[1].y);
        }

/* DETERMINE THE CONTOUR PIXEL CLOSEST TO THE UPPER LEFT CORNER */
/* OF THE BOUNDING BOX                                          */

distance = INT_MAX;
for (j=0; j<f_size.y; j++)
    for(i=1; i<f_size.x; i++)   
        if (fatmap[PIX(i,j)]==CONTOUR){
            d = (i-bbox[0].x) * (i-bbox[0].x) + (j-bbox[0].y) * (j-bbox[0].y);
            if (d < distance) {
                distance = d;
                start_pixel.x = i;
                start_pixel.y = j;
            }
        }

/* BEGIN THE ENCODING PROCEDURE */
pixel.x = start_pixel.x;
pixel.y = start_pixel.y;
fatmap[PIX(pixel.x, pixel.y)] = VISITED;
last_dir = 4;
while(0 < 1) {
    /* AT FIRST, CHECK THE PIXEL IN THE LAST KNOWN DIRECTION */
    addPt2(&pixel, &contour_dir[last_dir], &test_pixel);
    if (fatmap[PIX(test_pixel.x, test_pixel.y)] == CONTOUR){
        pixel.x = test_pixel.x;
        pixel.y = test_pixel.y;
        fatmap[PIX(pixel.x, pixel.y)] = VISITED;
        code4.add(direction_code[last_dir]);
        }
    /* CHECK ALL THE POSSIBLE DIRECTIONS, CLOCKWISE */
    for (i=0;i<8;i++) {
        addPt2(&pixel, &contour_dir[i], &test_pixel);
        if (fatmap[PIX(test_pixel.x, test_pixel.y)] == CONTOUR){
            pixel.x = test_pixel.x;
            pixel.y = test_pixel.y;
            fatmap[PIX(pixel.x, pixel.y)] = VISITED;
            code4.add(direction_code[i]);
            last_dir = i;
            break;
            }
        }
    if (i == 8)
        break;
}

/* WRITE THE LAST MOVE TO THE OUTPUT VECTOR */
for (i=0; i<8; i++) {
    subPt2(&start_pixel, &pixel, &test_pixel);
    if (test_pixel.x==contour_dir[i].x && test_pixel.y==contour_dir[i].y){
        code4.add(direction_code[i]);
        break;
        }
    }

/* POST-PROCESSING LOOP:                                  */
/* GO BACK TO A LOWER RESOLUTION BY FILTERING THE 4x CODE */

code1 = code4.postProcess();
return code1;
}

