/******************************************************************************
                   Signed Distance from Point to Plane
                       Author: Priamos Georgiades
******************************************************************************/
#include<stdio.h>

typedef struct vect3str {
    float x, y, z;
    } vect3;
typedef struct vect4str {
    float x, y, z, w;
    } vect4;

#define Vect3Dot(v1, v2) ((v1).x * (v2).x + (v1).y * (v2).y + (v1).z * (v2).z)

/*
Compute the distance from a point to a plane. The plane is
pleq->x * X + pleq->y * Y + pleq->z * Z + pleq->w = 0.
The distance is positive if on same side as the normal, otherwise negative.
Assume the plane normal to be of unit length.
*/
float Pt2Plane(vect3 *pt, vect4 *pleq)
{
float dist;

dist = Vect3Dot(*pleq, *pt) + pleq->w;
return(dist);
}

