/* 
2d and 3d Vector C Library 
by Andrew Glassner
from "Graphics Gems", Academic Press, 1990
*/

#include <math.h>
#include "GGems.h"

/******************/
/*   2d Library   */
/******************/

/* returns squared length of input vector */	
double V2SquaredLength(a) 
Vector2 *a;
{	return((a->x * a->x)+(a->y * a->y));
	}
	
/* returns length of input vector */
double V2Length(a) 
Vector2 *a;
{
	return(sqrt(V2SquaredLength(a)));
	}
	
/* negates the input vector and returns it */
Vector2 *V2Negate(v) 
Vector2 *v;
{
	v->x = -v->x;  v->y = -v->y;
	return(v);
	}

/* normalizes the input vector and returns it */
Vector2 *V2Normalize(v) 
Vector2 *v;
{
double len = V2Length(v);
	if (len != 0.0) { v->x /= len;  v->y /= len; }
	return(v);
	}


/* scales the input vector to the new length and returns it */
Vector2 *V2Scale(v, newlen) 
Vector2 *v;
double newlen;
{
double len = V2Length(v);
	if (len != 0.0) { v->x *= newlen/len;   v->y *= newlen/len; }
	return(v);
	}

/* return vector sum c = a+b */
Vector2 *V2Add(a, b, c)
Vector2 *a, *b, *c;
{
	c->x = a->x+b->x;  c->y = a->y+b->y;
	return(c);
	}
	
/* return vector difference c = a-b */
Vector2 *V2Sub(a, b, c)
Vector2 *a, *b, *c;
{
	c->x = a->x-b->x;  c->y = a->y-b->y;
	return(c);
	}

/* return the dot product of vectors a and b */
double V2Dot(a, b) 
Vector2 *a, *b; 
{
	return((a->x*b->x)+(a->y*b->y));
	}

/* linearly interpolate between vectors by an amount alpha */
/* and return the resulting vector. */
/* When alpha=0, result=lo.  When alpha=1, result=hi. */
Vector2 *V2Lerp(lo, hi, alpha, result) 
Vector2 *lo, *hi, *result; 
double alpha;
{
	result->x = LERP(alpha, lo->x, hi->x);
	result->y = LERP(alpha, lo->y, hi->y);
	return(result);
	}


/* make a linear combination of two vectors and return the result. */
/* result = (a * ascl) + (b * bscl) */
Vector2 *V2Combine (a, b, result, ascl, bscl) 
Vector2 *a, *b, *result;
double ascl, bscl;
{
	result->x = (ascl * a->x) + (bscl * b->x);
	result->y = (ascl * a->y) + (bscl * b->y);
	return(result);
	}

/* multiply two vectors together component-wise */
Vector2 *V2Mul (a, b, result) 
Vector2 *a, *b, *result;
{
	result->x = a->x * b->x;
	result->y = a->y * b->y;
	return(result);
	}

/* return the distance between two points */
double V2DistanceBetween2Points(a, b)
Point2 *a, *b;
{
double dx = a->x - b->x;
double dy = a->y - b->y;
	return(sqrt((dx*dx)+(dy*dy)));
	}

/* return the vector perpendicular to the input vector a */
Vector2 *V2MakePerpendicular(a, ap)
Vector2 *a, *ap;
{
	ap->x = -a->y;
	ap->y = a->x;
	return(ap);
	}

/* create, initialize, and return a new vector */
Vector2 *V2New(x, y)
double x, y;
{
Vector2 *v = NEWTYPE(Vector2);
	v->x = x;  v->y = y; 
	return(v);
	}
	

/* create, initialize, and return a duplicate vector */
Vector2 *V2Duplicate(a)
Vector2 *a;
{
Vector2 *v = NEWTYPE(Vector2);
	v->x = a->x;  v->y = a->y; 
	return(v);
	}
	
/* multiply a point by a projective matrix and return the transformed point */
Point2 *V2MulPointByProjMatrix(pin, m, pout)
Point2 *pin, *pout;
Matrix3 *m;
{
double w;
	pout->x = (pin->x * m->element[0][0]) + 
             (pin->y * m->element[1][0]) + m->element[2][0];
	pout->y = (pin->x * m->element[0][1]) + 
             (pin->y * m->element[1][1]) + m->element[2][1];
	w    = (pin->x * m->element[0][2]) + 
             (pin->y * m->element[1][2]) + m->element[2][2];
	if (w != 0.0) { pout->x /= w;  pout->y /= w; }
	return(pout);
	}

/* multiply together matrices c = ab */
/* note that c must not point to either of the input matrices */
Matrix3 *V2MatMul(a, b, c)
Matrix3 *a, *b, *c;
{
int i, j, k;
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			c->element[i][j] = 0;
		for (k=0; k<3; k++) c->element[i][j] += 
				a->element[i][k] * b->element[k][j];
			}
		}
	return(c);
	}

