/* mainBsp.c: main driver of BSP application
 * Copyright (c) Norman Chin 
 */
#include "bsp.h"

#define MAXBUFFER 80
#define NOBLOCK 'n'
#define COMMENT ';'

/* local functions */
int main(int argc,char *argv[]);
void getScene(const char *fileName,POINT *position,FACE **faceList);
#define dumpPosition(p) (printf("Position: (%f,%f,%f)\n",p.xx,p.yy,p.zz))

/* Main driver */
int main(int argc,char *argv[])
{
   FACE *faceList; POINT oldPosition;
   if (argc != 2) {fprintf(stderr,"Usage: %s <datefile>\n",argv[0]);exit(1);}
   
   getScene(argv[1],&oldPosition,&faceList); /* get list of faces from file */
   drawFaceList(stdout,faceList); /* dump faces */
   dumpPosition(oldPosition);	/* dump viewer position */

   {
      BSPNODE *root= BSPconstructTree(&faceList); /* construct BSP tree */

      BSPtraverseTreeAndRender(root,&oldPosition); /* traverse and render it */

      BSPfreeTree(&root);	/* free it */
   }
   freeFaceList(&faceList);	/* free list of faces read in */
   
   return(0);
} /* main() */

/* Reads from fileName a list of convex planar faces oriented counter-clockwise
 * of at least 3 vertices each where the first three vertices are not 
 * collinear. The last vertex of each face will automatically be duplicated. 
 * This list of faces is returned.
 * 
 */ 
void getScene(const char *fileName,POINT *position,FACE **fList)
{
   FACE *fListTail= NULL_FACE;
   VERTEX *vList= NULL_VERTEX, *vListTail= NULL_VERTEX;
   static char buffer[MAXBUFFER];
   COLOR color; PLANE plane;

   FILE *fp= fopen(fileName,"r");
   if (fp == NULL) {fprintf(stderr,"?Unable to open %s\n",fileName); exit(1);}
   printf("File: %s\n",fileName);

   position->xx= 0.0; position->yy= 5.0; position->zz= 10.0; 
   *fList= NULL_FACE; 

   while (fgets(buffer,MAXBUFFER,fp)) {
      if (buffer[0] == 'f') {	/* start of face */
	 if (vList != NULL_VERTEX) { /* previous face? */
	    appendVertex(&vList,&vListTail, /* append duplicate 1st vertex  */
			 allocVertex(vList->xx,vList->yy,vList->zz));
	    computePlane(vList->xx,vList->yy,vList->zz,
			 vList->vnext->xx,vList->vnext->yy,vList->vnext->zz,
			 vList->vnext->vnext->xx,
			 vList->vnext->vnext->yy,
			 vList->vnext->vnext->zz,&plane);
	    appendFace(fList,&fListTail,allocFace(&color,vList,&plane));
	 }

	 sscanf(buffer,"%*c %f %f %f",&color.rr,&color.gg,&color.bb);
	 /* save vars for this face and start new vertex list */
	 /* printf("f %f/%f/%f",color.rr,color.gg,color.bb); */
	 vList= vListTail= NULL_VERTEX;
      }
      else if (buffer[0] == 'v') { /* read in vertex */
	 float xx,yy,zz; sscanf(buffer,"%*c %f %f %f",&xx,&yy,&zz);
	 /* printf("v (%f,%f,%f)\n",xx,yy,zz); */
	 appendVertex(&vList,&vListTail,allocVertex(xx,yy,zz));
      }
      else if (buffer[0] == COMMENT) { } /* comment */
      else fprintf(stderr,"?Illegal command: '%c'\n",buffer[0]);
   } /* while */
   if (vList != NULL_VERTEX) { /* process last face (or only) */ 
      appendVertex(&vList,&vListTail, /* append duplicate 1st vertex */
		   allocVertex(vList->xx,vList->yy,vList->zz));
      computePlane(vList->xx,vList->yy,vList->zz,vList->vnext->xx,
		   vList->vnext->yy,vList->vnext->zz,vList->vnext->vnext->xx,
		   vList->vnext->vnext->yy,vList->vnext->vnext->zz,&plane);
      appendFace(fList,&fListTail,allocFace(&color,vList,&plane));
   }

   fclose(fp);
} /* getScene() */
/*** mainBsp.c ***/
