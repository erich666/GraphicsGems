#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <starbase.c.h>

#include "fixpoint.h"

extern void printnbits();
extern void subpixel_triangle(fixpoint x0, fixpoint y0,
							  fixpoint x1, fixpoint y1,
							  fixpoint x2, fixpoint y2);
extern void triangle(int x0, int y0, int x1, int y1, int x2, int y2);


/*
  Exhaustively test all cases of (subpixel) triangle drawing.

  In fixpoint.h, Set HIBITS, LOBITS to small values (like 5, 2) to run this,
  or you'll wait forever and run out of memory.

  */

static int fildes;
static struct color {float r, g, b;};
int verbose = 0;

#define SIZE 200
#define NPIXELS (1 << (HIBITS - 2))
#define BUFSIZE NPIXELS
int buffer[BUFSIZE][BUFSIZE];

void clear_buffer()
{
  register int i, j;
  for(i=0; i<BUFSIZE; i++)
	 for(j=0; j<BUFSIZE; j++)
		buffer[i][j] = 0;
}

/* Check if test succeeded (ie. buffer is full of ones,
	except for first line/column).
*/
void buffer_check()
{
  register int i, j;
  int broken = 0;

  for(i=1; i<BUFSIZE; i++)
	 for(j=1; j<BUFSIZE; j++)
		if (buffer[i][j] != 1) {
		  printf("ERROR: Buffer[%d][%d] = %d, not 1!!!\n",
					i, j, buffer[i][j]);
		  broken = 1;
		  break;
		}

  if (broken) {
	 for(j=1; j<BUFSIZE; j++) {
		for(i=1; i<BUFSIZE; i++) {
		  printf("%c", buffer[i][j] ? 'o' : '.' );
		}
		printf("\n");
	 }
  }
}


int xoff = 100;
int yoff = 100;

void
draw_point (int ix, int iy)
{

  if ((ix >= BUFSIZE) || (iy >= BUFSIZE)) {
	  printf("ERROR: ix (%d) or iy (%d) >= BUFSIZE (%d)\n", ix, iy, BUFSIZE);
	  abort();
	}

  if (buffer[ix][iy]){
	 printf("OOPS -- repainting pixel (%d, %d)\n", ix, iy);

	 fill_color(fildes, 1.0, 0.0, 0.5);
	 intrectangle(fildes, xoff + ix, yoff + iy, xoff + ix+1, yoff + iy+1);
	 fill_color(fildes, 1.0, 1.0, 1.0);

	 buffer[ix][iy] = 0;

	 make_picture_current(fildes);
	 abort();
  }
  else {
	 buffer[ix][iy] = 1;
	 intrectangle(fildes, xoff + ix, yoff + iy, xoff + ix+1, yoff + iy+1);
  }
}


int
InitScreen()
{
	char	*sb_dev, *driver;
	
	printnbits();

	sb_dev = getenv("SB_OUTDEV");   
	if(!sb_dev) sb_dev = getenv("OUTDEV");   
	if(!sb_dev) sb_dev = "/dev/crt1";

	driver = getenv("SB_OUTDRIVER");
	if(!driver) driver = getenv("OUTDRIVER");
	if (!driver ) driver = "hp98731";
	
	fildes = gopen(sb_dev,OUTDEV,driver,INIT|INT_XFORM);
	interior_style(fildes,INT_SOLID,FALSE);
	
	intvdc_extent(fildes,0,0,SIZE, SIZE);
	mapping_mode(fildes,FALSE);
	
	drawing_mode(fildes,6);	  /* xor mode */
	
	clear_control(fildes, CLEAR_VIEWPORT);
	fill_color(fildes, 1.0, 1.0, 1.0);
	
	return(fildes);
}


void
main()
{
  int i, j;
  int nsubpixels = 1 << (HIBITS+LOBITS);
  fixpoint zero, maxpix;

  zero = 0;
  maxpix = ((NPIXELS-1) << LOBITS) | LOMASK;

  InitScreen();

  if (verbose) {
	 printf("zero "); fp_print(zero); printf(", max "); fp_print(maxpix);
	 printf("\n");
  }

  for (i=0; i != maxpix + 1; i++) {
	 for (j=0; j != maxpix + 1; j++) {

		if (verbose) {
		  printf("\n\n************** New Iteration ************\n\n");
		  printf("%3d, %3d --> ", i, j);
		  fp_print(i); printf(", "); fp_print(j); printf("\n");
		}

		clear_buffer();
		clear_view_surface(fildes); 

		fill_color(fildes, 0.5, 0.5, 0.5);
		subpixel_triangle(zero, zero,  zero,  maxpix, i, j);

		fill_color(fildes, 0.0, 1.0, 0.0);
		subpixel_triangle(zero, zero,  maxpix,  zero, i, j);

		fill_color(fildes, 0.0, 0.0, 1.0);
		subpixel_triangle(zero,  maxpix,  maxpix,   maxpix, i, j);

		fill_color(fildes, 1.0, 0.0, 0.0);
		subpixel_triangle( maxpix, zero,  maxpix,   maxpix, i, j);

		buffer_check();
		make_picture_current(fildes);

		if (verbose) {
		  printf("  hit <return> for next iteration\n");
		  getchar();
		}

	 }
  }
}

