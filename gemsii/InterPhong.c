/******************************************************
InterPhong shading for Scan-line rendering algorithms 

InterPhong shading has been used for rendering the 
synthetic actors Marilyn
Monroe and Humphrey Bogart in the film "Rendez-vous 
a Montreal" directed by Nadia Magnenat Thalmann and 
Daniel Thalmann, 1987

*******************************************************/

#include <math.h>
#include "GraphicsGems.h"

#define RESANTI	3839
#define NBMAXSOURCES	10
#define SQRT3_2	3.464101615
#define NIL 	0

#define	Trunc(v)	((int)floor(v))

/* normalize vector, return in pn */
#define	unity(v,pn)	{ double len ; \
			  len = sqrt((v).x*(v).x+(v).y*(v).y+(v).z*(v).z) ; \
			  (pn)->x=(v).x/len; (pn)->y=(v).y/len; \
			  (pn)->z=(v).z/len; }

typedef struct  {
	double r, g, b;
} Colors;

typedef struct  {
	Colors coul;
	double w, n;
} RecCoul;

/*	Declaration of types used for the datastructures that 
represent the information of a figure for the treatment by 
scanline ( software rendering )
 */

typedef struct blocedge { 
	struct blocedge *edsuiv;	/* next edge in the list */
	struct blocpoly *ptpoly1, *ptpoly2;	/* 
polygons sharing this edge */
	double x, dx;	/* Xmin and Xdelta */
	double z, dz;	/* Zmin and Zdelta */
	double ymax;	/* maximum Y of edge */
	double nx, dnx, ny, dny, nz, dnz;
	double px, dpx, py, dpy, pz, dpz;
} BlockEdge;

typedef struct blocpoly {
	struct blocpoly *polsuiv;	/* next polygon in the list */
	struct T_ptedge *ptlisttrie;
	RecCoul refl;	/* polygon characteristics color, spec.
			 coeff., ... */
	Colors coulpoly;	/* polygon shading */
	Vector3 normalctri;
	double bias, tension;
} BlockPoly;

typedef struct T_ptedge {
	BlockEdge *ptedtrie;
	struct T_ptedge *ptedsuiv;
} PtEdge;

/*
 * Declaration of types concerning the calculated image for the
 * current scanline ( Z-buffer )
 */

typedef struct scanbuf_el {
	Colors c, 	/* final color of this pixel */
	       polycolor;	/* initial color of 
visible polygon */
} ScanBufType [RESANTI + 1]; 

typedef struct depthbuf_el {
	double depth;	/* depth of opaque 
pixel */
} DepthBufType [RESANTI + 1]; 

typedef struct	{
	int	xmin, xmax;
} PosBufType;

/*
 * Declaration of data structure types to store light source information 
 */

static PosBufType	posbuffer;

ScanBufType _scanbuffer;		/* Z-buffer */
DepthBufType _depthbuffer;

static
void intphong(nestime, noriginal, bias, tension)
Vector3 *nestime, *noriginal;
double bias, tension;

/*
    Purpose: interphong interpolation
    Arguments
	nestime 	: estimated normal
	noriginal	: original normal
	bias, tension	: bias and tension 
*/

{
	double fact;
	Vector3	vtemp;

	V3Sub (noriginal, nestime, &vtemp);
	fact = fabs(vtemp.x) + fabs(vtemp.y) + fabs(vtemp.z);
	fact = (fact + bias * (SQRT3_2 - fact)) * tension;
	V3Scale (vtemp,fact*V3Length (vtemp));
	V3Add (nestime, &vtemp,nestime); 
	V3Normalize (nestime);
}

/*===========================================================*/

void shadepoly(ptpoly)
BlockPoly	*ptpoly;

/*
    Purpose: shades a polygon on the current scanline
    Arguments
	ptpoly		: polygon to render
	noscline	: current scanline
*/

{
	BlockEdge	*edge1, *edge2;
	PtEdge	*tripedtrie;
	int		xx;
	double	dxx;
	double	zz, dzz;
	double	diffx;
	double	dnnx, dnny, dnnz;
	Vector3	normal, unitn;
	double	dppx, dppy, dppz;
	Vector3	point;
	Colors	cc;
	RecCoul	ptrefl;
	register struct scanbuf_el	*scanel ;
	register struct depthbuf_el	*depthel ;

	tripedtrie = ptpoly->ptlisttrie;
	ptrefl = ptpoly->refl;
	while (tripedtrie != (PtEdge *)NIL) 
	{
		edge1 = tripedtrie->ptedtrie;
		if (tripedtrie->ptedsuiv != (PtEdge *)NIL) 
		{
			tripedtrie = tripedtrie->ptedsuiv;
			edge2 = tripedtrie->ptedtrie;
		} 
		else 
			abort(" Odd number of edges on scanline");

		dxx = edge2->x - edge1->x;	/* distance between edges
						   on current scanline */
		if (dxx < 1.0)			/* crossing edges ? */
			dxx = 1.0;
		dxx = 1.0 / dxx;		/* increment per pixel */
		xx = Trunc(edge1->x) + 1;	/* first pixel to be 
						   colored */
		diffx = xx - edge1->x;

		dzz = (edge2->z - edge1->z) * dxx;
		zz = edge1->z + dzz * diffx;

		if (xx < posbuffer.xmin)
			posbuffer.xmin = xx;
		if (edge2->x > posbuffer.xmax)
			posbuffer.xmax = Trunc(edge2->x);

		dnnx = (edge2->nx - edge1->nx) * dxx;
		dnny = (edge2->ny - edge1->ny) * dxx;
		dnnz = (edge2->nz - edge1->nz) * dxx;
		normal.x = (edge1->nx + dnnx * diffx) + dnnx;
		normal.y = (edge1->ny + dnny * diffx) + dnny;
		normal.z = (edge1->nz + dnnz * diffx) + dnnz;
		dppx = (edge2->px - edge1->px) * dxx;
		dppy = (edge2->py - edge1->py) * dxx;
		dppz = (edge2->pz - edge1->pz) * dxx;
		point.x = (edge1->px + dppx * diffx) + dppx;
		point.y = (edge1->py + dppy * diffx) + dppy;
		point.z = (edge1->pz + dppz * diffx) + dppz;

		while (xx <= edge2->x) 
		{
			scanel = &_scanbuffer[xx];
			depthel = &_depthbuffer[xx];

			if (zz < depthel->depth) 
			{
				unity(normal, &unitn);
				intphong(&unitn, &ptpoly->normalctri, 
					ptpoly->bias, ptpoly->tension);
				cc = ptpoly->coulpoly;
				depthel->depth = zz;
				scanel->polycolor = ptrefl.coul;
				scanel->c = cc;
			}
			xx = xx + 1;
			zz = zz + dzz;
			normal.x = normal.x + dnnx;
			normal.y = normal.y + dnny;
			normal.z = normal.z + dnnz;
			point.x = point.x + dppx;
			point.y = point.y + dppy;
			point.z = point.z + dppz;
		}
		tripedtrie = tripedtrie->ptedsuiv;
	}
}




