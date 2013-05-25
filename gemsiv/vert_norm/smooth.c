/*
 * ANSI C code from the article
 * "Building Vertex Normals from an Unstructured Polygon List"
 * by Andrew Glassner, glassner@parc.xerox.com
 * in "Graphics Gems IV", Academic Press, 1994
 */

/* smooth.c - Compute vertex normals for polygons.
   Andrew S. Glassner / Xerox PARC

  The general idea is to 1) initialize the tables, 2) add polygons one by one,
  3) optionally enable edge preservation, 4) optionally set the fuzz factor,
  5) compute the normals, 6) do something with the new normals, then 7) free
  the new storage.  The calls to do this are:

  1) smooth = initAllTables();
  2) includePolygon(int numVerts, Point3 *verts, Smooth smooth);
  3) (optional) enableEdgePreservation(Smooth smooth, float minDot);
  4) (optional) setFuzzFraction(smooth Smooth, float fuzzFraction);
  5) makeVertexNormals(smooth);
  6) YOUR CODE
  7) freeSmooth();

  Edge preservation is used to retain sharp creases in the model.  If it is
  enabled, then the dot product of each pair of polygons sharing a vertex
  is computed.	If this value is below the value of 'minDot' (that is,
  the two polygons are a certain distance away from flatness), then the
  polygons are not averaged together for the vertex normal.

  If you want to re-compute the results without edge preservation, call
    disableEdgePreservation(smooth);

  The general flow of the algorithm is:
    1. currentHash = scan hashTable
    2. while (any unmarked) {
	3. firstVertex = first unmarked vertex.	 set to MARKWORKING
	4. normal = firstVertex->polygon->normal
	5. scan currentHash.  If vertex = firstVertex
	    6. normal += vertex->polygon->normal
	    7. set vertex to MARKWORKING
	    (end of scan)
	8. set normal to unit length
	9. scan currentHash.  If vertex set to MARKWORKING
	    10. set vertex->normal = normal
	    11. set to MARKDONE
	    (end of scan)
	(end while)

  The HASH macro must always return a non-negative value, even for negative inputs.
  The size of the hash table can be adjusted to taste.
  The fuzz for comparison needs to be matched to the resolution of the model.
*/

#include "smooth.h"

void	 addVertexToTable(Point3 *pt, Polygon polygon, int vNum, Smooth smooth);
void	 makePolyNormal(Polygon polygon);
void	 writeSmooth(FILE *fp, int numPolys);
HashNode getFirstWaitingNode(HashNode node);
void	 processHashNode(HashNode headNode, HashNode firstNode, Smooth smooth);
int	 hashPolys(boolean phase);
void	 writeGeom(int numPolys);
void	 freeSmooth(Smooth smooth);
boolean	 compareVerts(Point3 *v0, Point3 *v1, Smooth smooth);
void	 computeFuzz(Smooth smooth);

/********* ENTRY PROCS *********/

/* add this polygon to the tables */
void includePolygon(int numVerts, Point3 *verts, Smooth smooth, void *user) {
int i;
Point3 *vp, *ovp;
    Polygon polygon = NEWTYPE(Polygon_def);
    polygon->next = NULL;
    if (smooth->polyTail != NULL) {
      smooth->polyTail->next = polygon;
    } else {
	smooth->polygonTable = polygon;
    };
    smooth->polyTail = polygon;
    polygon->vertices = NEWA(struct Point3Struct, numVerts);
    polygon->normals = NEWA(struct Point3Struct, numVerts);
    polygon->user = user;
    vp = polygon->vertices;
    ovp = verts;
    polygon->numVerts = numVerts;

    for (i=0; i<numVerts; i++) {
	vp->x = ovp->x;
	vp->y = ovp->y;
	vp->z = ovp->z;
	addVertexToTable(vp, polygon, i, smooth);
	vp++;
	ovp++;
	};
    makePolyNormal(polygon);
    }

void enableEdgePreservation(Smooth smooth, float minDot) {
    smooth->edgeTest = TRUE;
    smooth->minDot = minDot;
    }

void disableEdgePreservation(Smooth smooth) {
    smooth->edgeTest = FALSE;
    }

void setFuzzFraction(Smooth smooth, float fuzzFraction) {
    smooth->fuzzFraction = fuzzFraction;
    }

/******** PROCEDURES ********/

/* set all the hash-table linked lists to NULL */
Smooth initAllTables() {
int i;
Smooth smooth = NEWTYPE(Smooth_def);
    for (i=0; i<HASH_TABLE_SIZE; i++) smooth->hashTable[i] = NULL;
    smooth->polygonTable = NULL;
    smooth->polyTail = NULL;
    smooth->edgeTest = FALSE;
    smooth->minDot = 0.2;
    smooth->fuzzFraction = 0.001;
    smooth->fuzz = 0.001;
    return(smooth);
    }

/* hash this vertex and add it into the linked list */
void addVertexToTable(Point3 *pt, Polygon polygon, int vNum, Smooth smooth) {
int hash = HASH(pt);
HashNode newNode = NEWTYPE(HashNode_def);
    newNode->next = smooth->hashTable[hash];
    smooth->hashTable[hash] = newNode;
    newNode->polygon = polygon;
    newNode->vertexNum = vNum;
    newNode->marked = MARKWAITING;
    }

