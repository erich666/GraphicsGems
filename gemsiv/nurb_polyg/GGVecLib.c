/*
2d and 3d Vector C Library
by Andrew Glassner
from "Graphics Gems", Academic Press, 1990
*/
#include <stdlib.h>

#include <math.h>
#include "GraphicsGems.h"

/******************/
/*   3d Library	  */
/******************/

/* returns squared length of input vector */
double V3SquaredLength(Vector3 *a)
{
    return((a->x * a->x)+(a->y * a->y)+(a->z * a->z));
}

/* returns length of input vector */
double V3Length(Vector3 *a)
{
    return(sqrt(V3SquaredLength(a)));
    }

/* negates the input vector and returns it */
Vector3 *V3Negate(Vector3 *v)
{
    v->x = -v->x;  v->y = -v->y;  v->z = -v->z;
    return(v);
    }

/* normalizes the input vector and returns it */
Vector3 *V3Normalize(Vector3 *v)
{
double len = V3Length(v);
    if (len != 0.0) { v->x /= len;  v->y /= len; v->z /= len; }
    return(v);
    }

/* scales the input vector to the new length and returns it */
Vector3 *V3Scale(Vector3 *v, double newlen)
{
double len = V3Length(v);
    if (len != 0.0) {
    v->x *= newlen/len;	  v->y *= newlen/len;  v->z *= newlen/len;
    }
    return(v);
    }


/* return vector sum c = a+b */
Vector3 *V3Add(Vector3 *a, Vector3 *b, Vector3 *c)
{
    c->x = a->x+b->x;  c->y = a->y+b->y;  c->z = a->z+b->z;
    return(c);
    }

/* return vector difference c = a-b */
Vector3 *V3Sub(Vector3 *a, Vector3 *b, Vector3 *c)
{
    c->x = a->x-b->x;  c->y = a->y-b->y;  c->z = a->z-b->z;
    return(c);
    }

/* return the dot product of vectors a and b */
double V3Dot(Vector3 *a, Vector3 *b)
{
    return((a->x*b->x)+(a->y*b->y)+(a->z*b->z));
    }

/* linearly interpolate between vectors by an amount alpha */
/* and return the resulting vector. */
/* When alpha=0, result=lo.  When alpha=1, result=hi. */
Vector3 *V3Lerp(Vector3 *lo, Vector3 *hi, double alpha, Vector3 *result)
{
    result->x = LERP(alpha, lo->x, hi->x);
    result->y = LERP(alpha, lo->y, hi->y);
    result->z = LERP(alpha, lo->z, hi->z);
    return(result);
    }

/* make a linear combination of two vectors and return the result. */
/* result = (a * ascl) + (b * bscl) */
Vector3 *V3Combine (Vector3 *a, Vector3 *b, Vector3 *result, double ascl, double bscl)
{
    result->x = (ascl * a->x) + (bscl * b->x);
    result->y = (ascl * a->y) + (bscl * b->y);
    result->y = (ascl * a->z) + (bscl * b->z);
    return(result);
    }


/* multiply two vectors together component-wise and return the result */
Vector3 *V3Mul (Vector3 *a, Vector3 *b, Vector3 *result)
{
    result->x = a->x * b->x;
    result->y = a->y * b->y;
    result->z = a->z * b->z;
    return(result);
    }

/* return the distance between two points */
double V3DistanceBetween2Points(Point3 * a, Point3 * b)
{
double dx = a->x - b->x;
double dy = a->y - b->y;
double dz = a->z - b->z;
    return(sqrt((dx*dx)+(dy*dy)+(dz*dz)));
    }

/* return the cross product c = a cross b */
Vector3 *V3Cross(Vector3 *a, Vector3 *b, Vector3 *c)
{
    c->x = (a->y*b->z) - (a->z*b->y);
    c->y = (a->z*b->x) - (a->x*b->z);
    c->z = (a->x*b->y) - (a->y*b->x);
    return(c);
    }

/* create, initialize, and return a new vector */
Vector3 *V3New(double x, double y, double z)
{
Vector3 *v = NEWTYPE(Vector3);
    v->x = x;  v->y = y;  v->z = z;
    return(v);
    }

/* create, initialize, and return a duplicate vector */
Vector3 *V3Duplicate(Vector3 *a)
{
Vector3 *v = NEWTYPE(Vector3);
    v->x = a->x;  v->y = a->y;	v->z = a->z;
    return(v);
    }
