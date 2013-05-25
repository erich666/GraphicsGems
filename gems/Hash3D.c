/*
A 3D Grid Hashing Function
by Brian Wyvill
from "Graphics Gems", Academic Press, 1990
*/

/* Test Program for 3D hash function.
In C the hash function can be defined in a macro which
avoids a function call
and the bit operations are defined in the language.
*/

#include <stdio.h>
#include <math.h>
#include "GraphicsGems.h"		

#define RANGE       256
#define NBITS       4
#define RBITS       4
#define MASK        0360
#define HASH(a,b,c) ((((a&MASK)<<NBITS|b&MASK)<<NBITS|c&MASK)>>RBITS)
#define HSIZE       1<<NBITS*3
#define IABS(x)     (int)((x) < 0 ? -(x) : (x))

typedef struct {
  double x,y,z;
} Triple, *RefTriple;

typedef struct {   /* linked list of objects to be stored */
  Triple origin;
  struct Object *link;
} Object, *RefObject;

typedef struct {  /* linked list of voxels (object pointers) */
  RefObject objectList;
  struct Voxel *link;
} Voxel, *RefVoxel;

RefVoxel table[HSIZE];  /* Table of pointers to Voxels */


checkrange(z) double z;
{
  if (z < 0 || z >= RANGE)
	fprintf(stderr,"%f out of range\n",z), 		exit(1);
}

double getcoord()
{
  char buf[80];
  double z;
  scanf("%s",buf);
  z = atof(buf);
  checkrange(z);  
  return z;
}

main()
{
  Triple a;
  while (TRUE) {
    printf("Enter object position x y z ===> ");
    a.x = getcoord();
    a.y = getcoord();
    a.z = getcoord();
    printf("\ncoord: %d %d %d Hashes to %d\n",IABS(a.x),IABS(a.y),IABS(a.z),
	   HASH(IABS(a.x), IABS(a.y), IABS(a.z) ));
  };  
}
