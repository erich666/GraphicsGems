/*
NEWELL'S METHOD FOR COMPUTING THE PLANE EQUATION OF A POLYGON


Filippo Tampieri
Cornell University
*/











#include <math.h>

/* definition for the components of vectors and plane equations */
#define X   0
#define Y   1
#define Z   2
#define D   3

/* a few useful vector operations */
#define VZERO(v)    (v[X] = v[Y] = v[Z] = 0.0)
#define VNORM(v)    (sqrt(v[X] * v[X] + v[Y] * v[Y] + v[Z] * v[Z]))
#define VDOT(u, v)  (u[0] * v[0] + u[1] * v[1] + u[2] * v[2])
#define VINCR(u, v) (u[X] += v[X], u[Y] += v[Y], u[Z] += v[Z])

/* type definitions for vectors and plane equations */
typedef float Vector[3];
typedef Vector Point;
typedef Vector Normal;
typedef float Plane[4];

/*
**  PlaneEquation--computes the plane equation of an arbitrary
**  3D polygon using Newell's method.
**
**  Entry:
**      verts  - list of the vertices of the polygon
**      nverts - number of vertices of the polygon
**  Exit:
**      plane  - normalized (unit normal) plane equation
*/

PlaneEquation(verts, nverts, plane)
Point *verts;
int nverts;
Plane plane;
{
    int i;
    Point refpt;
    Normal normal;
    float *u, *v, len;




    /* compute the polygon normal and a reference point on
       the plane. Note that the actual reference point is
       refpt / nverts
    */
    VZERO(normal);
    VZERO(refpt);
    for(i = 0; i < nverts; i++) {
        u = verts[i];
        v = verts[(i + 1) % nverts];
        normal[X] += (u[Y] - v[Y]) * (u[Z] + v[Z]);
        normal[Y] += (u[Z] - v[Z]) * (u[X] + v[X]);
        normal[Z] += (u[X] - v[X]) * (u[Y] + v[Y]);
        VINCR(refpt, u);
    }
    /* normalize the polygon normal to obtain the first
       three coefficients of the plane equation
    */
    len = VNORM(normal);
    plane[X] = normal[X] / len;
    plane[Y] = normal[Y] / len;
    plane[Z] = normal[Z] / len;
    /* compute the last coefficient of the plane equation */
    len *= nverts;
    plane[D] = -VDOT(refpt, normal) / len;
}



