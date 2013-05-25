/* bspTree.c: module to construct and traverse a BSP tree.
 * Copyright (c) Norman Chin 
 */
#include "bsp.h"

/* local functions */
static void BSPchoosePlane(FACE *faceList,PLANE *plane);
static boolean doesFaceStraddlePlane(const FACE *face,const PLANE *plane);
static BSPNODE *allocBspNode(NODE_TYPE kind,FACE *sameDir,FACE *oppDir);
static PARTITIONNODE *allocPartitionNode(FACE *sameDir,FACE *oppDir);
static void freePartitionNode(PARTITIONNODE **partitionNode);

/* Returns a BSP tree of scene from a list of convex faces.
 * These faces' vertices are oriented in counterclockwise order where the last 
 * vertex is a duplicate of the first, i.e., a square has five vertices. 
 *
 * faceList - list of faces
 */
BSPNODE *BSPconstructTree(FACE **faceList)
{
   BSPNODE *newBspNode; PLANE plane; 
   FACE *sameDirList,*oppDirList, *faceNegList,*facePosList;

   /* choose plane to split scene with */
   BSPchoosePlane(*faceList,&plane); 
   BSPpartitionFaceListWithPlane(&plane,faceList,&faceNegList,&facePosList,
				 &sameDirList,&oppDirList);
   assert(*faceList == NULL_FACE); assert(sameDirList != NULL_FACE);

   /* construct the tree */
   newBspNode= allocBspNode(PARTITION_NODE,sameDirList,oppDirList);

   /* construct tree's "-" branch */
   if (faceNegList == NULL_FACE) 
    newBspNode->node->negativeSide= allocBspNode(IN_NODE,NULL_FACE,NULL_FACE);
   else newBspNode->node->negativeSide= BSPconstructTree(&faceNegList);

   /* construct tree's "+" branch */
   if (facePosList == NULL_FACE) 
    newBspNode->node->positiveSide=allocBspNode(OUT_NODE,NULL_FACE,NULL_FACE);
   else newBspNode->node->positiveSide= BSPconstructTree(&facePosList);

   return(newBspNode);
} /* BSPconstructTree() */

/* Traverses BSP tree to render scene back-to-front based on viewer position.
 *
 * bspNode  - a node in BSP tree
 * position - position of viewer
 */
void BSPtraverseTreeAndRender(const BSPNODE *bspNode,const POINT *position)
{
   if (bspNode == NULL_BSPNODE) return;

   if (bspNode->kind == PARTITION_NODE) {
      if (BSPisViewerInPositiveSideOfPlane(&bspNode->node->sameDir->plane,position)){

	 BSPtraverseTreeAndRender(bspNode->node->negativeSide,position);
	 drawFaceList(stdout,bspNode->node->sameDir);
	 drawFaceList(stdout,bspNode->node->oppDir); /* back-face cull */
	 BSPtraverseTreeAndRender(bspNode->node->positiveSide,position);

      }
      else {

	 BSPtraverseTreeAndRender(bspNode->node->positiveSide,position);
	 drawFaceList(stdout,bspNode->node->oppDir);
	 drawFaceList(stdout,bspNode->node->sameDir); /* back-face cull */
	 BSPtraverseTreeAndRender(bspNode->node->negativeSide,position);

      }
   }
   else assert(bspNode->kind == IN_NODE || bspNode->kind == OUT_NODE);
} /* BSPtraverseTreeAndRender() */

/* Frees a BSP tree and sets pointer to it to null
 *
 * bspNode - a pointer to a node in BSP tree, set to null upon exit 
 */
void BSPfreeTree(BSPNODE **bspNode)
{
   if (*bspNode == NULL_BSPNODE) return;

   if ((*bspNode)->kind == PARTITION_NODE) 
      freePartitionNode(&(*bspNode)->node);

   MYFREE((char *) *bspNode); *bspNode= NULL_BSPNODE;

} /* BSPfreeTree() */

/* Chooses plane with which to partition. 
 * The algorithm is to examine the first MAX_CANDIDATES on face list. For
 * each candidate, count how many splits it would make against the scene.
 * Then return the one with the minimum amount of splits as the 
 * partitioning plane.
 *
 * faceList - list of faces
 * plane    - plane equation returned
 */
static void BSPchoosePlane(FACE *faceList,PLANE *plane)
{
   FACE *rootrav; int ii;
   int minCount= MAXINT; 
   FACE *chosenRoot= faceList;	/* pick first face for now */

   assert(faceList != NULL_FACE);
   /* for all candidates... */
#define MAX_CANDIDATES 100
   for (rootrav= faceList, ii= 0; rootrav != NULL_FACE && ii< MAX_CANDIDATES;
	rootrav= rootrav->fnext, ii++) {
      FACE *ftrav; int count= 0;
      /* for all faces in scene other than itself... */
      for (ftrav= faceList; ftrav != NULL_FACE; ftrav= ftrav->fnext) {
	 if (ftrav != rootrav) 
	    if (doesFaceStraddlePlane(ftrav,&rootrav->plane)) count++;
      }
      /* remember minimum count and its corresponding face */
      if (count < minCount) { minCount= count; chosenRoot= rootrav; }
      if (count == 0) break; /* can't do better than 0 so return this plane */
   }
   *plane= chosenRoot->plane;	/* return partitioning plane */
} /* BSPchoosePlane() */

