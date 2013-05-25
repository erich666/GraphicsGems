/* smooth.h */
/* header file for polygon smoothing */
/* Andrew S. Glassner / Xerox PARC */

#include <stdio.h>
#include <math.h>

#ifdef STANDALONE_TEST

#define NEWTYPE(x) (x *)malloc((unsigned)(sizeof(x)))

typedef struct Point3Struct {
   double x, y, z;
   } Point3;
typedef Point3 Vector3;
typedef int boolean;
#define TRUE  1
#define FALSE 0

Vector3 *V3Normalize(Vector3 *v);
Vector3 *V3Add(Vector3 *a, Vector3 *b, Vector3 *c);
double V3Dot(Vector3 *a, Vector3 *b);
#else
#include "GraphicsGems.h"
#endif

/********* MACROS and CONSTANTS *********/

/* new array creator */
#define NEWA(x, num) (x *)malloc((unsigned)((num) * sizeof(x)))

#define MARKWAITING    0
#define MARKWORKING    1
#define MARKDONE       2

/* fuzzy comparison macro */
#define FUZZEQ(x,y)  (fabs((x)-(y))<(smooth->fuzz))

/* hash table size; related to HASH */
#define HASH_TABLE_SIZE	       1000

/* quantization increment */
#define QSIZE	 1000.0

#define QUANT(x)     (((int)((x)*QSIZE))/QSIZE)
#define ABSQUANT(x)  (((int)((fabs(x))*QSIZE))/QSIZE)
#define HASH(pt)     ( \
		     (int)(((3*ABSQUANT(pt->x)) + \
			    (5*ABSQUANT(pt->y)) + \
			    (7*ABSQUANT(pt->z))) * \
		      HASH_TABLE_SIZE)) % HASH_TABLE_SIZE

/********* STRUCTS AND TYPES *********/

typedef struct Polygonstruct {
    Point3	*vertices;  /* polygon vertices */
    Vector3	*normals;   /* normal at each vertex */
    Vector3	normal;	    /* normal for polygon */
    int		numVerts;   /* number of vertices */
    void	*user;	    /* user information */
    struct	Polygonstruct *next;
    } Polygon_def;
typedef Polygon_def *Polygon;

typedef struct HashNodestruct {
    Polygon   polygon;	    /* polygon for this vertex */
    int	      vertexNum;    /* which vertex this is */
    int	      marked;	    /* vertex status */
    struct    HashNodestruct *next;
    } HashNode_def;
typedef HashNode_def *HashNode;

typedef struct SmoothStruct {
    HashNode  hashTable[HASH_TABLE_SIZE];
    Polygon   polygonTable;
    Polygon   polyTail;
    double    fuzz;	      /* distance for vertex equality */
    double    fuzzFraction;   /* fraction of model size for fuzz */
    boolean   edgeTest;	      /* apply edging test using minDot */
    float     minDot;	      /* if > this, make sharp edge; see above */
    } Smooth_def;
typedef Smooth_def *Smooth;

/********* public procs ************/
Smooth	 initAllTables();
void	 includePolygon(int numVerts, Point3 *verts, Smooth smooth, void *user);
void	 makeVertexNormals(Smooth smooth);

/********* public option contorl procs ************/
void	 setFuzzFraction(Smooth smooth, float fuzzFraction);
void	 enableEdgePreservation(Smooth smooth, float minDot);
void	 disableEdgePreservation(Smooth smooth);
