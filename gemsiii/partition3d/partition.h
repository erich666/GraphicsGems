
















/* partition.h: header file for partition module.
 * Copyright (c) Norman Chin
 */
#ifndef _PARTITION_INCLUDED
#define _PARTITION_INCLUDED

typedef enum { ZERO, NEGATIVE, POSITIVE } SIGN;

typedef struct vertexTag {
   double xx,yy,zz;		/* 3D coordinates of vertex */

   struct vertexTag *vnext;	/* ptr to next vertex */
} VERTEX;
#define NULL_VERTEX ((VERTEX *) NULL)

typedef struct {
   int someInfo; /* some face info goes here, ie. material properties */
   
   VERTEX *vhead;		/* ptr to first vertex of face */
} FACE; 
#define NULL_FACE ((FACE *) NULL)

#define TOLER 0.000001
#define ISDOUBLE_EQ(a,b) ((fabs((a)-(b)) >= (double) TOLER) ? 0 : 1)  

#define ISVERTEX_EQ(v1,v2) \
                          (ISDOUBLE_EQ((v1)->xx,(v2)->xx) && \
                           ISDOUBLE_EQ((v1)->yy,(v2)->yy) && \
                           ISDOUBLE_EQ((v1)->zz,(v2)->zz))
#define FREEVERTEX(v) (free((char *) (v)))

/* external functions */
void partitionFaceWithPlane(/* double aa, double bb, double cc, double dd,
			       FACE *face, FACE **faceOn, 
			       FACE **faceNeg, FACE **facePos
		             */
			   );
#endif _PARTITION_INCLUDED


