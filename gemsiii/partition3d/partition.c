/* partition.c: module to partition 3D convex face with an arbitrary plane.
 * Copyright (c) Norman Chin
 */ 
#include <stdio.h>		/* NULL */
#include <assert.h>		/* assert() */
#include <malloc.h>		/* malloc() */
#include <math.h>		/* fabs() */
#include "GraphicsGems.h"	
#include "partition.h"

/* local functions */
static VERTEX *findNextIntersection(/* VERTEX *vstart, double aa,
				       double bb, double cc, double dd,
				       double *ixx, double *iyy, double *izz,
				       SIGN *sign */);
static SIGN anyEdgeIntersectWithPlane(/* double x1, double y1, double z1,
					 double x2, double y2, double z2,
					 double aa, double bb, double cc,
					 double dd, 
					 double *ixx, double *iyy, double *izz
				       */);
static FACE *createOtherFace(/* FACE *face, VERTEX *v1, double ixx1, 
				double iyy1, double izz1, VERTEX *v2,
				double ixx2, double iyy2, double izz2 */);
static SIGN whichSideIsFaceWRTplane(/* FACE *face, double aa, double bb,
				       double cc, double dd */);
static VERTEX *allocVertex(/* double xx, double yy, double zz */);
static FACE *allocFace(/* FACE *face, VERTEX *vhead */);

/* Partitions a 3D convex polygon (face) with an arbitrary plane into its 
 * negative and positive fragments, if any, w.r.t. the partitioning plane.
 * Note that inputFace is unusable afterwards since its vertex list has been
 * parceled out to the other faces. It's set to null to avoid dangling
 * pointer problem. 
 */
void partitionFaceWithPlane(aa,bb,cc,dd,inputFace,faceOn,faceNeg,facePos)
double aa,bb,cc,dd;		/* partitioning plane's coefficients */
FACE **inputFace;		/* face to be partitioned */
FACE **faceOn;			/* returns face embedded in plane, if any  */
FACE **faceNeg;			/* returns face on negative side, if any */
FACE **facePos;			/* returns face on positive side, if any */
{
   VERTEX *v1, *v2;
   double ixx1,iyy1,izz1, ixx2,iyy2,izz2;
   SIGN signV1, signV2;

   *faceOn= *faceNeg= *facePos= NULL_FACE;

   /* find first intersection */
   v1= findNextIntersection((*inputFace)->vhead,aa,bb,cc,dd, 
			    &ixx1,&iyy1,&izz1,&signV1);
   if (v1 != NULL_VERTEX) {
      assert(signV1 != ZERO);

      /* first one found, find the 2nd one, if any */
      v2= findNextIntersection(v1->vnext,aa,bb,cc,dd, 
			       &ixx2,&iyy2,&izz2,&signV2);

      /* Due to numerical instabilities, both intersection points may
       * have the same sign such as in the case when splitting very close
       * to a vertex. This should not count as a split.
       */
      if (v2 != NULL_VERTEX && signV1 == signV2) v2= NULL_VERTEX; 

   }
   else v2= NULL_VERTEX;	/* No first intersection found,
                                 * therefore no second intersection.
				 */
                                
   /* an intersection? */
   if (v1 != NULL_VERTEX && v2 != NULL_VERTEX) { /* yes, intersection */
      FACE *newOtherFace;

      /* inputFace's vertex list will be modified */
      newOtherFace= createOtherFace(*inputFace,v1,ixx1,iyy1,izz1,
				               v2,ixx2,iyy2,izz2);
      /* return split faces on appropriate lists */
      if (signV1 == NEGATIVE) { 
	 *faceNeg= *inputFace; 
         *facePos= newOtherFace;
      }
      else {
	 assert(signV1 == POSITIVE);
	 *faceNeg= newOtherFace;
	 *facePos= *inputFace;
      }
				    
   }
   else {			/* no intersection  */
      SIGN side;

      /* Face is embedded or wholly to one side of partitioning plane. */
      side= whichSideIsFaceWRTplane(*inputFace,aa,bb,cc,dd);
      if (side == NEGATIVE)
	 *faceNeg= *inputFace;
      else if (side == POSITIVE)
	 *facePos= *inputFace;
      else {
	 assert(side == ZERO);
	 *faceOn= *inputFace;
      }
   }
   /* inputFace's vertex list has been parceled out to other lists so
    * set this to null.
    */
   *inputFace= NULL_FACE;		

   /* If a face is embedded in plane then there must not be any faces on
    *    either side.
    * Otherwise the face isn't embedded in plane so there must be at least 
    *    one face on either side or both.
    */
   assert( (*faceOn != NULL_FACE) ? 
	    (*faceNeg == NULL_FACE &&
	     *facePos == NULL_FACE) :
	    (*faceNeg != NULL_FACE ||
	     *facePos != NULL_FACE) );
} /* partitionFaceWithPlane() */
			    
/* Finds next intersection on or after vstart. 
 * 
 * If an intersection is found, 
 *    a pointer to first vertex of the edge is returned, 
 *    the intersection point (ixx,iyy,izz) and its sign is updated. 
 * Otherwise a null pointer is returned.
 */
