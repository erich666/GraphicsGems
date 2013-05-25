/**************************************************************************
This program illustrates the calculation of delta form-factors for the cubic tetrahedral algorithm.  The delta form-factor of each shaded cell in Fig. 2 
is computed and displayed.
**************************************************************************/

#include <math.h>

#define MIN   -2.0		/* Minimum value of a ct coordinate */
#define MAX    1.0		/* Maximum value of a ct coordinate */
#define SUBDIV 8		/* Number of cell subdivisions */

/* Calculate the form-factor of a cell centered at (u,v) with area a. */

float formFactor(u, v, a)
  float u, v, a;
{
  float	r = u*u + v*v + 1;
  return (a * (u + v + 1) / (M_PI * r*r * sqrt(3.0)));
}




main ()
{
  int	left, right, top, bottom,	/* Cell index boundaries */
		row, column;		/* Current cell indices */

  float 	delta, halfDelta,	/* Cell sizes */
		area, halfArea,		/* Cell areas */
		y, z;			/* Cell center location */

/* Initialize index values */

  left = 1;  right = SUBDIV;  
  top  = 1;  bottom = (SUBDIV + 1) / 2;
  row  = 1;  column = 1;

/* Initialize cell values */

  delta = (MAX - MIN) / SUBDIV;  halfDelta = delta / 2.0;
  area  = delta * delta;         halfArea  = area / 2.0;
  y = z = MAX - halfDelta;

/* Calculate and display delta form factors */

  for (row = top; row <= bottom; row++) {
    for (column = left; column < right; column++) {
      printf("Cell(%0d,%0d) = %f\n", row, column, formFactor(y, z, area));
      y -= delta;
    }
    printf("Cell(%0d,%0d) = %f\n", row, column,
      formFactor(y+halfDelta, z+halfDelta, halfArea));
    left++;  right--;
    y = z -= delta;
  }
}
