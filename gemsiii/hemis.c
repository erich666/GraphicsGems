/*
Hemispherical Projection of a Triangle

Buming Bian

UT System---Center for High Performance Computing 

Austin, Texas

*/
#include <math.h>
#include <stdio.h>

#include "GraphicsGems.h"
#include "GraphicsGems.c"


#define HOMON 4
#define PONPATCH 3
#define ERR  1.0e-10
#define ONE  1 - ERR
/* types of structures */
typedef struct { 
        Point3 pt[PONPATCH];   /* Vertices coordinates */
        double ref;            /* Reflectivity of the patch */
        double trs;            /* transmittance of the patch */
   } Patch;

/* return a patch normal. */
Vector3 *patchnorm(suf)
Patch *suf;
{
    Vector3 *vt1 = NEWTYPE(Vector3); 
    Vector3 *vt2 = NEWTYPE(Vector3);
    Vector3 *norm = NEWTYPE(Vector3);
    vt1 = V3Sub(&suf->pt[1], &suf->pt[0], vt1);
    vt2 = V3Sub(&suf->pt[2], &suf->pt[0], vt2);
    norm = V3Normalize(V3Cross(vt1, vt2, norm));
    free(vt1); free(vt2);
    return(norm);
}
/******************************************************************************
*  Compute the form factor. Homogeneous transformation is applied to the two  *
*  input Patches such that the center of patch suf1 located at origin with    *
*  its normal coinciding with z axis. A hemisphere is constructed above the xy* 
*  plane, patch suf2 is projected on the hemisphere to form a spherical       * 
*  triangle. This spherical triangle is again projected on the xy plane and   * 
*  the projected area is the form factor of patch suf2 to patch suf1.         * 
******************************************************************************/
double hemisphere(suf1, suf2)
Patch *suf1, *suf2;
{
    int i;
    double out, hm[HOMON][HOMON], projectarea();
    Vector3 *vect[PONPATCH];
	void patchtransf(), ghmgn();

/* use patch suf1 to form the homogeneous transformation matrix */

    ghmgn(suf1, hm);
/* apply the transformation to patch suf2 */
    patchtransf(hm, suf2);
/* represent the vertices with vectors */
    for (i = 0; i < PONPATCH; i++)
        vect[i] = V3New(suf2->pt[i].x, suf2->pt[i].y, suf2->pt[i].z);
/* Normalize the form factor so the all sub-total of the form factor */
    out = projectarea(vect);
    for (i = 0; i < PONPATCH; i++)
        free(vect[i]);
    return(ABS(out/PI));
}
/* Calculate the form factor by adding up all the ellipse sectors formed by two 
 * neighbor points */
double projectarea(vect)
Vector3 *vect[PONPATCH];
{
    int i;
    double out= 0, twopntarea();
    for (i = 0; i < PONPATCH; i++)
    out += twopntarea(vect[i], vect[(i+1)%PONPATCH]);
    return(out);
}
/* Calculate the area of the ellipsoid triangle */
double twopntarea(vect1, vect2)
Vector3 *vect1, *vect2;
{
    double a, b;
    double sum, cos_sita, halfsita, halfa, d1, d2;
    double arccos(), arctan();
    Vector3 *major, *norm, *v;
/* Find out the normal vector of the great circle */
    major = V3New(0.0, 0.0, 1.0);
    norm = V3Duplicate(vect1);
    v = NEWTYPE(Vector3);
    norm = V3Normalize(V3Cross(norm, vect2, v));
    cos_sita = ABS(V3Dot(major, norm)); 
    if (cos_sita < ERR){
/* if the normal vector is perpendicular  to z axis, form factor is zero */
        sum = 0.0;
        free(norm);
        free(major);
        return(sum);
    } 
    halfsita = cos_sita / 2.0;
    halfa = PI * halfsita;
    if (cos_sita < ONE){
/* project the great circle on to the equatorial plane and calculate the form factor */
        major = V3Normalize(V3Cross(major, norm, v));
        norm->x *= -1; norm->y *= -1; norm->z = 0;
        d1 = sqrt(V3Dot(vect1, vect1));
        d2 = sqrt(V3Dot(vect2, vect2));
        a  = V3Dot(major, vect1) / d1;
        if (ABS(a) > 1.0) a /= ABS(a);
        b = V3Dot(norm, vect1);
        sum  = arccos(a, b);
        a = V3Dot(major, vect2) / d2;
        if (ABS(a) > 1.0) a /= ABS(a);
        b  = V3Dot(norm, vect2);
        sum  -= arccos(a, b);
        }
    else{
/* if the normal vector is parallel to z axis, form factor is maximum */
        sum  = arctan(vect1->x, vect1->y);
        sum -= arctan(vect2->x, vect2->y); 
        }
    sum *= halfsita;
    free(norm); free(major); free(v);
    if (sum > halfa) sum -= 2.0 * halfa;
    else if (sum < -halfa) sum += 2.0 * halfa;
    return(sum);
}

