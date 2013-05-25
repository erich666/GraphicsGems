/* bspCollide.c: module to detect collisions between the viewer and static
 * objects in an environment represented as a BSP tree.
 * Copyright (c) Norman Chin 
 */
#include "bsp.h"

/* flags to indicate if any piece of a line segment is inside any polyhedron
 *     or outside all polyhedra
 */
static boolean anyPieceOfLineIn, anyPieceOfLineOut;

/* local functions - see function definition */
static int BSPclassifyPoint(const POINT *point, const BSPNODE *bspNode);
static void BSPclassifyLineInterior(const POINT *from, const POINT *to,
				    const BSPNODE *bspNode);
				    
/* Returns a boolean to indicate whether or not a collision had occurred 
 * between the viewer and any static objects in an environment represented as 
 * a BSP tree.
 * 
 * from    - start position of viewer 
 * to      - end position of viewer
 * bspTree - BSP tree of scene
 */
boolean BSPdidViewerCollideWithScene(const POINT *from, const POINT *to,
				     const BSPNODE *bspTree)
{
   /* first classify the endpoints */ 
   int sign1= BSPclassifyPoint(from,bspTree);
   int sign2= BSPclassifyPoint(to,bspTree);

   /* collision occurs iff there's a state change between endpoints or
    * either endpoint is on an object 
    */
   if (sign1 == 0 || sign2 == 0 || sign1 != sign2) return(1);
   else {
      anyPieceOfLineIn= anyPieceOfLineOut= 0; /* clear flags */
      /* since we already classified the endpoints, try interior of line */ 
      /*    this routine will set the flags to appropriate values */
      BSPclassifyLineInterior(from,to,bspTree);

      /* if line interior is inside and outside an object, collision detected*/
      /* else no collision detected */
      return( (anyPieceOfLineIn && anyPieceOfLineOut) ? 1 : 0 );
   }
} /*  BSPdidViewerCollideWithScene() */

/* Classifies point as to whether or not it is inside, outside or on an object
 * represented as a BSP tree, where inside is -1, outside is 1, on is 0.
 * 
 * point   - position of point
 * bspNode - a node in BSP tree
 */
static int BSPclassifyPoint(const POINT *point,const BSPNODE *bspNode)
{
   if (bspNode == NULL_BSPNODE) return(1); /* point is out since no tree */

   if (bspNode->kind == PARTITION_NODE) { /* compare point with plane */
      const PLANE *plane= &bspNode->node->sameDir->plane;
      float dp= plane->aa*point->xx + plane->bb*point->yy + 
                plane->cc*point->zz + plane->dd;
      if (dp < -TOLER)		/* point on "-" side, filter down "-" branch */
	 return(BSPclassifyPoint(point,bspNode->node->negativeSide));
      else if (dp > TOLER)	/* point on "+" side, filter down "+" branch */
	 return(BSPclassifyPoint(point,bspNode->node->positiveSide));
      else {			
	 /* point is on plane, so classify the neighborhood of point by 
	  * filtering the same point down both branches.  
	  */
	 int sign1= BSPclassifyPoint(point,bspNode->node->negativeSide);
	 int sign2= BSPclassifyPoint(point,bspNode->node->positiveSide);
	 /* if classification is same then return it otherwise it's on */
	 return( (sign1 == sign2) ? sign1 : 0 );
      }
   }
   else if (bspNode->kind == OUT_NODE) return(1); /* point is outside */
   else { assert(bspNode->kind == IN_NODE); return(-1); } /* point is inside */
} /* BSPclassifyPoint() */

/* Classifies interior of line segment (not including endpoints) as to whether
 * or not any piece is inside or outside an object represented as a BSP tree.
 * If it's on, it's recursively called on both half-spaces to set the flags.
 * There is no explicit on condition like we have with BSPclassifyPoint().
 * 
 * from    - endpoint of line segment
 * to      - other endpoint of line segment
 * bspNode - a node in BSP tree     
 */
static void BSPclassifyLineInterior(const POINT *from,const POINT *to,
				    const BSPNODE *bspNode)
				    
{
   if (bspNode->kind == PARTITION_NODE) { /* compare line segment with plane */
      float ixx,iyy,izz;
      const PLANE *plane= &bspNode->node->sameDir->plane;
      float dp1= plane->aa*from->xx + plane->bb*from->yy +
	         plane->cc*from->zz + plane->dd;
      float dp2= plane->aa*to->xx + plane->bb*to->yy +
	         plane->cc*to->zz + plane->dd;
      SIGN sign1= FSIGN(dp1); SIGN sign2= FSIGN(dp2);

      if ( (sign1 == NEGATIVE && sign2 == POSITIVE) || 
	   (sign1 == POSITIVE && sign2 == NEGATIVE) ) {	/* split! */
	 SIGN check= anyEdgeIntersectWithPlane(from->xx,from->yy,from->zz,
					       to->xx,to->yy,to->zz,
					       plane,&ixx,&iyy,&izz);
	 POINT iPoint;
	 assert(check != ZERO);
	 
	 /* filter split line segments down appropriate branches */
	 iPoint.xx= ixx; iPoint.yy= iyy; iPoint.zz= izz;
	 if (sign1 == NEGATIVE) { assert(sign2 == POSITIVE);
	    BSPclassifyLineInterior(from,&iPoint,bspNode->node->negativeSide);
	    BSPclassifyLineInterior(to,&iPoint,bspNode->node->positiveSide);
	 }
	 else { assert(sign1 == POSITIVE && sign2 == NEGATIVE); 
	    BSPclassifyLineInterior(from,&iPoint,bspNode->node->positiveSide);
	    BSPclassifyLineInterior(to,&iPoint,bspNode->node->negativeSide);
	 }
      }
      else {			/* no split,so on same side */
	 if (sign1 == ZERO && sign2 == ZERO) {
	    BSPclassifyLineInterior(from,to,bspNode->node->negativeSide);
	    BSPclassifyLineInterior(from,to,bspNode->node->positiveSide);
	 }
	 else if (sign1 == NEGATIVE || sign2 == NEGATIVE) {
	    BSPclassifyLineInterior(from,to,bspNode->node->negativeSide);
	 }
	 else { assert(sign1 == POSITIVE || sign2 == POSITIVE);
	    BSPclassifyLineInterior(from,to,bspNode->node->positiveSide);
	 }
      }
   }
   else if (bspNode->kind == IN_NODE) anyPieceOfLineIn= 1; /* line inside */
   else { assert(bspNode->kind == OUT_NODE); anyPieceOfLineOut= 1; }
} /* BSPclassifyLineInterior() */
/*** bspCollide.c ***/