static VERTEX *findNextIntersection(vstart,aa,bb,cc,dd,ixx,iyy,izz,sign)
VERTEX *vstart;			/* where to start searching on vertex list */
double aa,bb,cc,dd;		/* plane's coefficients */
double *ixx,*iyy,*izz;		/* returned intersection point */
SIGN *sign;		/* returned classification of intersection point */
{
   VERTEX *vtrav;

   /* for all edges starting from vstart ... */
   for (vtrav= vstart; vtrav->vnext != NULL_VERTEX; vtrav= vtrav->vnext) {
      if ((*sign= anyEdgeIntersectWithPlane(vtrav->xx,vtrav->yy,vtrav->zz,
					    vtrav->vnext->xx,vtrav->vnext->yy,
					    vtrav->vnext->zz,aa,bb,cc,dd,
					    ixx,iyy,izz))) {
	 return(vtrav);
      }
   }

   return(NULL_VERTEX);
} /* findNextIntersection() */

/* Memory allocated for split face's vertices and pointers tediously updated.
 */
static FACE *createOtherFace(face,v1,ixx1,iyy1,izz1,v2,ixx2,iyy2,izz2)
FACE *face;			/* face to be split */
VERTEX *v1;	/* 1st vertex of edge of where 1st intersection was found */
double ixx1,iyy1,izz1;		/* 1st intersection  */
VERTEX *v2;     /* 1st vertex of edge of where 2nd intersection was found */
double ixx2,iyy2,izz2;		/* 2nd intersection */
{
   VERTEX *i1p1, *i2p1;		/* new vertices for original face  */
   VERTEX *i1p2, *i2p2;		/* new vertices for new face */
   VERTEX *p2end;		/* new vertex for end of new face */
   VERTEX *vtemp;		/* place holders */
   register VERTEX *beforeV2;	/* place holders */
   FACE *newFace;		/* newly allocated face */

   /* new intersection vertices */
   i1p1= allocVertex(ixx1,iyy1,izz1); 
   i2p1= allocVertex(ixx2,iyy2,izz2);
   i1p2= allocVertex(ixx1,iyy1,izz1);
   i2p2= allocVertex(ixx2,iyy2,izz2); 

   /* duplicate 1st vertex of 2nd list to close it up */
   p2end= allocVertex(v2->xx,v2->yy,v2->zz);

   vtemp= v1->vnext;

   /* merge both intersection vertices i1p1 & i2p1 into 1st list */
   if (ISVERTEX_EQ(i2p1,v2->vnext)) { /* intersection vertex coincident? */
      FREEVERTEX(i2p1);		/* yes, so free it */
      i1p1->vnext= v2->vnext;
   }
   else {
      i2p1->vnext= v2->vnext;	/* attach intersection list onto 1st list */
      i1p1->vnext= i2p1;	/* attach both intersection vertices */
   }
   v1->vnext= i1p1; /* attach front of 1st list to intersection vertices */

   /* merge intersection vertices i1p2, i2p2 & p2end into second list */
   i2p2->vnext= i1p2;		/* attach both intersection vertices */
   v2->vnext= i2p2;		/* attach 2nd list to interection vertices */
   if (vtemp == v2) {
      i1p2->vnext= p2end;	/* close up 2nd list */
   }
   else {
      if (ISVERTEX_EQ(i1p2,vtemp)) { /* intersection vertex coincident? */
	 FREEVERTEX(i1p2);	/* yes, so free it */
	 i2p2->vnext= vtemp;	/* attach intersection vertex to 2nd list */
      }
      else {
	 i1p2->vnext= vtemp;	/* attach intersection list to 2nd list */
      }
      /* find previous vertex to v2 */
      for (beforeV2= vtemp; beforeV2->vnext != v2; beforeV2= beforeV2->vnext)
	 ;			/* lone semi-colon */
      beforeV2->vnext= p2end;	/* and attach it to complete the 2nd list */
   }

   /* copy original face info but with new vertex list */
   newFace= allocFace(face,v2);

   return(newFace);
} /* createOtherFace() */

/* Determines which side a face is with respect to a plane with coefficient
 * (aa,bb,cc,dd).
 *
 * However, due to numerical problems, when a face is very close to the plane,
 * some vertices may be misclassified. 
 * There are several solutions, two of which are mentioned here:
 *    1- classify the one vertex furthest away from the plane, (note that
 *       one need not compute the actual distance) and use that side.
 *    2- count how many vertices lie on either side and pick the side
 *       with the maximum. (this is the one implemented).
 */