double arccos(x, y)
double x, y;
{
    double angle;
    angle = acos(x);
    if (y < 0) angle = 2 * PI - angle;
    return(angle);
}

double arctan(x, y)
double x, y;
{
    double angle;
    angle = atan2(y, x);
    if (y < 0) angle = 2 * PI + angle;
    return(angle);
}

/* Derive the homogeneous transformation matrix for the given patch */
void ghmgn(suf, hm) 
Patch *suf;
double hm[HOMON][HOMON];
{
    int i, j;
    Vector3 *v1;
    Point3 *pnt = NEWTYPE(Vector3);
    double d;
    double tt[HOMON][HOMON], tr1[HOMON][HOMON];
    double tr2[HOMON][HOMON], rr[HOMON][HOMON];
    void mtxprd();

    pnt->x = (suf->pt[0].x+suf->pt[1].x+suf->pt[2].x)/PONPATCH;
    pnt->y = (suf->pt[0].y+suf->pt[1].y+suf->pt[2].y)/PONPATCH;
    pnt->z = (suf->pt[0].z+suf->pt[1].z+suf->pt[2].z)/PONPATCH;
    for (i = 0; i < HOMON; i++) 
        for (j = 0; j < HOMON; j++)
            if (i == j){
                tt[i][j] = 1.0; tr1[i][j] = 1.0; tr2[i][j] = 1.0; }
            else{
                tt[i][j] = 0; tr1[i][j] = 0; tr2[i][j] = 0; }
    v1 = patchnorm(suf);
    tt[3][0] = -pnt->x; tt[3][1] = -pnt->y; tt[3][2] = -pnt->z;
    free(pnt);
    d = sqrt(SQR(v1->y)+SQR(v1->z));
    if (d > ERR){
        tr1[0][0] = 1;
        tr1[3][3] = 1;
        tr1[1][1] = v1->z/d;
        tr1[2][2] = v1->z/d;
        tr1[2][1] = -v1->y/d;
        tr1[1][2] = v1->y/d;
    }
    mtxprd(tt, tr1, rr);
    tr2[0][0] = d; tr2[2][2] = d; tr2[2][0] = -v1->x; tr2[0][2] = v1->x;
    free(v1);
    mtxprd(rr, tr2, hm);
}


/* Transform a given patch */
void patchtransf(hm, suf)
double hm[HOMON][HOMON];
Patch *suf;
{
    int i;
    void pnttransf();
    for (i = 0; i < PONPATCH; i++)
        pnttransf(hm, &suf->pt[i]);
}

/* Transform a given point */
void pnttransf(hm, pnt)
double hm[HOMON][HOMON];
Point3 *pnt;
{
    double hi[HOMON], ho[HOMON];
    void transform();
    hi[0] = pnt->x; hi[1] = pnt->y; hi[2] = pnt->z; hi[3] = 1.;
    transform(hm, hi, ho);
    pnt->x = ho[0] / ho[3]; pnt->y = ho[1] / ho[3]; pnt->z = ho[2] / ho[3];
}

/* Transform a given vector */
void vecttransf(hm, vect)
double hm[HOMON][HOMON];
Vector3 *vect;
{
    double hi[HOMON], ho[HOMON];
    void transform();
    hi[0] = vect->x; hi[1] = vect->y; hi[2] = vect->z; hi[3] = 0.;
    transform(hm, hi, ho);
    vect->x = ho[0]; vect->y = ho[1]; vect->z = ho[2];
}

/* Transform a given vector */
void transform(hm, hi, ho)
double hm[HOMON][HOMON];
double hi[HOMON], ho[HOMON];
{
    int j, k;
    for (j = 0; j < HOMON; j++){
        ho[j] = 0;
        for (k = 0; k < HOMON; k++)
            ho[j] += hi[k] * hm[k][j];
    }  
}

/* Multiple two matrices */
void mtxprd(h1, h2, h3)
double h1[HOMON][HOMON], h2[HOMON][HOMON],h3[HOMON][HOMON];
{
    int i, j, k;
    for (i = 0; i < HOMON; i++)
        for (j = 0; j < HOMON; j++){
            h3[i][j] = 0;
            for (k = 0; k < HOMON; k++)
                h3[i][j] += h1[i][k] * h2[k][j];
        }
}