/* transpose matrix a, return b */
Matrix3 *TransposeMatrix3(a, b)
Matrix3 *a, *b;
{
int i, j;
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++)
			b->element[i][j] = a->element[j][i];
		}
	return(b);
	}




/******************/
/*   3d Library   */
/******************/
	
/* returns squared length of input vector */	
double V3SquaredLength(a) 
Vector3 *a;
{
	return((a->x * a->x)+(a->y * a->y)+(a->z * a->z));
	}

/* returns length of input vector */
double V3Length(a) 
Vector3 *a;
{
	return(sqrt(V3SquaredLength(a)));
	}

/* negates the input vector and returns it */
Vector3 *V3Negate(v) 
Vector3 *v;
{
	v->x = -v->x;  v->y = -v->y;  v->z = -v->z;
	return(v);
	}

/* normalizes the input vector and returns it */
Vector3 *V3Normalize(v) 
Vector3 *v;
{
double len = V3Length(v);
	if (len != 0.0) { v->x /= len;  v->y /= len; v->z /= len; }
	return(v);
	}

/* scales the input vector to the new length and returns it */
Vector3 *V3Scale(v, newlen) 
Vector3 *v;
double newlen;
{
double len = V3Length(v);
	if (len != 0.0) {
	v->x *= newlen/len;   v->y *= newlen/len;  v->z *= newlen/len;
	}
	return(v);
	}


/* return vector sum c = a+b */
Vector3 *V3Add(a, b, c)
Vector3 *a, *b, *c;
{
	c->x = a->x+b->x;  c->y = a->y+b->y;  c->z = a->z+b->z;
	return(c);
	}
	
/* return vector difference c = a-b */
Vector3 *V3Sub(a, b, c)
Vector3 *a, *b, *c;
{
	c->x = a->x-b->x;  c->y = a->y-b->y;  c->z = a->z-b->z;
	return(c);
	}

/* return the dot product of vectors a and b */
double V3Dot(a, b) 
Vector3 *a, *b; 
{
	return((a->x*b->x)+(a->y*b->y)+(a->z*b->z));
	}

/* linearly interpolate between vectors by an amount alpha */
/* and return the resulting vector. */
/* When alpha=0, result=lo.  When alpha=1, result=hi. */
Vector3 *V3Lerp(lo, hi, alpha, result) 
Vector3 *lo, *hi, *result; 
double alpha;
{
	result->x = LERP(alpha, lo->x, hi->x);
	result->y = LERP(alpha, lo->y, hi->y);
	result->z = LERP(alpha, lo->z, hi->z);
	return(result);
	}

/* make a linear combination of two vectors and return the result. */
/* result = (a * ascl) + (b * bscl) */
Vector3 *V3Combine (a, b, result, ascl, bscl) 
Vector3 *a, *b, *result;
double ascl, bscl;
{
	result->x = (ascl * a->x) + (bscl * b->x);
	result->y = (ascl * a->y) + (bscl * b->y);
	result->z = (ascl * a->z) + (bscl * b->z);
	return(result);
	}


/* multiply two vectors together component-wise and return the result */
Vector3 *V3Mul (a, b, result) 
Vector3 *a, *b, *result;
{
	result->x = a->x * b->x;
	result->y = a->y * b->y;
	result->z = a->z * b->z;
	return(result);
	}

/* return the distance between two points */
double V3DistanceBetween2Points(a, b)
Point3 *a, *b;
{
double dx = a->x - b->x;
double dy = a->y - b->y;
double dz = a->z - b->z;
	return(sqrt((dx*dx)+(dy*dy)+(dz*dz)));
	}

/* return the cross product c = a cross b */
Vector3 *V3Cross(a, b, c)
Vector3 *a, *b, *c;
{
	c->x = (a->y*b->z) - (a->z*b->y);
	c->y = (a->z*b->x) - (a->x*b->z);
	c->z = (a->x*b->y) - (a->y*b->x);
	return(c);
	}

/* create, initialize, and return a new vector */
Vector3 *V3New(x, y, z)
double x, y, z;
{
Vector3 *v = NEWTYPE(Vector3);
	v->x = x;  v->y = y;  v->z = z;
	return(v);
	}

/* create, initialize, and return a duplicate vector */
Vector3 *V3Duplicate(a)
Vector3 *a;
{
Vector3 *v = NEWTYPE(Vector3);
	v->x = a->x;  v->y = a->y;  v->z = a->z;
	return(v);
	}

	
