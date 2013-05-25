/*  
       Z Buffer Rendering Program Version 10, 02/12/96
       Copyright Raghu Karinthi, West Virginia University	
*/	       

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "./tga/lug.h"
#include "./tga/lugfnts.h"

#define MAT3_EPSILON	1e-12			/* Close enough to zero   */
#define MAT3_PI 	M_PI			/* Pi			  */

typedef double MAT3mat[4][4];		/* 4x4 matrix			 */
typedef double MAT3vec[3];		/* Vector			 */

#define HIBITS                        15
#define LOBITS                        14
#define ZLOBITS                       22

#define FLOATSZM1 31
#define FFRACSZ 23
#define FEXPOFFSET 127
#define FFRACMASK 0x007FFFFF
#define EXPMASK 0x7F800000
#define FFRACSZMLOBITS (FFRACSZ - LOBITS)

#ifndef TRUE
#define TRUE                          1
#endif

#ifndef FALSE
#define FALSE                         0
#endif

#define LOMASK       (~(0xffffffff << LOBITS))
#define HIMASK       ((~(0xffffffff << HIBITS)) << LOBITS)
#define FIX1         (1 << LOBITS)

#define RMASK 0x000000FF
#define GMASK 0x0000FF00
#define BMASK 0x00FF0000

#define WAITING           0 
#define VIEWING           1
#define TRIDATA           2

#define NUM_TRIANGLES     10000
#define X                 0
#define Y                 1
#define Z                 2
#define W                 3

#define WW      500
#define WH      480

#define MAX_INTENSITY     256 
#define NUM_LIGHT_SOURCES 4
#define COLOR_DEPTH       24

typedef long int32;

typedef int int16;

typedef int32 fixpoint;

typedef int32 color;  /* Framebuffer Color */

typedef float MAT3fvec[4];

typedef float MAT3fmat[4][4];

typedef struct Normal_Vertex_struct {
  MAT3fvec vertex;
  MAT3fvec normal;
} Normal_Vertex;

typedef struct Color_Vertex_struct {
  MAT3fvec vertex;
  float red, green, blue;
} Color_Vertex;

typedef Color_Vertex *Color_VertexP;


typedef color FrameBuffer[WH][WW];

typedef fixpoint ZBuffer[WH][WW];

typedef struct _origtriangle {
  Normal_Vertex vertices[3];
  MAT3fvec normal;
  float v0dotn;
} OrigTriangle;

typedef OrigTriangle *OrigTriangleP;

typedef struct color_triangle_struct {
  MAT3fvec normal;
  Color_Vertex vertices[3];
} ColorTriangle;

typedef ColorTriangle *ColorTriangleP;

typedef struct BackgroundColor_struct {
  unsigned char red, green, blue;
} BackGroundColor;

typedef struct LightPoint_struct {
  MAT3fvec location;
  float red, green, blue; 
} LightPoint;
typedef LightPoint *Lights;

typedef struct MtlProperties_struct {
  float red, green, blue, diffuseK, ambientK, c1, c2;
} MtlProp;

/*
 *  The following edge data structure is modified from the code
 *  obtained in Graphics Gems III subdirectory accurate_scan due to Kurt
 *  Fleischer.  Also Refer Lathrop, Kirk, Voorhies, IEEE CG&A 10(5),
 *  1990 for a discussion.
 */

typedef struct edge_struct {
  fixpoint E, DEA, DEB;
  short Ix, AStep, BStep;} edge;

typedef struct SpecLightPoint_struct {
  MAT3fvec location;
  float red, green, blue;
  float ks;
  int16 spec_exp;
} SpecLightPoint;

typedef SpecLightPoint *SpecLightP;

