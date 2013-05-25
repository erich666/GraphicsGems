/*
GROUPING NEARLY COPLANAR POLYGONS INTO COPLANAR SETS


David Salesin
Filippo Tampieri

Cornell University
*/









/*
    This code partitions a given set of arbitrary 3D polygons into
    subsets of coplanar polygons.
    The input polygons are organized in a linked list and the
    resulting subsets of coplanar polygons are returned in the form
    of a binary tree; each node of the binary tree contains a
    different subset (once again stored as a linked list) and its
    plane equation.  An inorder traversal of the binary tree will
    return the sets of coplanar polygons sorted by plane equations
    according to the total ordering imposed by the relation
    implemented by the routine 'comparePlaneEqs'.
*/

#include <stdio.h>
#include <math.h>

#define X   0
#define Y   1
#define Z   2
#define D   3

#define VZERO(v)    (v[X] = v[Y] = v[Z] = 0.0)
#define VNORM(v)    (sqrt(v[X] * v[X] + v[Y] * v[Y] + v[Z] * v[Z]))
#define VDOT(u, v)  (u[0] * v[0] + u[1] * v[1] + u[2] * v[2])
#define VINCR(u, v) (u[X] += v[X], u[Y] += v[Y], u[Z] += v[Z])

typedef float Vector[3];
typedef Vector Point;
typedef Vector Normal;
typedef float Plane[4];

/*
    Polygon--a polygon is stored as an array 'vertices' of size
    'numVertices'.  Pointer 'next' is used to implement sets of
    polygons through linked lists.
*/
typedef struct polygon {
    Point *vertices;
    int numVertices;
    struct polygon *next;
} Polygon;

/*
    Node--each node stores a set of coplanar polygons. The set is
    implemented as a linked list pointed to by 'plist', and the
    plane equation of the set is stored in 'plane'. Pointers 'left'
    and 'right' point to the subtrees containing sets of coplanar
    polygons with plane equations respectively less than and
    greater than that stored in 'plane'.
*/
typedef struct node {
        Plane plane;
        Polygon *plist;
        struct node *left, *right;
} Node;

static float linEps;  /* tolerances used by 'comparePlaneEq' to */
static float cosEps;  /* account for numerical errors           */

/* declare local routines */
static void computePlaneEq() ;
static Node *insertPlaneEq() ;
static int comparePlaneEqs() ;

/*
    coplanarSets--takes as input a linked list of polygons 'plist',
    and two tolerances 'linearEps' and 'angularEps' and returns a
    binary tree of sets of coplanar polygons.
    The tolerances are used to deal with floating-point arithmetic
    and numerical errors when comparing plane equations; two plane
    equations are considered equal if the angle between their
    respective normals is less than or equal to angularEps (in
    degrees) and the distance between the two planes is less than
    or equal to linearEps.
*/
Node *coplanarSets(plist, linearEps, angularEps)
Polygon *plist;
float linearEps, angularEps;
{
    Node *tree;
    Plane plane;
    void computePlaneEq();
    Node *pset, *insertPlaneEq();
    Polygon *polygon, *nextPolygon;

    /* initialize the tolerances used by comparePlaneEqs() */
    linEps = linearEps;
    cosEps = cos(angularEps * M_PI / 180.0);

    /* place each input polygon in the appropriate set
       of coplanar polygons
    */
    tree = NULL;
    polygon = plist;
    while(polygon != NULL) {
        nextPolygon = polygon->next;
        /* first, compute the plane equation of the polygon */
        computePlaneEq(polygon, plane);
        /* then, find the set of coplanar polygons with this plane
    	   equation or create a new, empty one if none exist
        */
        tree = insertPlaneEq(tree, plane, &pset);
        /* finally, add the polygon to the selected set of
    	   coplanar polygons
    	*/
        polygon->next = pset->plist;
        pset->plist = polygon;
        /* go to the next polygon in the input list. Note that the
    	   'next' field in the polygon structure is reused to
    	   assemble lists of coplanar polygons; thus the necessity
    	   for 'nextPolygon'
        */
        polygon = nextPolygon;
    }

    return(tree);
}

