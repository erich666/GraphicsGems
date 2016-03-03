/*****************************************************************************
* 	room.c 
*
*	This is a test program which constrcuts the Cornell radiosity room with 
*   a ceiling light and two boxes inside. The side faces of the boxes are not 
*	directly illuminated by the light. Therefore, they are a good example of 
*	the color bleeding effects.
*   This program calls IniRad(), DoRad() and CleanUpRad() in rad.c to perform 
*	the radiosity rendering.
*
*	Copyright (C) 1990-1991 Apple Computer, Inc.
*	All rights reserved.
*
*	12/1990 S. Eric Chen	
******************************************************************************/

#include <stdlib.h>
#include "rad.h"

/* a quadrilateral */
typedef struct {
	short verts[4];	/* vertices of the quadrilateral */
	short patchLevel; /* patch subdivision level (how fine to subdivide the quadrilateral?) */
	short elementLevel; /* element subdivision level (how fine to subdivide a patch?) */
	float area; /* area of the quadrilateral */
	TVector3f normal; /* normal of the quadrilateral */
	TSpectra* reflectance; /* diffuse reflectance of the quadrilateral */
	TSpectra* emission; /* emission of the quadrilateral */
} TQuad;

/* input parameters */
TRadParams	params = {
	0.001,			/* convergence threshold */
	0, 0, 0, 0, 0, 0,	/* patches, elements and points; initialize these in InitParams */
	{{ 108, 120, 400 },	/* camera location */
	{ 108, 100, 100 },	/* look at point */
	{ 0, 1, 0 },	/* up vector */
	60, 60,			/* field of view in x, y*/
	1, 550,			/* near, far */
	200, 200,		/* resolution x, y */
	0 },			/* buffer */
	100,			/* hemi-cube resolution */
	250,			/* approximate diameter of the room */
	50,				/* intensity scale */
	1				/* add the ambient term */
};

TPoint3f roomPoints[] = {
	{0.f, 0.f, 0.f},
	{216.f, 0.f, 0.f},
	{216.f, 0.f, 215.f},
	{0.f, 0.f, 215.f},
	{0.f, 221.f, 0.f},
	{216.f, 221.f, 0.f},
	{216.f, 221.f, 215.f},
	{0.f, 221.f, 215.f},

	{85.5f, 220.f, 90.f},
	{130.5f, 220.f, 90.f},
	{130.5f, 220.f, 130.f},
	{85.5f, 220.f, 130.f},

	{53.104f, 0.f, 64.104f},
	{109.36f, 0.f, 96.604f},
	{76.896f, 0.f, 152.896f},
	{20.604f, 0.f, 120.396f},
	{53.104f, 65.f, 64.104f},
	{109.36f, 65.f, 96.604f},
	{76.896f, 65.f, 152.896f},
	{20.604f, 65.f, 120.396f},

	{134.104f, 0.f, 67.104f},
	{190.396f, 0.f, 99.604f},
	{157.896f, 0.f, 155.896f},
	{101.604f, 0.f, 123.396f},
	{134.104f, 130.f, 67.104f},
	{190.396f, 130.f, 99.604f},
	{157.896f, 130.f, 155.896f},
	{101.604f, 130.f, 123.396f}
};

static TSpectra red = { {0.80, 0.10, 0.075} };
static TSpectra yellow = { {0.9, 0.8, 0.1} };
static TSpectra blue = { {0.075, 0.10, 0.35} };
static TSpectra white = { {1.0, 1.0, 1.0} };
static TSpectra lightGrey = { {0.9, 0.9, 0.9} };
static TSpectra black = { {0.0, 0.0, 0.0} };

