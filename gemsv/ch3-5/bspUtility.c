/* bspUtility.c: module to compute plane equation, normalize a vector and 
 * perform cross products. 
 * Copyright (c) Norman Chin 
 */
#include "bsp.h"

/* local functions */
static void normalizeVector(float ii,float jj,float kk,
			    float *ii2,float *jj2,float *kk2);
static void crossProduct(float ii,float jj,float kk,
			 float ii2,float jj2,float kk2,
			 float *iicp,float *jjcp,float *kkcp);

/* Computes plane equation.
 *
 * xx0,yy0,zz0, xx1,yy1,zz1, xx2,yy2,zz2 - 3 non-collinear vertices 
 * plane                                 - plane equation returned
 */
void computePlane(float xx0,float yy0,float zz0,float xx1,float yy1,float zz1,
		  float xx2,float yy2,float zz2, PLANE *plane)
{
   float ii1= xx1 - xx0; float jj1= yy1 - yy0; float kk1= zz1 - zz0;
   float ii2= xx2 - xx0; float jj2= yy2 - yy0; float kk2= zz2 - zz0;
   float iicp, jjcp, kkcp;

   crossProduct(ii1,jj1,kk1,ii2,jj2,kk2,&iicp,&jjcp,&kkcp);
   assert(!(IS_EQ(iicp,0.0) && IS_EQ(jjcp,0.0) && IS_EQ(kkcp,0.0)));

   /* normalize plane equation */
   normalizeVector(iicp,jjcp,kkcp,&plane->aa,&plane->bb,&plane->cc);

   /* compute D of plane equation */
   plane->dd= - (plane->aa * xx0) - (plane->bb * yy0) - (plane->cc * zz0); 
} /* computePlane() */

/* Performs cross product.
 *
 * ii1,jj1,kk1, ii2,j2,kk2 - two vectors
 * iicp,jjcp,kkcp          - cross product
 */
static void crossProduct(float ii1,float jj1,float kk1,
			 float ii2,float jj2,float kk2,
			 float *iicp,float *jjcp,float *kkcp)
{
 *iicp= jj1*kk2 - jj2*kk1; *jjcp= ii2*kk1 - ii1*kk2; *kkcp= ii1*jj2 - ii2*jj1;
} /* crossProduct() */

/* Normalize a vector.
 *
 * ii,jj,kk    - vector to be normalized  
 * ii2,jj2,kk2 - vector normalized
 */
static void normalizeVector(float ii,float jj,float kk,
			    float *ii2,float *jj2,float *kk2)
{
   double magnitude= sqrt((double)ii*ii + (double)jj*jj + (double)kk*kk);
   double dfactor= 1.0 / magnitude;

   *ii2= (float) (ii * dfactor);
   *jj2= (float) (jj * dfactor);
   *kk2= (float) (kk * dfactor);
} /* normalizeVector() */ 
/*** bspUtility.c ***/