/* compute the normal for this polygon using Newell's method */
/* (see Tampieri, Gems III, pg 517) */
void makePolyNormal(Polygon polygon) {
Point3 *vp, *p0, *p1;
int i;
   polygon->normal.x = 0.0; polygon->normal.y = 0.0; polygon->normal.z = 0.0;
   vp = polygon->vertices;
   for (i=0; i<polygon->numVerts; i++) {
      p0 = vp++;
      p1 = vp;
      if (i == polygon->numVerts-1) p1 = polygon->vertices;
      polygon->normal.x += (p1->y - p0->y) * (p1->z + p0->z);
      polygon->normal.y += (p1->z - p0->z) * (p1->x + p0->x);
      polygon->normal.z += (p1->x - p0->x) * (p1->y + p0->y);
      };
   (void) V3Normalize(&(polygon->normal));
   }

/* scan each list at each hash table entry until all nodes are marked */
void makeVertexNormals(Smooth smooth) {
HashNode currentHashNode;
HashNode firstNode;
int i;
    computeFuzz(smooth);
    for (i=0; i<HASH_TABLE_SIZE; i++) {
	currentHashNode = smooth->hashTable[i];
	do {
	    firstNode = getFirstWaitingNode(currentHashNode);
	    if (firstNode != NULL) {
		processHashNode(currentHashNode, firstNode, smooth);
		};
	    } while (firstNode != NULL);
	};
    }

void computeFuzz(Smooth smooth) {
Point3 min, max;
double od, d;
Point3 *v;
int i;
Polygon poly = smooth->polygonTable;
  min.x = max.x = poly->vertices->x;
  min.y = max.y = poly->vertices->y;
  min.z = max.z = poly->vertices->z;
  while (poly != NULL) {
    v = poly->vertices;
    for (i=0; i<poly->numVerts; i++) {
      if (v->x < min.x) min.x = v->x;
      if (v->y < min.y) min.y = v->y;
      if (v->z < min.z) min.z = v->z;
      if (v->x > max.x) max.x = v->x;
      if (v->y > max.y) max.y = v->y;
      if (v->z > max.z) max.z = v->z;
      v++;
      };
    poly = poly->next;
    };
  d = fabs(max.x - min.x);
  od = fabs(max.y - min.y);  if (od > d) d = od;
  od = fabs(max.z - min.z);  if (od > d) d = od;
  smooth->fuzz = od * smooth->fuzzFraction;
  }

/* get first node in this list that isn't marked as done */
HashNode getFirstWaitingNode(HashNode node) {
    while (node != NULL) {
	if (node->marked != MARKDONE) return(node);
	node = node->next;
	};
    return(NULL);
    }

/* are these two vertices the same to with the tolerance? */
boolean compareVerts(Point3 *v0, Point3 *v1, Smooth smooth) {
    float q0, q1;
    q0 = QUANT(v0->x); q1 = QUANT(v1->x); if (!FUZZEQ(q0, q1)) return(FALSE);
    q0 = QUANT(v0->y); q1 = QUANT(v1->y); if (!FUZZEQ(q0, q1)) return(FALSE);
    q0 = QUANT(v0->z); q1 = QUANT(v1->z); if (!FUZZEQ(q0, q1)) return(FALSE);
    return(TRUE);
    }

/* compute the normal for an unmarked vertex */
void processHashNode(HashNode headNode, HashNode firstNode, Smooth smooth) {
    HashNode scanNode = firstNode->next;
    Point3 *firstVert = &(firstNode->polygon->vertices[firstNode->vertexNum]);
    Point3 *headNorm = &(firstNode->polygon->normal);
    Point3 *testVert, *testNorm;
    Point3 normal;
    float ndot;

    firstNode->marked = MARKWORKING;
    normal.x = firstNode->polygon->normal.x;
    normal.y = firstNode->polygon->normal.y;
    normal.z = firstNode->polygon->normal.z;

    while (scanNode != NULL) {
	testVert = &(scanNode->polygon->vertices[scanNode->vertexNum]);
	if (compareVerts(testVert, firstVert, smooth)) {
	    testNorm = &(scanNode->polygon->normal);
	    ndot = V3Dot(testNorm, headNorm);

		if ((!(smooth->edgeTest)) || (ndot > smooth->minDot)) {
		V3Add(&normal, testNorm, &normal);
		scanNode->marked = MARKWORKING;
		};
	    };
	scanNode = scanNode->next;
	};

    V3Normalize(&normal);

    scanNode = firstNode;
    while (scanNode != NULL) {
	if (scanNode->marked == MARKWORKING) {
	    testNorm = &(scanNode->polygon->normals[scanNode->vertexNum]);
	    testNorm->x = normal.x;
	    testNorm->y = normal.y;
	    testNorm->z = normal.z;
	    scanNode->marked = MARKDONE;
	    testVert = &(scanNode->polygon->vertices[scanNode->vertexNum]);
	    };
	scanNode = scanNode->next;
	};
    }

/* free up all the memory */
void freeSmooth(Smooth smooth) {
HashNode headNode;
HashNode nextNode;
Polygon poly;
Polygon nextPoly;
int i;
    for (i=0; i<HASH_TABLE_SIZE; i++) {
	headNode = smooth->hashTable[i];
	while (headNode != NULL) {
	    nextNode = headNode->next;
	    free(headNode);
	    headNode = nextNode;
	    };
	};
    poly = smooth->polygonTable;
    while (poly != NULL) {
	nextPoly = poly->next;
	freePoly(poly);
	poly = nextPoly;
	};
    smooth->polygonTable = NULL;
    free(smooth);
    }

freePoly(polygon) Polygon polygon; {
    if (polygon->vertices != NULL) free(polygon->vertices);
    if (polygon->normals != NULL) free(polygon->normals);
    polygon->next = NULL;
    free(polygon);
    }
