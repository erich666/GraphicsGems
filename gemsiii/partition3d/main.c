/* main.c: sample driver for partition module.
 * Copyright (c) Norman Chin
 */
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include "GraphicsGems.h"
#include "partition.h"

static FACE *getFace(/* void */);
static void dumpFace(/* FACE *face, char *string */);

int main()
{
   FACE *inputFace;
   FACE *faceOn, *faceNeg, *facePos; 
   FACE *faceOn2, *faceNeg2, *facePos2;

   inputFace= getFace(); dumpFace(inputFace,"input");

   /* partition unit square on XY plane about origin */
   partitionFaceWithPlane(1.0,0.0,0.0,0.0,&inputFace, 
			  &faceOn,&faceNeg,&facePos); 
   assert(inputFace == NULL_FACE);
   dumpFace(faceNeg,"negative side"); dumpFace(facePos,"positive side");

   /* now partition positive piece's upper left corner */
   inputFace= facePos;
   partitionFaceWithPlane(0.707,-0.707,0.0,0.0,&inputFace,
			  &faceOn2,&faceNeg2,&facePos2);
   assert(inputFace == NULL_FACE);
   dumpFace(faceNeg2,"negative side"); dumpFace(facePos2,"positive side");

   /* code to free all faces & their vertices goes here */

   return(0); 
} /* main() */

static void dumpFace(face,string)
FACE *face;
char *string;
{
   VERTEX *vtrav;

   if (face == NULL_FACE) {
      (void) printf("%s is empty\n",string);
      return;
   }

   (void) printf("begin dump of %s at 0x%x\n",string,face);
   for (vtrav= face->vhead; vtrav != NULL_VERTEX; vtrav= vtrav->vnext) {
      (void) printf("   0x%x: (%lf %lf %lf)\n",vtrav,
		    vtrav->xx,vtrav->yy,vtrav->zz);
   }
   (void) printf("end %s\n",string);
} /* dumpFace() */

static FACE *getFace()
{
   /* list of vertices for unit square on XY plane about origin */
   static VERTEX square[]= {
       {1.0,-1.0,0.0, NULL_VERTEX},
       {1.0, 1.0,0.0, NULL_VERTEX},
       {-1.0, 1.0,0.0, NULL_VERTEX},
       {-1.0,-1.0,0.0, NULL_VERTEX},
       {1.0,-1.0,0.0, NULL_VERTEX}
   };
   VERTEX *v1, *v2, *v3, *v4, *v5;
   FACE *newFace;

   v1= NEWTYPE(VERTEX); assert(v1 != NULL_VERTEX); *v1= square[0];
   v2= NEWTYPE(VERTEX); assert(v2 != NULL_VERTEX); *v2= square[1];
   v3= NEWTYPE(VERTEX); assert(v3 != NULL_VERTEX); *v3= square[2];
   v4= NEWTYPE(VERTEX); assert(v4 != NULL_VERTEX); *v4= square[3];
   v5= NEWTYPE(VERTEX); assert(v5 != NULL_VERTEX); *v5= square[4];
   /* chain vertices */
   v1->vnext= v2; v2->vnext= v3; v3->vnext= v4; v4->vnext= v5; 

   /* attach vertex list to a face */
   newFace= NEWTYPE(FACE); assert(newFace != NULL_FACE); newFace->vhead= v1;

   return(newFace);
} /* getFace() */