/* Returns a boolean to indicate whether the face straddles the plane
 *
 * face  - face to check 
 * plane - plane 
 */
static boolean doesFaceStraddlePlane(const FACE *face, const PLANE *plane)
{
   boolean anyNegative= 0, anyPositive= 0;
   VERTEX *vtrav; 

   assert(face->vhead != NULL_VERTEX);
   /* for all vertices... */
   for (vtrav= face->vhead; vtrav->vnext !=NULL_VERTEX; vtrav= vtrav->vnext) {
      float value= plane->aa*vtrav->xx + plane->bb*vtrav->yy +
	           plane->cc*vtrav->zz + plane->dd;
      /* check which side vertex is on relative to plane */
      SIGN sign= FSIGN(value);
      if (sign == NEGATIVE) anyNegative= 1; 
      else if (sign == POSITIVE) anyPositive= 1;

      /* if vertices on both sides of plane then face straddles else it no */  
      if (anyNegative && anyPositive) return(1);
   }
   return(0);
} /* doesFaceStraddlePlane() */

/* Returns a boolean to indicate whether or not point is in + side of plane.
 * 
 * plane    - plane 
 * position - position of point
 */
boolean BSPisViewerInPositiveSideOfPlane(const PLANE *plane,const POINT *position)
{
   float dp= plane->aa*position->xx + plane->bb*position->yy +
             plane->cc*position->zz + plane->dd;
   return( (dp > 0.0) ? 1 : 0 );
} /* BSPisViewerInPositiveSideOfPlane() */

/* Allocates a BSP node.
 *
 * kind            - type of BSP node
 * sameDir, oppDir - list of faces to be embedded in this node 
 */
static BSPNODE *allocBspNode(NODE_TYPE kind,FACE *sameDir,FACE *oppDir)
{
   BSPNODE *newBspNode;
   if ((newBspNode= (BSPNODE *) MYMALLOC(sizeof(BSPNODE))) == NULL_BSPNODE) {
      fprintf(stderr,"?Unable to malloc bspnode.\n");
      exit(1);
   }
   newBspNode->kind= kind;

   if (newBspNode->kind == PARTITION_NODE) 
      newBspNode->node= allocPartitionNode(sameDir,oppDir);
   else { assert(kind == IN_NODE || kind == OUT_NODE);
      assert(sameDir == NULL_FACE && oppDir == NULL_FACE);
      newBspNode->node= NULL_PARTITIONNODE;
   }
   return(newBspNode);
} /* allocBspNode() */

/* Allocates a partition node.
 * 
 * sameDir, oppDir - list of faces embedded in partition node
 */
static PARTITIONNODE *allocPartitionNode(FACE *sameDir,FACE *oppDir)
{
   PARTITIONNODE *newPartitionNode;
   if ((newPartitionNode= (PARTITIONNODE *) MYMALLOC(sizeof(PARTITIONNODE)))==
       NULL_PARTITIONNODE) {
      fprintf(stderr,"?Unable to malloc partitionnode.\n");
      exit(1);
   }
   newPartitionNode->sameDir= sameDir; newPartitionNode->oppDir= oppDir;
   newPartitionNode->negativeSide= NULL_BSPNODE;
   newPartitionNode->positiveSide= NULL_BSPNODE;

   return(newPartitionNode);
} /* allocPartitionNode() */

/* Frees partition node and sets pointer to it to null.
 *
 * partitonNode - partition node to be freed, pointer is set to null
 */
static void freePartitionNode(PARTITIONNODE **partitionNode)
{
   freeFaceList(&(*partitionNode)->sameDir);
   freeFaceList(&(*partitionNode)->oppDir);
   BSPfreeTree(&(*partitionNode)->negativeSide);
   BSPfreeTree(&(*partitionNode)->positiveSide);

   MYFREE((char *) *partitionNode); *partitionNode= NULL_PARTITIONNODE;
} /* freePartitionNode() */

/* Dumps information on faces. This should be replaced with user-supplied    
 * polygon scan converter.
 */
void drawFaceList(FILE *fp,const FACE *faceList)
{
   const FACE *ftrav;
   for (ftrav= faceList; ftrav != NULL_FACE; ftrav= ftrav->fnext) { 
      VERTEX *vtrav;

      fprintf(fp,"Face: RGBi:%.2f/%.2f/%.2f a: %.3f b: %.3f c: %.3f d: %.3f ",
	      ftrav->color.rr,ftrav->color.gg,ftrav->color.bb,
             ftrav->plane.aa,ftrav->plane.bb,ftrav->plane.cc,ftrav->plane.dd);
      fprintf(fp,"\n");
      for (vtrav= ftrav->vhead; vtrav->vnext != NULL_VERTEX; 
	   vtrav= vtrav->vnext) {
	 fprintf(fp,"\t(%.3f,%.3f,%.3f) ",vtrav->xx,vtrav->yy,vtrav->zz);
	 fprintf(fp,"\n");
      }
   }
} /* drawFaceList() */
/*** bspTree.c ***/ 
