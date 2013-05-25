/*******************************************************************************
 The efficiency of the following c-code can be increased by defining macros at
 appropriate places. For example, the Leaf() function can be replaced by a 
 corresponding macros. Another way to increase the code efficiency is by passing
 the address of structures instead of the structures themselves.
 ******************************************************************************/

/*******************************************************************************
 Supporting Data Structures 
 ******************************************************************************/

#include <stdio.h>
#include "GraphicsGems.h"

typedef struct {
     Point3   min, max;        /* extent of the primitive */

     /* definition for different primitives */

} GeomObj, *GeomObjPtr;

typedef struct {

    /* Link list of primitives */

    int  length;                /* Length of the link list */
} GeomObjList;

typedef struct {
       Point3     origin;       /* ray origin */
       Point3     direction;    /* unit vector, indicating ray direction */
} Ray;

typedef struct BinNode {
    Point3      min, max;      /* extent of node */
    GeomObjList members;       /* list of enclosed primitives */
    struct BinNode *child[2];  /* pointers to children nodes, if any */

    /* distance to the plane which subdivides the children */
    double  (*DistanceToDivisionPlane)();

    /* children near/far ordering relative to a input point */
    void    (*GetChildren)();

} BinNode, *BinNodePtr;

typedef struct {
    Point3     min, max;       /* extent of the entire bin tree */
    GeomObjList members;       /* list of all of the primitives */
    int         MaxDepth;      /* max allowed depth of the tree */
    int         MaxListLength; /* max primitive allowed in a leaf node */
    BinNodePtr  root;          /* root of the entire bin tree */
} BinTree;


/*******************************************************************************
 Data structure for a simple stack. This is necessary for implementing an 
 efficient linear BSP tree walking. A Stack size of 50 means it is possible
 to support a BSP tree of up to depth 49 and NO MORE!!! It should be enough for 
 the next decade or so.
 ******************************************************************************/
#define STACKSIZE  50

typedef struct {
    BinNodePtr node;
    double     min, max;
} StackElem;

typedef struct {
    int       stackPtr;
    StackElem stack[STACKSIZE];
} Stack, *StackPtr;


/*******************************************************************************
 Stack operations.
 ******************************************************************************/
void InitStack(stack)
StackPtr stack;
{
    stack->stack[0].node = NULL;
    stack->stackPtr = 1;
}

void push(stack, node, min, max)
StackPtr    stack;
BinNodePtr  node;
double min, max;
{
    stack->stack[stack->stackPtr].node = node;
    stack->stack[stack->stackPtr].min = min;
    stack->stack[stack->stackPtr].max = max;
    (stack->stackPtr)++;
}

void pop(stack, node, min, max)
StackPtr   stack;
BinNodePtr *node;
double     *min, *max;
{
    (stack->stackPtr)--;
    *node = stack->stack[stack->stackPtr].node;
    *min = stack->stack[stack->stackPtr].min;
    *max = stack->stack[stack->stackPtr].max;
}


/*******************************************************************************
 Returns the distance between origin and plane, measured along the input 
 direction. direction is a unit vector.

 Entry:
   plane     - subdivision plane of current node
   origin    - origin of the ray
   direction - direction of the ray, must be a unit vector
 
 Exit:
   returns the distance between the origin and plane measured along
   the direction

 Note:
   there is a function for each of the three subdivision planes
 ******************************************************************************/

double DistanceToXPlane(plane, ray)
Point3 plane;
Ray    ray;
{
   return ( (plane.x - ray.origin.x) / ray.direction.x);
}

double DistanceToYPlane(plane, ray)
Point3 plane;
Ray    ray;
{
   return ( (plane.y - ray.origin.y) / ray.direction.y);
}

double DistanceToZPlane(plane, ray)
Point3 plane;
Ray    ray;
{
   return ( (plane.z - ray.origin.z) / ray.direction.z);
}


/*******************************************************************************
 Determines which of the half space of the two children contains origin, return 
 that child as near, the other as far.

 Entry:
   currentNode - node currently working on
   origin      - origin of the ray

 Exit:
   near - node whose half plane contains the origin
   far  - node whose half plane does not contain the origin

 Note:
   there is a function for each of the three subdivision planes
 ******************************************************************************/
void GetXChildren(currentNode, origin, near, far)
BinNodePtr currentNode, *near, *far;
Point3 origin; 
{

 /* remember that child[0]->max or child[1]->min is the subdivision plane */

    if ( currentNode->child[0]->max.x >= origin.x ) {
        *near = currentNode->child[0];
        *far = currentNode->child[1];
    } else {
        *far = currentNode->child[0];
        *near = currentNode->child[1];
    }
}

