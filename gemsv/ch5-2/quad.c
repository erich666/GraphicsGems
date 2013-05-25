/* ------------------------------------------------------------------------- *\
   QUAD.C :

   by Christophe Schlick and Gilles Subrenat (15 May 1994)

   "Ray Intersection of Tessellated Surfaces : Quadrangles versus Triangles"
   in Graphics Gems V (edited by A. Paeth), Academic Press
\* ------------------------------------------------------------------------- */

#include "quad.h"

/*
** Macro definitions
*/
#define MY_TOL                ((real) 0.0001)

#define LARGEST_COMPONENT(A)  (ABS((A).x) > ABS((A).y) ? \
                              (ABS((A).x) > ABS((A).z) ? 'x' : 'z') : \
                              (ABS((A).y) > ABS((A).z) ? 'y' : 'z'))

/*
** Check if the point is in the quadrangle
*/
static bool point_in_quad (QUAD *Quad, HIT *Hit)
{
    char       LargestComponent;             /* of the normal vector         */
    realvec2   A, B, C, D;                   /* Projected vertices           */
    realvec2   M;                            /* Projected intersection point */
    realvec2   AB, BC, CD, AD, AM, AE;       /* Miscellanous 3D-vectors      */
    real       u, v;                         /* Parametric coordinates       */
    real       a, b, c, SqrtDelta;           /* Quadratic equation           */
    bool       Intersection = FALSE;         /* Intersection flag            */
    realvec2   Vector;                       /* Temporary 2D-vector          */

    /*
    ** Projection on the plane that is most parallel to the facet
    */
    LargestComponent = LARGEST_COMPONENT(Quad->Normal);

    if (LargestComponent == 'x') {
        A.x = Quad->A.y; B.x = Quad->B.y; C.x = Quad->C.y; D.x = Quad->D.y;
        M.x = Hit->Point.y;
    }
    else {
        A.x = Quad->A.x; B.x = Quad->B.x; C.x = Quad->C.x; D.x = Quad->D.x;
        M.x = Hit->Point.x;
    }

    if (LargestComponent == 'z') {
        A.y = Quad->A.y; B.y = Quad->B.y; C.y = Quad->C.y; D.y = Quad->D.y;
        M.y = Hit->Point.y;
    }
    else {
        A.y = Quad->A.z; B.y = Quad->B.z; C.y = Quad->C.z; D.y = Quad->D.z;
        M.y = Hit->Point.z;
    }

    SUB_VEC2 (AB, B, A); SUB_VEC2 (BC, C, B);
    SUB_VEC2 (CD, D, C); SUB_VEC2 (AD, D, A);
    ADD_VEC2 (AE, CD, AB); NEG_VEC2 (AE, AE); SUB_VEC2 (AM, M, A);

    if (ZERO_TOL (DELTA_VEC2(AB, CD), MY_TOL))             /* case AB // CD */
    {
        SUB_VEC2 (Vector, AB, CD);
        v = DELTA_VEC2(AM, Vector) / DELTA_VEC2(AD, Vector);
        if ((v >= 0.0) && (v <= 1.0)) {
            b = DELTA_VEC2(AB, AD) - DELTA_VEC2(AM, AE);
            c = DELTA_VEC2 (AM, AD);
            u = ZERO_TOL(b, MY_TOL) ? -1.0 : c/b;
            Intersection = ((u >= 0.0) && (u <= 1.0));
        }
    }
    else if (ZERO_TOL(DELTA_VEC2(BC, AD), MY_TOL))         /* case AD // BC */
    {
        ADD_VEC2 (Vector, AD, BC);
        u = DELTA_VEC2(AM, Vector) / DELTA_VEC2(AB, Vector);
        if ((u >= 0.0) && (u <= 1.0)) {
            b = DELTA_VEC2(AD, AB) - DELTA_VEC2(AM, AE);
            c = DELTA_VEC2 (AM, AB);
            v = ZERO_TOL(b, MY_TOL) ? -1.0 : c/b;
            Intersection = ((v >= 0.0) && (v <= 1.0));
        }
    }
    else                                                    /* general case */
    {
        a = DELTA_VEC2(AB, AE); c = - DELTA_VEC2 (AM,AD);
        b = DELTA_VEC2(AB, AD) - DELTA_VEC2(AM, AE);
        a = -0.5/a; b *= a; c *= (a + a); SqrtDelta = b*b + c;
        if (SqrtDelta >= 0.0) {
            SqrtDelta = sqrt(SqrtDelta);
            u = b - SqrtDelta;
            if ((u < 0.0) || (u > 1.0))        /* we want u between 0 and 1 */
                u = b + SqrtDelta;
            if ((u >= 0.0) && (u <= 1.0)) {
                v = AD.x + u * AE.x;
                if (ZERO_TOL(v, MY_TOL))
                    v = (AM.y - u * AB.y) / (AD.y + u * AE.y);
                else
                    v = (AM.x - u * AB.x) / v;
                Intersection = ((v >= 0.0) && (v <= 1.0));
            }
        }
    }

    if (Intersection) {
        Hit->u = u;
        Hit->v = v;
    }
    return (Intersection);
}

/*
** Search for an intersection between a quadrangle and a ray
*/
bool hit_ray_quad (RAY *Ray, QUAD *Quad, HIT *Hit)
{
    realvec3     Point;

    /* if the ray is parallel to the quadrangle, there is no intersection */
    Hit->Distance = DOT_VEC3 (Ray->Vector, Quad->Normal);
    if (ZERO_TOL(Hit->Distance, MY_TOL)) return (FALSE);

    /* compute ray intersection with the plane of the quadrangle */
    SUB_VEC3 (Point, Quad->A, Ray->Point);
    Hit->Distance = DOT_VEC3 (Point, Quad->Normal) / Hit->Distance;
    MULS_VEC3 (Hit->Point, Ray->Vector, Hit->Distance);
    INC_VEC3 (Hit->Point, Ray->Point);

    /* is the point in the quadrangle ? */
    return (point_in_quad(Quad, Hit));
}

/* ------------------------------------------------------------------------- */
