#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <starbase.c.h>

#ifndef M_PI
#  define M_PI		3.14159265358979323846
#endif

extern double drand48();

typedef int fixpoint;
extern void subpixel_triangle(fixpoint x0, fixpoint y0,
										fixpoint x1, fixpoint y1,
										fixpoint x2, fixpoint y2);
extern void triangle(int x0, int y0, int x1, int y1, int x2, int y2);
extern void printnbits();

static int	fildes;
static struct color {float r, g, b;};
int verbose = 0;

#define MAXVERTICES     1000
#define MAXTRIANGLES    1000

#define SIZE 200
#define BUFSIZE SIZE*2
int buffer[BUFSIZE][BUFSIZE];
int which_tri;

void clear_buffer()
{
  register int i, j;
  if (verbose) printf("\n\n************** New Iteration ************\n\n");

  for(i=0; i<BUFSIZE; i++)
	 for(j=0; j<BUFSIZE; j++)
		buffer[i][j] = 0;
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
	
	if (!verbose)
	  double_buffer(fildes,TRUE,12); 

	clear_control(fildes, CLEAR_VIEWPORT);
	
	return(fildes);
}

int buf = 0;
int t0[MAXTRIANGLES], t1[MAXTRIANGLES], t2[MAXTRIANGLES];
fixpoint x[MAXVERTICES], y[MAXVERTICES];
float xf[MAXVERTICES], yf[MAXVERTICES];


void print_triangle(int n)
{
  printf("Triangle %d contains points %d, %d, %d\n", n, t0[n], t1[n], t2[n]);
  printf("which is (%g, %g), (%g, %g), (%g, %g)\n",
			xf[t0[n]], yf[t0[n]],
			xf[t1[n]], yf[t1[n]],
			xf[t2[n]], yf[t2[n]]);
  printf("which is ");
  fp_print(x[t0[n]]);
  printf(", ");
  fp_print(y[t0[n]]);
  printf(", ");
  fp_print(x[t1[n]]);
  printf(", ");
  fp_print(y[t1[n]]);
  printf(", ");
  fp_print(x[t2[n]]);
  printf(", ");
  fp_print(y[t2[n]]);
  printf("\n");
}


void
draw_point (int ix, int iy)
{

/*  if ((ix == 91) && (iy == 74)) {
  printf("Triangle painting (%d, %d) ... Triangle %d\n", ix, iy, which_tri);
  print_triangle(which_tri); }
*/

  if (buffer[ix][iy]){
	 printf("OOPS -- repainted pixel (%d, %d) for Triangle %d\n",
			  ix, iy, which_tri);
	 print_triangle(which_tri);

	 fill_color(fildes, 1.0, 0.0, 0.5);
	 intrectangle(fildes,ix,iy,ix+1,iy+1);
	 fill_color(fildes, 1.0, 1.0, 1.0);
/*	 dbuffer_switch(fildes, !buf); 
	 make_picture_current(fildes);
	 abort();
*/
  }
  else {
	 buffer[ix][iy] = 1;
	 intrectangle(fildes,ix,iy,ix+1,iy+1);
  }
}



#define CENTER 0.5

main (int argc, char *argv[])
{
    int xi[MAXVERTICES], yi[MAXVERTICES];
    struct color tc[MAXTRIANGLES];
    int nv, nt, i, j, k, white;
    double c, s;
    float rotinc, xr, yr;
	 float scale = 90.0;

	 char *filename = NULL;
	 FILE *fp;

	 if (argc == 2) {
		filename = argv[1];
		printf("reading from file %s\n", filename);
	 }

	 if (filename) fp = fopen(filename, "r");
	 else fp = stdin;

    fscanf(fp, "%f %d %d",&rotinc,&white,&nv);
	 printf("Read header info.\n");
    for (i=0; i<nv; i++) fscanf(fp, "%f %f",&xf[i],&yf[i]);
	 printf("Read %d verts\n", nv);

    for (nt=0; (fscanf(fp, "%d %d %d",&t0[nt],&t1[nt],&t2[nt])!=EOF) &&
			(nt < MAXTRIANGLES); nt++) {
		if (white) 
		  tc[nt].r = tc[nt].g = tc[nt].b = 1.0;
		else
		  tc[nt].r = drand48(), tc[nt].g = drand48(), tc[nt].b = drand48();
	 }

	 printf("Read %d triangles\n", nt);

    InitScreen();

    for (k=0; k<1000; k++) {
        c = cos(k*rotinc*M_PI/180.0);
        s = sin(k*rotinc*M_PI/180.0);

		  if (verbose) printf ("%4d: (%g, %g)\n", k, c, s);

        for (j=0; j<nv; j++) {
            xr = (( xf[j]-CENTER)*c + (yf[j]-CENTER)*s + 0.5);
            yr = ((-xf[j]+CENTER)*s + (yf[j]-CENTER)*c + 0.5);
            x[j] = fp_fix(xr * scale + 105.0);
            y[j] = fp_fix(yr * scale + 32.0);

            xi[j] = (int) (xr * scale + 5.5);
            yi[j] = (int) (yr * scale + 32.5);
        }

        clear_view_surface(fildes); 

		  clear_buffer();
        for (i=0; i<nt; i++) {
			 if (verbose) 
				printf("+++++++++++ Drawing triangle %d ++++++++++\n", i);

			 which_tri = i;
			 if (i < nt/2) fill_color(fildes,tc[i].r,tc[i].g,tc[i].b);
			 else          fill_color(fildes,1.0, 1.0, 0.5);

			 triangle(xi[t0[i]], yi[t0[i]],
						 xi[t1[i]], yi[t1[i]], xi[t2[i]], yi[t2[i]]);

			 if (verbose) printf(" Subpixel: ");
			 subpixel_triangle(x[t0[i]], y[t0[i]],
									 x[t1[i]], y[t1[i]], x[t2[i]], y[t2[i]]);

			 if (verbose) make_picture_current(fildes);				
        }

        if (!verbose) {
			 dbuffer_switch(fildes, buf = !buf); 
			 make_picture_current(fildes);
		  }

		  if (verbose) {
			 printf("hit <return> for next iteration\n");
			 getchar();
		  }
    }
}