/* multiply a point by a matrix and return the transformed point */
Point3 *V3MulPointByMatrix(pin, m, pout)
Point3 *pin, *pout;
Matrix3 *m;
{
	pout->x = (pin->x * m->element[0][0]) + (pin->y * m->element[1][0]) + 
		 (pin->z * m->element[2][0]);
	pout->y = (pin->x * m->element[0][1]) + (pin->y * m->element[1][1]) + 
		 (pin->z * m->element[2][1]);
	pout->z = (pin->x * m->element[0][2]) + (pin->y * m->element[1][2]) + 
		 (pin->z * m->element[2][2]);
	return(pout);
	}

/* multiply a point by a projective matrix and return the transformed point */
Point3 *V3MulPointByProjMatrix(pin, m, pout)
Point3 *pin, *pout;
Matrix4 *m;
{
double w;
	pout->x = (pin->x * m->element[0][0]) + (pin->y * m->element[1][0]) + 
		 (pin->z * m->element[2][0]) + m->element[3][0];
	pout->y = (pin->x * m->element[0][1]) + (pin->y * m->element[1][1]) + 
		 (pin->z * m->element[2][1]) + m->element[3][1];
	pout->z = (pin->x * m->element[0][2]) + (pin->y * m->element[1][2]) + 
		 (pin->z * m->element[2][2]) + m->element[3][2];
	w =    (pin->x * m->element[0][3]) + (pin->y * m->element[1][3]) + 
		 (pin->z * m->element[2][3]) + m->element[3][3];
	if (w != 0.0) { pout->x /= w;  pout->y /= w;  pout->z /= w; }
	return(pout);
	}

/* multiply together matrices c = ab */
/* note that c must not point to either of the input matrices */
Matrix4 *V3MatMul(a, b, c)
Matrix4 *a, *b, *c;
{
int i, j, k;
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			c->element[i][j] = 0;
			for (k=0; k<4; k++) c->element[i][j] += 
				a->element[i][k] * b->element[k][j];
			}
		}
	return(c);
	}

/* binary greatest common divisor by Silver and Terzian.  See Knuth */
/* both inputs must be >= 0 */
gcd(u, v)
int u, v;
{
int t, f;
	if ((u<0) || (v<0)) return(1); /* error if u<0 or v<0 */
	f = 1;
	while ((0 == (u%2)) && (0 == (v%2))) {
		u>>=1;  v>>=1,  f*=2;
		}
	if (u&01) { t = -v;  goto B4; } else { t = u; }
	B3: if (t > 0) { t >>= 1; } else { t = -((-t) >> 1); }
	B4: if (0 == (t%2)) goto B3;

	if (t > 0) u = t; else v = -t;
	if (0 != (t = u - v)) goto B3;
	return(u*f);
	}	

/***********************/
/*   Useful Routines   */
/***********************/

/* return roots of ax^2+bx+c */
/* stable algebra derived from Numerical Recipes by Press et al.*/
int quadraticRoots(a, b, c, roots)
double a, b, c, *roots;
{
double d, q;
int count = 0;
	d = (b*b)-(4*a*c);
	if (d < 0.0) { *roots = *(roots+1) = 0.0;  return(0); }
	q =  -0.5 * (b + (SGN(b)*sqrt(d)));
	if (a != 0.0)  { *roots++ = q/a; count++; }
	if (q != 0.0) { *roots++ = c/q; count++; }
	return(count);
	}


/* generic 1d regula-falsi step.  f is function to evaluate */
/* interval known to contain root is given in left, right */
/* returns new estimate */
double RegulaFalsi(f, left, right)
double (*f)(), left, right;
{
double d = (*f)(right) - (*f)(left);
	if (d != 0.0) return (right - (*f)(right)*(right-left)/d);
	return((left+right)/2.0);
	}

/* generic 1d Newton-Raphson step. f is function, df is derivative */
/* x is current best guess for root location. Returns new estimate */
double NewtonRaphson(f, df, x)
double (*f)(), (*df)(), x;
{
double d = (*df)(x);
	if (d != 0.0) return (x-((*f)(x)/d));
	return(x-1.0);
	}


/* hybrid 1d Newton-Raphson/Regula Falsi root finder. */
/* input function f and its derivative df, an interval */
/* left, right known to contain the root, and an error tolerance */
/* Based on Blinn */
double findroot(left, right, tolerance, f, df)
double left, right, tolerance;
double (*f)(), (*df)();
{
double newx = left;
	while (ABS((*f)(newx)) > tolerance) {
		newx = NewtonRaphson(f, df, newx);
		if (newx < left || newx > right) 
			newx = RegulaFalsi(f, left, right);
		if ((*f)(newx) * (*f)(left) <= 0.0) right = newx;  
			else left = newx;
		}
	return(newx);
	} 

