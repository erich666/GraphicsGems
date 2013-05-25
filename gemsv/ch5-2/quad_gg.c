/* ------------------------------------------------------------------------- *\
   QUAD_GG.C :

   by Christophe Schlick and Gilles Subrenat (15 May 1994)

   "Ray Intersection of Tessellated Surfaces : Quadrangles versus Triangles"
   in Graphics Gems V (edited by A. Paeth), Academic Press
\* ------------------------------------------------------------------------- */

#include "quad_gg.h"


/*********************
 * macro definitions *
 *********************/
#define EPSILON             ((REAL) 0.0001)

#define DETERMINANT(A,B)    ((REAL) ((A).x * (B).y - (A).y * (B).x))

#define V3_LIN(R,A,k,B)     ((R).x = (A).x + k * (B).x, \
                             (R).y = (A).y + k * (B).y, \
                             (R).z = (A).z + k * (B).z)

#define LARGEST_COMPONENT(A)  (ABS((A).x) > ABS((A).y) ? \
                              (ABS((A).x) > ABS((A).z) ? 'x' : 'z') : \
                              (ABS((A).y) > ABS((A).z) ? 'y' : 'z'))

/*
** Compute uv-coordinates of the point
** Return TRUE if the point is in the quadrangle
*/
static boolean point_in_quad (QUAD *Quad, HIT *Hit)
{
    char       LargestComponent;             /* of the normal vector         */
    Point2     A, B, C, D;                   /* Projected vertices           */
    Point2     M;                            /* Projected intersection point */
    Vector2    AB, BC, CD, AD, AM, AE;       /* AE = DC - AB = DA - CB       */
    REAL       u, v;                         /* Parametric coordinates       */
    REAL       a, b, c, SqrtDelta;           /* for the quadratic equation   */
    boolean    IsInside = FALSE;             /* if u and v are inside        */
    Vector2    Vector;                       /* temporary 2D-vector          */

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

    V2Sub (&B, &A, &AB); V2Sub (&C, &B, &BC);
    V2Sub (&D, &C, &CD); V2Sub (&D, &A, &AD);
    V2Add (&CD, &AB, &AE); V2Negate (&AE); V2Sub (&M, &A, &AM);

    if (fabs(DETERMINANT(AB, CD)) < EPSILON)               /* case AB // CD */
    {
        V2Sub (&AB, &CD, &Vector);
        v = DETERMINANT(AM, Vector) / DETERMINANT(AD, Vector);
        if ((v >= 0.0) && (v <= 1.0)) {
            b = DETERMINANT(AB, AD) - DETERMINANT(AM, AE);
            c = DETERMINANT (AM, AD);
            u = ABS(b) < EPSILON ? -1 : c/b;
            IsInside = ((u >= 0.0) && (u <= 1.0));
        }
    }
    else if (fabs(DETERMINANT(BC, AD)) < EPSILON)          /* case AD // BC */
    {
        V2Add (&AD, &BC, &Vector);
        u = DETERMINANT(AM, Vector) / DETERMINANT(AB, Vector);
        if ((u >= 0.0) && (u <= 1.0)) {
            b = DETERMINANT(AD, AB) - DETERMINANT(AM, AE);
            c = DETERMINANT (AM, AB);
            v = ABS(b) < EPSILON ? -1 : c/b;
            IsInside = ((v >= 0.0) && (v <= 1.0));
        }
    }
    else                                                    /* general case */
    {
        a = DETERMINANT(AB, AE); c = - DETERMINANT (AM,AD);
        b = DETERMINANT(AB, AD) - DETERMINANT(AM, AE);
        a = -0.5/a; b *= a; c *= (a + a); SqrtDelta = b*b + c;
        if (SqrtDelta >= 0.0) {
            SqrtDelta = sqrt(SqrtDelta);
            u = b - SqrtDelta;
            if ((u < 0.0) || (u > 1.0))      /* to choose u between 0 and 1 */
                u = b + SqrtDelta;
            if ((u >= 0.0) && (u <= 1.0)) {
                v = AD.x + u * AE.x;
                if (ABS(v) < EPSILON)
                    v = (AM.y - u * AB.y) / (AD.y + u * AE.y);
                else
                    v = (AM.x - u * AB.x) / v;
                IsInside = ((v >= 0.0) && (v <= 1.0));
            }
        }
    }

    if (IsInside) {
        Hit->u = u;
        Hit->v = v;
    }
    return (IsInside);
}

/*
** Search for an intersection between a facet and a ray
*/
boolean hit_ray_quad (RAY *Ray, QUAD *Quad, HIT *Hit)
{
    Point3     Point;

    /* if the ray is parallel to the facet, there is no intersection */
    Hit->Distance = V3Dot (&(Ray->Vector), &(Quad->Normal));
    if (ABS(Hit->Distance) < EPSILON) return (FALSE);

    /* compute ray intersection with the plane of the facet */
    V3Sub (&(Quad->A), &(Ray->Point), &Point);
    Hit->Distance = V3Dot (&Point, &(Quad->Normal)) / Hit->Distance;
    V3_LIN (Hit->Point, Ray->Point, Hit->Distance, Ray->Vector);

    /* is the intersection point inside the facet */
    return (point_in_quad(Quad, Hit));
}

/* ------------------------------------------------------------------------- */
