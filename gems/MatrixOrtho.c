/* 
Matrix Orthogonalization
Eric Raible
from "Graphics Gems", Academic Press, 1990
*/

/*
 * Reorthogonalize matrix R - that is find an orthogonal matrix that is
 * "close" to R by computing an approximation to the orthogonal matrix
 *
 *           T  -1/2
 *   RC = R(R R)
 *			        			 T      -1
 * [RC is orthogonal because (RC) = (RC) ]
 *				               	                    -1/2
 * To compute C, we evaluate the Taylor expansion of F(x) = (I + x)
 * (where x = C - I) about x=0.
 * This gives C = I - (1/2)x + (3/8)x^2 - (5/16)x^3 + ...
 */

#include "GraphicsGems.h"

static float coef[10] = 			/* From mathematica */
  { 1, -1/2., 3/8., -5/16., 35/128., -63/256.,
    231/1024., -429/2048., 6435/32768., -12155/65536. };

MATRIX_reorthogonalize (R, limit)
     Matrix4 R;
{
  Matrix4 I, Temp, X, X_power, Sum;
  int power;

  limit = MAX(limit, 10);

  MATRIX_transpose (R, Temp);		/* Rt */
  MATRIX_multiply (Temp, R, Temp);	/* RtR */
  MATRIX_identify (I);
  MATRIX_subtract (Temp, I, X);	/* RtR - I */
  MATRIX_identify (X_power);		/* X^0 */
  MATRIX_identify (Sum);			/* coef[0] * X^0 */

  for (power = 1; power < limit; ++power)
    {
      MATRIX_multiply (X_power, X, X_power);
      MATRIX_constant_multiply (coef[power], X_power, Temp);
      MATRIX_add (Sum, Temp, Sum);
    }

  MATRIX_multiply (R, Sum, R);
}