/* Assume a right-handed coordinate system */
/* Polygon vertices follow counter-clockwise order when viewing from front */
#define numberOfPolys 	18
TQuad roomPolys[numberOfPolys] = {
	{{4, 5, 6, 7},     2, 8, 216.f*215.f, { 0.f,   -1.f,  0.f   }, &lightGrey, &black}, /* ceiling */
	{{0, 3, 2, 1},     3, 8, 216.f*215.f, { 0.f,    1.f,  0.f   }, &lightGrey, &black}, /* floor */
	{{0, 4, 7, 3},     2, 8, 221.f*215.f, { 1.f,    0.f,  0.f   }, &red, &black}, /* wall */
	{{0, 1, 5, 4},     2, 8, 221.f*216.f, { 0.f,    0.f,  1.f   }, &lightGrey, &black}, /* wall */
	{{2, 6, 5, 1},     2, 8, 221.f*215.f, {-1.f,    0.f,  0.f   }, &blue, &black}, /* wall */
	{{8, 9, 10, 11},   2, 1,  40.f* 45.f, { 0.f,   -1.f,  0.f   }, &black, &white}, /* light */
	{{16, 19, 18, 17}, 1, 5,  65.f* 65.f, { 0.f,    1.f,  0.f   }, &yellow, &black}, /* box 1 */
	{{12, 13, 14, 15}, 1, 1,  65.f* 65.f, { 0.f,   -1.f,  0.f   }, &yellow, &black},
	{{12, 15, 19, 16}, 1, 5,  65.f* 65.f, {-0.866f, 0.f, -0.5f  }, &yellow, &black},
	{{12, 16, 17, 13}, 1, 5,  65.f* 65.f, { 0.5f,   0.f, -0.866f}, &yellow, &black},
	{{14, 13, 17, 18}, 1, 5,  65.f* 65.f, { 0.866f, 0.f,  0.5f  }, &yellow, &black},
	{{14, 18, 19, 15}, 1, 5,  65.f* 65.f, {-0.5f,   0.f,  0.866f}, &yellow, &black},
	{{24, 27, 26, 25}, 1, 5,  65.f* 65.f, { 0.f,    1.f,  0.f   }, &lightGrey, &black}, /* box 2 */
	{{20, 21, 22, 23}, 1, 1,  65.f* 65.f, { 0.f,   -1.f,  0.f   }, &lightGrey, &black},
	{{20, 23, 27, 24}, 1, 6,  65.f*130.f, {-0.866f, 0.f, -0.5f  }, &lightGrey, &black},
	{{20, 24, 25, 21}, 1, 6,  65.f*130.f, { 0.5f,   0.f, -0.866f}, &lightGrey, &black},
	{{22, 21, 25, 26}, 1, 6,  65.f*130.f, { 0.866f, 0.f,  0.5f  }, &lightGrey, &black},
	{{22, 26, 27, 23}, 1, 6,  65.f*130.f, {-0.5f,   0.f,  0.866f}, &lightGrey, &black},
};


/* Compute the xyz coordinates of a point on a quadrilateral given its u, v coordinates using bi-linear mapping */
void UVToXYZ(const TPoint3f quad[4], float u, float v, TPoint3f* xyz)
{
	xyz->x = quad[0].x * (1-u)*(1-v) + quad[1].x * (1-u)*v + quad[2].x * u*v + 			quad[3].x * u*(1-v);
	xyz->y = quad[0].y * (1-u)*(1-v) + quad[1].y * (1-u)*v + quad[2].y * u*v + 			quad[3].y * u*(1-v);
	xyz->z = quad[0].z * (1-u)*(1-v) + quad[1].z * (1-u)*v + quad[2].z * u*v + 			quad[3].z * u*(1-v);
}

#define Index(i, j) ((i)*(nv+1)+(j))

int iOffset; 	/* index offset to the point array */
TPatch* pPatch;
TElement* pElement;
TPoint3f* pPoint;