void GetYChildren(currentNode, origin, near, far)
BinNodePtr currentNode, *near, *far;
Point3 origin; 
{

 /* remember that child[0]->max or child[1]->min is the subdivision plane */

    if ( currentNode->child[0]->max.y >= origin.y ) {
        *near = currentNode->child[0];
        *far = currentNode->child[1];
    } else {
        *far = currentNode->child[0];
        *near = currentNode->child[1];
    }
}

void GetZChildren(currentNode, origin, near, far)
BinNodePtr currentNode, *near, *far;
Point3 origin; 
{

 /* remember that child[0]->max or child[1]->min is the subdivision plane */

    if ( currentNode->child[0]->max.z >= origin.z ) {
        *near = currentNode->child[0];
        *far = currentNode->child[1];
    } else {
        *far = currentNode->child[0];
        *near = currentNode->child[1];
    }
}


/*******************************************************************************
 Some miscellaneous supporting functions.
 ******************************************************************************/

boolean Leaf(node)
BinNodePtr node;
{
    return (node->child[0] == NULL);
}

boolean PointInNode(node, pt)
BinNodePtr node;
Point3 pt;
{
    return ((pt.x >= node->min.x ) && (pt.y >= node->min.y ) && 
        (pt.z >= node->min.z ) && (pt.x <= node->max.x ) && 
        (pt.y <= node->max.y ) && (pt.z <= node->max.z ));
}

boolean GeomInNode(node, obj)
BinNodePtr node;
GeomObjPtr obj;
{
    if (node->min.x > obj->max.x || node->max.x < obj->min.x) return FALSE;
    if (node->min.y > obj->max.y || node->max.y < obj->min.y) return FALSE;
    if (node->min.z > obj->max.z || node->max.z < obj->min.z) return FALSE;
    return TRUE;
}

void PointAtDistance(ray, distance, pt)
Ray    ray;
double distance;
Point3 *pt;
{
    pt->x = ray.origin.x + distance * ray.direction.x;
    pt->y = ray.origin.y + distance * ray.direction.y;
    pt->z = ray.origin.z + distance * ray.direction.z;
}

boolean RayBoxIntersect(ray, min, max, returnMin, returnMax)
Ray ray;
Point3 min, max;
double *returnMin, *returnMax;
{
    /*
     This routine intersects the ray with the box 
     defined by min and max, returns the intersection 
     status. If ray successfully intersects the box, 
     then this routine also returns the distances 
     (from the ray origin) to the two points that the
     ray intersects the box on.

     For example, refer to Graphics Gems I, pp. 395 (736)
     */
}

boolean RayObjIntersect(ray, objList, obj, distance)
Ray         ray;
GeomObjList objList;
GeomObj     *obj;
double      *distance;
{
    /* 
    This routine intersects ray with all of the objects 
    in the objList and returns the closest intersection 
    distance and the interesting object, if there is one.
    */
}



/*******************************************************************************
 Traverses ray through BSPTree and intersects ray with all of the objects along
 the way. Returns the closest intersection distance and the intersecting object
 if there is one.

 Entry:
   ray     - the ray being traced
   BSPTree - the BSP tree enclosing the entire environment

 Exit:
   obj      - the first object that intersects the ray
   distance - distance to the intersecting object
 ******************************************************************************/
boolean RayTreeIntersect(ray, BSPTree, obj, distance)
    Ray     ray;
    BinTree BSPTree;
    GeomObj *obj;
    double  *distance;
{
    StackPtr stack;
    BinNodePtr currentNode, nearChild, farChild;
    double dist, min, max;
    Point3 p;

/* test if the whole BSP tree is missed by the input ray */

    if (!RayBoxIntersect(ray, BSPTree.min, BSPTree.max, &min, &max))
       return FALSE;

    stack = (StackPtr)malloc(sizeof(Stack));
    InitStack(stack);

    currentNode = BSPTree.root;

    while (currentNode != NULL) {

        while ( !(Leaf(currentNode)) ) {

            dist = currentNode->DistanceToDivisionPlane(
                             currentNode->child[0]->max, ray);
            currentNode->GetChildren(
                             currentNode, ray.origin, nearChild, farChild);

            if ( (dist>max) || (dist<0) ) {
                currentNode = nearChild;
            } else if (dist<min)  {
                      currentNode = farChild;
                   } else {
                      push(stack, farChild, dist, max);
                      currentNode = nearChild;
                      max = dist;
                   }
        }

        if ( RayObjIntersect(ray, currentNode->members, obj, distance) ) {
            PointAtDistance(ray, distance, &p);
            if (PointInNode(currentNode, p))
                return TRUE;
        }
        pop(stack, &currentNode, &min, &max);
    }
    return FALSE;
}



