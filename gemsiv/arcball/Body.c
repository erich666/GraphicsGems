/***** Body.c *****/
#include "../../fakeirisgl.h"
#include "Body.h"


enum QuatPart {X, Y, Z, W};
int bodyNPoints = 8;
int bodyNFaces = 7;

float theBodyRadius = 3.f;
float thePoints[][4] = {
    { 3.f,     0.f,	0.f,	1},
    {-1.f,     1.f,	0.f,	1},
    {-1.f,    -1.f,	0.f,	1},
    {-0.75,    0.f,    -0.25,	1},
    { 1.f,     0.f,	0.f,	1},
    {-0.75,    0.f,	0.75,	1},
    {-0.5,    -0.125,	0.f,	1},
    {-0.5,     0.125,	0.f,	1}
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
    {0.f, 0.f, 1.f, 0.f},
    {0.08152896377979659767f, -0.978347565357559172f, 0.1902342488195253946f, 0.f},
    {0.08152896377979659767f, 0.978347565357559172f, 0.1902342488195253946f, 0.f},
    {-0.9486832980505137996f, 0.f, -0.3162277660168379332f, 0.f},
    {0.06428243465332250222f, -0.2571297386132900089f, -0.9642365197998375334f, 0.f},
    {0.06428243465332250222f, 0.2571297386132900089f, -0.9642365197998375334f, 0.f},
    {-0.7071067811865475244f, 0.f, -0.7071067811865475244f, 0.f},
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
    float bodyScale = 1.f/theBodyRadius;
    register int i, j, k, n;

    pushmatrix();
    scale(bodyScale, bodyScale, bodyScale);
    for (j=0; j<bodyNFaces; j++) {
	float dot = Rot[X][Z]*theFaceNormals[j][X]
		    +Rot[Y][Z]*theFaceNormals[j][Y]
		    +Rot[Z][Z]*theFaceNormals[j][Z];
	if (dot>0.f) {	  /* Front-facing polygon, so draw it */
	    short shadedColor[3];
	    dot += 0.4f; if (dot>1.f) dot = 1.f;
	    shadedColor[0] = (short)(dot*theFaceColors[j][0]);
	    shadedColor[1] = (short)(dot*theFaceColors[j][1]);
	    shadedColor[2] = (short)(dot*theFaceColors[j][2]);
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
