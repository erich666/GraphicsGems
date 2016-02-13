/* test.c - sample driver for polygon smoother */
/* makes a mesh height field of quadrilaterals and triangles */
/* Andrew S. Glassner / Xerox PARC */

#include <stdlib.h>

#include "smooth.h"

#ifdef STANDALONE_TEST
/* from Graphics Gems library ; for standalone compile */

/* normalizes the input vector and returns it */
Vector3 *V3Normalize(Vector3 *v) {
	float len = sqrtf(V3Dot(v, v));
	if (len != 0.0) { v->x /= len;  v->y /= len; v->z /= len; }
	return(v);
}

/* return vector sum c = a+b */
Vector3 *V3Add(Vector3 *a, Vector3 *b, Vector3 *c) {
	c->x = a->x+b->x;  c->y = a->y+b->y;	c->z = a->z+b->z;
	return(c);
}

/* return the dot product of vectors a and b */
float V3Dot(Vector3 *a, Vector3 *b) {
	return((a->x*b->x)+(a->y*b->y)+(a->z*b->z));
}
#endif

/* z=f(x,y) */
float fofxy(float x, float y) {
	float h;
	h = 2.f * (0.5f - x); if (h < 0) h = -h; h = h * y;
	return(h);
}


void addTriangles(float lx, float ly, float hx, float hy,
	Point3 *vlist, Smooth smooth) {
		Point3 *p = vlist;
		/* make the first triangle */
		p->x = lx;  p->y = ly;  p->z = fofxy(p->x, p->y); p++;
		p->x = hx;  p->y = ly;  p->z = fofxy(p->x, p->y); p++;
		p->x = lx;  p->y = hy;  p->z = fofxy(p->x, p->y); p++;
		includePolygon(3, vlist, smooth, NULL);  /* add the polygon */
		/* make the other triangle */
		p = vlist;
		p->x = hx;  p->y = ly;  p->z = fofxy(p->x, p->y); p++;
		p->x = hx;  p->y = hy;  p->z = fofxy(p->x, p->y); p++;
		p->x = lx;  p->y = hy;  p->z = fofxy(p->x, p->y); p++;
		includePolygon(3, vlist, smooth, NULL);  /* add the polygon */
}

void addQuadrilateral(float lx, float ly, float hx, float hy,
	Point3 *vlist, Smooth smooth) {
		Point3 *p = vlist;
		p->x = lx;  p->y = ly;  p->z = fofxy(p->x, p->y); p++;
		p->x = hx;  p->y = ly;  p->z = fofxy(p->x, p->y); p++;
		p->x = hx;  p->y = hy;  p->z = fofxy(p->x, p->y); p++;
		p->x = lx;  p->y = hy;  p->z = fofxy(p->x, p->y); p++;
		includePolygon(4, vlist, smooth, NULL);  /* add the polygon */
}

void buildMesh(Smooth smooth, int xres, int yres) {
	int x, y;
	Point3 *vlist;
	float dx, dy, lx, ly, hx, hy;
	vlist = NEWA(struct Point3Struct, 4);
	dx = 1.f/((float)(xres));
	dy = 1.f/((float)(yres));
	for (y=0; y<yres; y++) {
		ly = y * dy;
		hy = (y+1) * dy;
		for (x=0; x<xres; x++) {
			lx = x * dx;
			hx = (x+1) * dx;
			if ((x+y)%2 == 0) addTriangles(lx, ly, hx, hy, vlist, smooth);
			else addQuadrilateral(lx, ly, hx, hy, vlist, smooth);
		};
	};
	free(vlist);
}

void savePolys(Smooth smooth) {
	Polygon poly = smooth->polygonTable;
	int i, k;
	Point3 *v, *n;
	printf("NQUAD\n");	/* header for point/normal format */
	while (poly != NULL) {
		for (i=0; i<4; i++) {
			k = i;	  /* we always write 4 points so float 3rd triangle vertex */
			if (i >= poly->numVerts) k = poly->numVerts-1;
			v = &(poly->vertices[k]);
			n = &(poly->normals[k]);
			printf("%f %f %f %f %f %f\n", v->x, v->y, v->z, n->x, n->y, n->z);
		};
		printf("\n");
		poly = poly->next;
	};
}

void freeSmooth(Smooth smooth);

/* make a square height field of quadrilaterals and triangles */
int main(int ac, char *av[]) {
	int xres, yres;
	Smooth smooth;
	if (ac < 3) { printf("use: test x y\n"); exit(-1); };	 /* abrupt, I know */
	xres = atoi(*++av);
	yres = atoi(*++av);
	smooth = initAllTables();		/* initialize */
	buildMesh(smooth, xres, yres);	/* build the mesh (calls includePolygon) */
	enableEdgePreservation(smooth, 0.0);	/* 90 degree folds or more stay crisp */
	makeVertexNormals(smooth);		/* build the normals */
	savePolys(smooth);			/* save the result in a file */
	freeSmooth(smooth);			/* take only normals, leave only footprints */
}