/*******************************************************************************
 Builds the BSP tree by subdividing along the center of x, y, or z bounds, one
 each time this function is called. This function calls itself recursively until
 either the tree is deeper than MaxDepth or all of the tree leaves contains less
 than MaxListLength of objects.

 Entry:
   node          - node currently working on
   depth         - current tree depth
   MaxDepth      - Max allowed tree depth
   MaxListLength - Max allowed object list length of a leave node
   axis          - subdivision axis for the children of node
                   (0-x, 1-y, 2-z)
 ******************************************************************************/
void Subdivide(node, depth, MaxDepth, MaxListLength, axis)
BinNodePtr  node; 
int depth,         /* current tree depth */
    MaxDepth,      /* the specified max allowed depth */
    MaxListLength, /* the specified max allowed list length */
    axis;          /* current subdivision plane, 1-x, 2-y, 3-z */
{
    int i,     nextAxis;
    GeomObjPtr ObjPtr;

    node->child[0] = node->child[1] = NULL;

    if ((node->members.length > MaxListLength) && (depth < MaxDepth)) {

       for (i = 0; i < 2; i++) {

           node->child[i] = (BinNodePtr)malloc(sizeof(BinNode));
           node->child[i]->min.x = node->min.x;
           node->child[i]->min.y = node->min.y;
           node->child[i]->min.z = node->min.z;
           node->child[i]->max.x = node->max.x;
           node->child[i]->max.y = node->max.y;
           node->child[i]->max.z = node->max.z;

           if (axis == 1) {

           /* current subdivision plane is x */
               node->child[i]->min.x = 
                        node->min.x + 0.5 * i * (node->max.x - node->min.x);
               node->child[i]->max.x = 
                        node->min.x + 0.5 * (i+1) * (node->max.x - node->min.x);

           /* child subdivision plane will be y */
               nextAxis = 2;
               node->child[i]->DistanceToDivisionPlane = DistanceToYPlane;
               node->child[i]->GetChildren = GetYChildren;

           } else if (axis == 2) {

              /* current subdivision plane is y */

               node->child[i]->min.y = 
                        node->min.y + 0.5 * i * (node->max.y - node->min.y);
               node->child[i]->max.y = 
                        node->min.y + 0.5 * (i+1) * (node->max.y - node->min.y);

              /* child subdivision plane will be z */
               nextAxis = 3;
               node->child[i]->DistanceToDivisionPlane = DistanceToZPlane;
               node->child[i]->GetChildren = GetZChildren;

              } else {

                 /* current subdivision plane is z */
                    node->child[i]->min.z = 
                        node->min.z + 0.5 * i * (node->max.z - node->min.z);
                    node->child[i]->max.z = 
                        node->min.z + 0.5 * (i+1) * (node->max.z - node->min.z);

                 /* child subdivision plane will be x */
                    nextAxis = 1;
                    node->child[i]->DistanceToDivisionPlane = DistanceToXPlane;
                    node->child[i]->GetChildren = GetXChildren;
              }

          ObjPtr = FirstOfLinkList(node->members);
          while (ObjPtr != NULL) {
              if (GeomInNode(node->child[i], ObjPtr))
                  AddToLinkList(node->child[i]->members, ObjPtr);
              ObjPtr = NextOfLinkList(node->members);
          }
          Subdivide(node->child[i], depth+1, MaxDepth, MaxListLength, nextAxis);
       }
    }
}



/*******************************************************************************
 Initialize and start the building of BSPTree.

 Entry:
   BSPTree       - The BSPTree enclosing the entire scene
 ******************************************************************************/
void InitBinTree(BSPTree)
BinTree *BSPTree;
{
    CalculateTheExtentOfTheBinTree(&(BSPTree->min), &(BSPTree->max));
    /* BSPTree->members = ObjectsWithinTheExtent(BSPTree->min, BSPTree->max);*/
    BSPTree->MaxDepth = GetMaxAllowedDepth();
    BSPTree->MaxListLength = GetMaxAllowedListLength();

/* Start building the BSPTree by subdividing along the x axis first */

    BSPTree->root = (BinNodePtr)malloc(sizeof(BinNode));
    BSPTree->root->min = BSPTree->min;
    BSPTree->root->max = BSPTree->max;
    BSPTree->root->DistanceToDivisionPlane = DistanceToXPlane;
    BSPTree->root->GetChildren = GetXChildren;
    DuplicateLinkList(BSPTree->root->members, BSPTree->members);

    Subdivide(BSPTree->root, 0, BSPTree->MaxDepth, BSPTree->MaxListLength, 1);
}

