/***** Body.c *****/
#include <gl/gl.h>
#include "Body.h"

enum QuatPart {X, Y, Z, W};
int bodyNPoints = 8;
int bodyNFaces = 7;

float theBodyRadius = 3.0;
float thePoints[][4] = {
    { 3.0,     0.0,	0.0,	1},
    {-1.0,     1.0,	0.0,	1},
    {-1.0,    -1.0,	0.0,	1},
    {-0.75,    0.0,    -0.25,	1},
    { 1.0,     0.0,	0.0,	1},
    {-0.75,    0.0,	0.75,	1},
    {-0.5,    -0.125,	0.0,	1},
    {-0.5,     0.125,	0.0,	1}
};
int theFaceVertices[][4] = {
    {3,	 0, 1, 2},
    {3,	 4, 5, 6},
    {3,	 4, 7, 5},
    {3,	 5, 7, 6},
    {3,	 0, 2, 3},
    {3,	 0, 3, 1},
    {3,	 1, 3, 2},
};
float theFaceNormals[][4] = {
    {0., 0., 1., 0},
    {0.08152896377979659767, -0.978347565357559172, 0.1902342488195253946, 0},
    {0.08152896377979659767, 0.978347565357559172, 0.1902342488195253946, 0},
    {-0.9486832980505137996, 0., -0.3162277660168379332, 0},
    {0.06428243465332250222, -0.2571297386132900089, -0.9642365197998375334, 0},
    {0.06428243465332250222, 0.2571297386132900089, -0.9642365197998375334, 0},
    {-0.7071067811865475244, 0., -0.7071067811865475244, 0},
};
short theFaceColors[][3] = {
    {102, 204, 255},
    {  0, 153, 204},
    {  0, 153, 204},
    {204,  51, 157},
    { 51, 102, 157},
    { 51, 102, 157},
    {102, 102, 172},
};

/* Transform body normals, draw front */
void drawbody(Matrix Rot)
{
    double bodyScale = 1.0/theBodyRadius;
    register int i, j, k, n;

    pushmatrix();
    scale(bodyScale, bodyScale, bodyScale);
    for (j=0; j<bodyNFaces; j++) {
	double dot = Rot[X][Z]*theFaceNormals[j][X]
		    +Rot[Y][Z]*theFaceNormals[j][Y]
		    +Rot[Z][Z]*theFaceNormals[j][Z];
	if (dot>0.0) {	  /* Front-facing polygon, so draw it */
	    short shadedColor[3];
	    dot += 0.4; if (dot>1.0) dot = 1.0;
	    shadedColor[0] = dot*theFaceColors[j][0];
	    shadedColor[1] = dot*theFaceColors[j][1];
	    shadedColor[2] = dot*theFaceColors[j][2];
	    n = theFaceVertices[j][0];
	    RGBcolor(shadedColor[0], shadedColor[1], shadedColor[2]);
	    bgnpolygon();
	    for (k=1; k<=n; k++) {
		i = theFaceVertices[j][k];
		v4f(thePoints[i]);
	    }
	    endpolygon();
	}
    }
    popmatrix();
}
