/******************************************************************************
                       Plane-to-Plane Intersection
                       Author: Priamos Georgiades
******************************************************************************/
#include <stdio.h>
#include <math.h>

#define TRUE 1
#define FALSE 0
#define MRG_ZERO 1.0e-8

typedef struct vect3str {
    float x, y, z;
    } vect3;
typedef struct vect4str {
    float x, y, z, w;
    } vect4;

#define Vect3Init(a, b, c, v) { \
    (v).x = a; (v).y = b; (v).z = c; \
    }
#define Vect3Muls(s, v1, v2) { \
    (v2).x = s * (v1).x; \
    (v2).y = s * (v1).y; \
    (v2).z = s * (v1).z; \
    }
#define Vect3Cross(v1, v2, v3) { \
    (v3).x = (v1).y * (v2).z - (v1).z * (v2).y; \
    (v3).y = (v1).z * (v2).x - (v1).x * (v2).z; \
    (v3).z = (v1).x * (v2).y - (v1).y * (v2).x; \
    }

/*
Calculate the line of intersection between two planes. The two planes are
specified by their equations in the form
              P->x * X + P->y * Y + P->z * Z + P->w = 0.
Initialize the unit direction vector of the line of intersection in xdir.
Pick the point on the line of intersection on the coordinate plane most normal
to xdir. Return TRUE if successful, FALSE otherwise (indicating that the planes
don't intersect). The order in which the planes are given determines the choice
of direction of xdir.
*/
int GetXLine(vect4 *pl1, vect4 *pl2, vect3 *xdir, vect3 *xpt)
{
float invdet;  /* inverse of 2x2 matrix determinant */
vect3 dir2;    /* holds the squares of the coordinates of xdir */

Vect3Cross(*pl1, *pl2, *xdir)
dir2.x = xdir->x * xdir->x;
dir2.y = xdir->y * xdir->y;
dir2.z = xdir->z * xdir->z;


if (dir2.z > dir2.y && dir2.z > dir2.x && dir2.z > MRG_ZERO)
    {
    /* then get a point on the XY plane */
    invdet = 1.0 / xdir->z;
    /* solve < pl1.x * xpt.x + pl1.y * xpt.y = - pl1.w >
             < pl2.x * xpt.x + pl2.y * xpt.y = - pl2.w > */
    Vect3Init(pl1->y * pl2->w - pl2->y * pl1->w,
              pl2->x * pl1->w - pl1->x * pl2->w, 0.0, *xpt)
    }
else if (dir2.y > dir2.x && dir2.y > MRG_ZERO)
    {
    /* then get a point on the XZ plane */
    invdet = -1.0 / xdir->y;	/*** correction ***/
    /* solve < pl1.x * xpt.x + pl1.z * xpt.z = -pl1.w >
             < pl2.x * xpt.x + pl2.z * xpt.z = -pl2.w > */
    Vect3Init(pl1->z * pl2->w - pl2->z * pl1->w, 0.0,
              pl2->x * pl1->w - pl1->x * pl2->w, *xpt)
    }
else if (dir2.x > MRG_ZERO)
    {
    /* then get a point on the YZ plane */
    invdet = 1.0 / xdir->x;
    /* solve < pl1.y * xpt.y + pl1.z * xpt.z = - pl1.w >
             < pl2.y * xpt.y + pl2.z * xpt.z = - pl2.w > */
    Vect3Init(0.0, pl1->z * pl2->w - pl2->z * pl1->w,
              pl2->y * pl1->w - pl1->y * pl2->w, *xpt)
    }
else /* xdir is zero, then no point of intersection exists */
    return FALSE;
Vect3Muls(invdet, *xpt, *xpt)
invdet = 1.0 / (float)sqrt(dir2.x + dir2.y + dir2.z);
Vect3Muls(invdet, *xdir, *xdir)
return TRUE;
}