/*
    computePlaneEq--takes as input a pointer 'polygon' to an
    arbitrary 3D polygon and returns in 'plane' the normalized
    (unit normal) plane equation of the polygon.
    Newell's method (see "Newell's Method for Computing the Plane
    Equation of a Polygon" in this volume) is used for the
    computation.
*/
static void computePlaneEq(polygon, plane)
Polygon *polygon;
Plane plane;
{
    int i;
    Point refpt;
    Normal normal;
    float *u, *v, len;

    /* first, compute the normal of the input polygon */
    VZERO(normal);
    VZERO(refpt);
    for(i = 0; i < polygon->numVertices; i++) {
        u = polygon->vertices[i];
        v = polygon->vertices[(i + 1) % polygon->numVertices];
        normal[X] += (u[Y] - v[Y]) * (u[Z] + v[Z]);
        normal[Y] += (u[Z] - v[Z]) * (u[X] + v[X]);
        normal[Z] += (u[X] - v[X]) * (u[Y] + v[Y]);
        VINCR(refpt, u);
    }
    /* then, compute the normalized plane equation using the
       arithmetic average of the vertices of the input polygon to
       determine its last coefficient. Note that, for efficiency,
       'refpt' stores the sum of the vertices rather than the
       actual average; the division by 'polygon->numVertices' is
       carried out together with the normalization when computing
       'plane[D]'.
    */
    len = VNORM(normal);
    plane[X] = normal[X] / len;
    plane[Y] = normal[Y] / len;
    plane[Z] = normal[Z] / len;
    len *= polygon->numVertices;
    plane[D] = -VDOT(refpt, normal) / len;
}

/*
    insertPlaneEq--takes as input a binary tree 'tree' of sets of
    coplanar polygons, and a plane equation 'plane', and returns
    a pointer 'pset' to a set of coplanar polygons with the given
    plane equation. A new, empty set is created if none is found.
*/

static Node *insertPlaneEq(tree, plane, pset)
Node *tree, **pset;
Plane plane;
{
    int i, c, comparePlaneEqs();

    if(tree == NULL) {
        /* the input plane is not in the tree.
    	   Create a new set for it
    	*/
        tree = (Node *)malloc(sizeof(Node));
        for(i = 0; i < 4; i++)
            tree->plane[i] = plane[i];
        tree->plist = NULL; /* no polygons in this set for now */
        tree->left = tree->right = NULL;
        *pset = tree;
    } else {
        /* compare the input plane equation with that
    	   of the visited node
    	*/
        c = comparePlaneEqs(plane, tree->plane);
        if(c < 0)
            tree->left = insertPlaneEq(tree->left, plane, pset);
        else if(c > 0)
            tree->right = insertPlaneEq(tree->right, plane, pset);
        else
            /* the input plane is approximately equal to that
    		   of this node
    		*/
            *pset = tree;
    }

    return(tree);
}

/*
    comparePlaneEqs--compares two plane equations, 'p1' and 'p2',
    and returns an integer less than, equal to, or greater than
    zero, depending on whether 'p1' is less than, equal to, or
    greater than 'p2'.  The two global variables, 'linEps' and
    'cosEps' are tolerances used to account for numerical errors.
*/
static int comparePlaneEqs(p1, p2)
Plane p1, p2;
{
    int ret;
    float cosTheta;

    /* compute the cosine of the angle between the normals
       of the two planes
    */
    cosTheta = VDOT(p1, p2);

    if(cosTheta < 0.0)
        ret = -1;
    else if(cosTheta < cosEps)
        ret = 1;
    else {
        /* the two planes are parallel and oriented in
    	   the same direction
    	*/
        if(p1[3] + linEps < p2[3])
            ret = -1;
        else if(p2[3] + linEps < p1[3])
            ret = 1;
        else
            /* the two planes are equal */
            ret = 0;
    }

    return ret;
}

