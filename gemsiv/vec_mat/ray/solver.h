/****************************************************************
*								*
* Polynomial root finder (polynoms up to degree 4)		*
* AUTHOR: Jochen SCHARZE (See 'Cubic & Quartic Roots' in	*
*			  'Graphics Gems 1', AP)		*
*								*
****************************************************************/

#include <math.h>
#define EQN_EPS 1e-9

int isZero(double x);
int solveQuadric(double c[3], double s[2]);
int solveCubic(double c[4], double s[3]);
int solveQuartic(double c[5], double s[4]);