static SIGN whichSideIsFaceWRTplane(face,aa,bb,cc,dd)
FACE *face;
double aa,bb,cc,dd;
{
   register VERTEX *vtrav;
   double value;
   boolean isNeg, isPos;

   isNeg= isPos= FALSE;
   
   for (vtrav= face->vhead; vtrav->vnext != NULL_VERTEX; vtrav= vtrav->vnext){
      value= (aa*vtrav->xx) + (bb*vtrav->yy) + (cc*vtrav->zz) + dd;
      if (value < -TOLER) 
	 isNeg= TRUE;
      else if (value > TOLER)
	 isPos= TRUE;
      else assert(ISDOUBLE_EQ(value,0.0));
   } /* for vtrav */ 

   /* in the very rare case that some vertices slipped thru to other side of
    * plane due to round-off errors, execute the above again but count the 
    * vertices on each side instead and pick the maximum.
    */
   if (isNeg && isPos) {	/* yes so handle this numerical problem */
      int countNeg, countPos;
      
      /* count how many vertices are on either side */
      countNeg= countPos= 0;
      for (vtrav= face->vhead; vtrav->vnext != NULL_VERTEX; 
	   vtrav= vtrav->vnext) {
	 value= (aa*vtrav->xx) + (bb*vtrav->yy) + (cc*vtrav->zz) + dd;
	 if (value < -TOLER)
	    countNeg++;
	 else if (value > TOLER)
	    countPos++;
	 else assert(ISDOUBLE_EQ(value,0.0));
      } /* for */

      /* return the side corresponding to the maximum */
      if (countNeg > countPos) return(NEGATIVE);
      else if (countPos > countNeg) return(POSITIVE);
      else return(ZERO);
   }
   else {			/* this is the usual case */
      if (isNeg) return(NEGATIVE);
      else if (isPos) return(POSITIVE);
      else return(ZERO);
   }
} /* whichSideIsFaceWRTplane() */

/* Determines if an edge bounded by (x1,y1,z1)->(x2,y2,z2) intersects
 * the plane with the coefficients (aa,bb,cc,dd).
 * 
 * If there's an intersection, 
 *    the sign of (x1,y1,z1), NEGATIVE or POSITIVE, w.r.t. the plane is
 *    returned with the intersection (ixx,iyy,izz) updated.
 * Otherwise ZERO is returned.    
 */
static SIGN anyEdgeIntersectWithPlane(x1,y1,z1,x2,y2,z2,aa,bb,cc,dd,
					ixx,iyy,izz)
double x1,y1,z1, x2,y2,z2;	/* both vertices of edge */
double aa,bb,cc,dd;		/* partitioning plane's coefficients */
double *ixx,*iyy,*izz;		/* returned intersection point, if any */
{
   double temp1, temp2;
   int sign1, sign2;		/* must be int since gonna do a bitwise ^ */

   /* get signs */
   temp1= (aa*x1) + (bb*y1) + (cc*z1) + dd;
   if (temp1 < -TOLER)
      sign1= -1;
   else if (temp1 > TOLER)
      sign1= 1;
   else {
      /* edges beginning with a 0 sign are not considered valid intersections
       * case 1 & 6 & 7, see text
       */
      assert(ISDOUBLE_EQ(temp1,0.0));
      return(ZERO);
   }

   temp2= (aa*x2) + (bb*y2) + (cc*z2) + dd;
   if (temp2 < -TOLER)
      sign2= -1;
   else if (temp2 > TOLER)
      sign2= 1;
   else {			/* case 8 & 9, see text  */
      assert(ISDOUBLE_EQ(temp2,0.0));
      *ixx= x2;
      *iyy= y2;
      *izz= z2;

      return( (sign1 == -1) ? NEGATIVE : POSITIVE);
   }

   /* signs different? 
    * recall: -1^1 == 1^-1 ==> 1    case 4 & 5, see text
    *         -1^-1 == 1^1 ==> 0    case 2 & 3, see text
    */
   if (sign1 ^ sign2) {
      double dx,dy,dz;
      double denom, tt;

      /* compute intersection point */
      dx= x2-x1;
      dy= y2-y1;
      dz= z2-z1;

      denom= (aa*dx) + (bb*dy) + (cc*dz);
      tt= - ((aa*x1) + (bb*y1) + (cc*z1) + dd) / denom;

      *ixx= x1 + (tt * dx);
      *iyy= y1 + (tt * dy);
      *izz= z1 + (tt * dz);

      assert(sign1 == 1 || sign1 == -1);

      return( (sign1 == -1) ? NEGATIVE : POSITIVE );
   }
   else return(ZERO);

} /* anyEdgeIntersectWithPlane() */

/* allocates vertex with coordinates (xx,yy,zz) */
static VERTEX *allocVertex(xx,yy,zz)
double xx,yy,zz;
{
   VERTEX *newVertex;

   newVertex= NEWTYPE(VERTEX); assert(newVertex != NULL_VERTEX);
   newVertex->xx= xx; newVertex->yy= yy; newVertex->zz= zz; 
   newVertex->vnext= NULL_VERTEX; 
   
   return(newVertex);
} /* allocVertex() */

/* allocates face with list of vertices, vhead */
static FACE *allocFace(face,vhead)
FACE *face;
VERTEX *vhead;
{
   FACE *newFace;

   newFace= NEWTYPE(FACE); assert(newFace != NULL_FACE); 
   *newFace= *face;
   newFace->vhead= vhead;

   return(newFace);
} /* allocFace() */