/* Mesh a quadrilateral into patches and elements */
/* Output goes to pPatch, pElement, pPoint */
void MeshQuad(TQuad* quad)
{
	TPoint3f pts[4];
	int nu, nv;
	double	du, dv;
	int i, j;
	double u, v;
	int nPts=0;
	float fi, fj;
	int pi, pj;
	
	/* Calculate element vertices */
	for (i=0; i<4; i++)
		pts[i] = roomPoints[quad->verts[i]];
	nu = nv = quad->patchLevel * quad->elementLevel+1;
	du = 1.0 / (nu-1); dv = 1.0 / (nv-1);
	for (i = 0, u = 0; i < nu; i++, u += du)
		for (j = 0, v = 0; j < nv; j++, v += dv, nPts++)
			UVToXYZ(pts, (float)u, (float)v, pPoint++);

	/* Calculate elements */
	nu = nv = quad->patchLevel*quad->elementLevel;
	du = 1.0 / nu; dv = 1.0 / nv;
	for (i = 0, u = du/2.0; i < nu; i++, u += du)
		for (j = 0, v = dv/2.0; j < nv; j++, v += dv, pElement++) {
			pElement->normal = quad->normal;
			pElement->nVerts = 4;
			pElement->verts = (unsigned long*)calloc(4, sizeof(unsigned long));
			pElement->verts[0] = Index(i, j)+iOffset;
			pElement->verts[1] = Index(i+1, j)+iOffset;
			pElement->verts[2] = Index(i+1, j+1)+iOffset;
			pElement->verts[3] = Index(i, j+1)+iOffset;
			pElement->area = quad->area / (nu*nv);
			/* find out the parent patch */
			fi = (float)i/(float)nu;
			fj = (float)j/(float)nv;
			pi = (int)(fi*(float)(quad->patchLevel));
			pj = (int)(fj*(float)(quad->patchLevel));
			pElement->patch = pPatch+pi*quad->patchLevel+pj;
	}
	
	/* Calculate patches */
	nu = quad->patchLevel; nv=quad->patchLevel;
	du = 1.0 / nu; dv = 1.0 / nv;
	for (i = 0, u = du/2.0; i < nu; i++, u += du)
		for (j = 0, v = dv/2.0; j < nv; j++, v += dv, pPatch++) {
			UVToXYZ(pts, (float)u, (float)v, &pPatch->center);
			pPatch->normal = quad->normal;
			pPatch->reflectance = quad->reflectance;
			pPatch->emission = quad->emission;
			pPatch->area = quad->area / (nu*nv);
		}
	
	iOffset += nPts;
}

/* Initialize input parameters */
void InitParams()
{
	int i;

	/* compute the total number of patches */
	params.nPatches=0;
	for (i=numberOfPolys; i--; )
		params.nPatches += roomPolys[i].patchLevel*roomPolys[i].patchLevel;
	params.patches = (TPatch*)calloc(params.nPatches, sizeof(TPatch));

	/* compute the total number of elements */
	params.nElements=0;
	for (i=numberOfPolys; i--; )
		params.nElements += roomPolys[i].elementLevel*roomPolys[i].patchLevel*
						roomPolys[i].elementLevel*roomPolys[i].patchLevel;
	params.elements = (TElement*)calloc(params.nElements, sizeof(TElement));

	/* compute the total number of element vertices */
	params.nPoints=0;
	for (i=numberOfPolys; i--; )
		params.nPoints += (roomPolys[i].elementLevel*roomPolys[i].patchLevel+1)*
						(roomPolys[i].elementLevel*roomPolys[i].patchLevel+1);
	params.points = (TPoint3f*)calloc(params.nPoints, sizeof(TPoint3f));

	/* mesh the room to patches and elements */
	iOffset = 0;
	pPatch= params.patches;
	pElement= params.elements;
	pPoint= params.points;
	for (i=0; i<numberOfPolys; i++)
		MeshQuad(&roomPolys[i]);
	
	params.displayView.buffer= (unsigned long*)calloc(
		params.displayView.xRes*params.displayView.yRes, sizeof(unsigned long));
	params.displayView.wid=0;
}

int main()
{
	InitParams();
	InitRad(&params);
	DoRad();
	CleanUpRad();
}



