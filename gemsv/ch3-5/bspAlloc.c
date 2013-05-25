/* bspAlloc.c: module to allocate, free and append vertices and faces.
 * Copyright (c) Norman Chin 
 */
#include "bsp.h"

/* Allocates a vertex with position (xx,yy,zz) */
VERTEX *allocVertex(float xx,float yy,float zz)
{
   VERTEX *newVertex;

   if ((newVertex= (VERTEX *) MYMALLOC(sizeof(VERTEX))) == NULL_VERTEX) {
      fprintf(stderr,"?Unable to malloc vertex.\n");
      exit(1);
   }
   newVertex->xx= xx; newVertex->yy= yy; newVertex->zz= zz;
   newVertex->vnext= NULL_VERTEX;

   return(newVertex);
} /* allocVertex() */

/* Allocates a face with color, a list of vertices, a plane equation.
 */
FACE *allocFace(const COLOR *color,VERTEX *vlist,const PLANE *plane)
{
   FACE *newFace;

   if ((newFace= (FACE *) MYMALLOC(sizeof(FACE))) == NULL_FACE) {
      fprintf(stderr,"?Unable to alloc face.\n");
      exit(1);
   }
   newFace->color= *color;
   newFace->vhead= vlist;
   newFace->plane= *plane;
   newFace->fnext= NULL_FACE;

   return(newFace);
} /* allocFace() */

/* Append a vertex to a list. */
void appendVertex(VERTEX **vhead,VERTEX **vtail,VERTEX *vertex)
{
   assert( (*vhead == NULL_VERTEX) ? (*vtail == NULL_VERTEX) : 1 );
   assert(vertex != NULL_VERTEX);

   if (*vhead == NULL_VERTEX)
      *vhead= vertex;
   else (*vtail)->vnext= vertex;

   *vtail= vertex;		/* update tail */
} /* appendVertex() */

/* Append a face to a list. */
void appendFace(FACE **fhead,FACE **ftail,FACE *face)
{
   assert( (*fhead == NULL_FACE) ? (*ftail == NULL_FACE) : 1 );
   assert(face != NULL_FACE);

   if (*fhead == NULL_FACE)
      *fhead= face;
   else (*ftail)->fnext= face;

   *ftail= face;		/* update tail */
} /* appendFace() */

/* Frees list of vertices. */
void freeVertexList(VERTEX **vlist)
{
   VERTEX *vtrav= *vlist, *vdel;
   while (vtrav != NULL_VERTEX) {
      vdel= vtrav; vtrav= vtrav->vnext;

      MYFREE((char *) vdel);
   }
   *vlist= NULL_VERTEX;
} /* freeVertexList() */

/* Frees list of faces. */
void freeFaceList(FACE **flist) 
{
   FACE *ftrav= *flist, *fdel;
   while (ftrav != NULL_FACE) {
      fdel= ftrav; ftrav= ftrav->fnext; freeVertexList(&fdel->vhead); 

      MYFREE((char *)fdel);

   }
   *flist= NULL_FACE;
} /* freeFaceList() */
/*** bspAlloc.c ***/
