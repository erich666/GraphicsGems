/*
 * GraphicsGems.h
 * Version 1.0 - Andrew Glassner
 * from "Graphics Gems", Academic Press, 1990
 */

#ifndef GG_H

#define GG_H 1

/*********************/
/* 2d geometry types */
/*********************/

typedef struct Point2Struct {	/* 2d point */
    double x, y;
    } Point2;
typedef Point2 Vector2;

typedef struct IntPoint2Struct {    /* 2d integer point */
    int x, y;
    } IntPoint2;

typedef struct Matrix3Struct {	/* 3-by-3 matrix */
    double element[3][3];
    } Matrix3;

typedef struct Box2dStruct {	    /* 2d box */
    Point2 min, max;
    } Box2;


/*********************/
/* 3d geometry types */
/*********************/

typedef struct Point3Struct {	/* 3d point */
    double x, y, z;
    } Point3;
typedef Point3 Vector3;

typedef struct IntPoint3Struct {    /* 3d integer point */
    int x, y, z;
    } IntPoint3;


typedef struct Matrix4Struct {	/* 4-by-4 matrix */
    double element[4][4];
    } Matrix4;

typedef struct Box3dStruct {	    /* 3d box */
    Point3 min, max;
    } Box3;



/***********************/
/* one-argument macros */
/***********************/

/* absolute value of a */
#define ABS(a)	    (((a)<0) ? -(a) : (a))

/* round a to nearest integer towards 0 */
#define FLOOR(a)	((a)>0 ? (int)(a) : -(int)(-a))

/* round a to nearest integer away from 0 */
#define CEILING(a) \
((a)==(int)(a) ? (a) : (a)>0 ? 1+(int)(a) : -(1+(int)(-a)))

/* round a to nearest int */
#define ROUND(a)    ((a)>0 ? (int)(a+0.5) : -(int)(0.5-a))

/* take sign of a, either -1, 0, or 1 */
#define ZSGN(a)	    (((a)<0) ? -1 : (a)>0 ? 1 : 0)

/* take binary sign of a, either -1, or 1 if >= 0 */
#define SGN(a)	    (((a)<0) ? -1 : 0)

/* shout if something that should be true isn't */
#define ASSERT(x) \
if (!(x)) fprintf(stderr," Assert failed: x\n");

/* square a */
#define SQR(a)	    ((a)*(a))


/***********************/
/* two-argument macros */
/***********************/

/* find minimum of a and b */
#define MIN(a,b)    (((a)<(b))?(a):(b))

/* find maximum of a and b */
#define MAX(a,b)    (((a)>(b))?(a):(b))

/* swap a and b (see Gem by Wyvill) */
#define SWAP(a,b)   { a^=b; b^=a; a^=b; }

/* linear interpolation from l (when a=0) to h (when a=1)*/
/* (equal to (a*h)+((1-a)*l) */
#define LERP(a,l,h) ((l)+(((h)-(l))*(a)))

/* clamp the input to the specified range */
#define CLAMP(v,l,h)	((v)<(l) ? (l) : (v) > (h) ? (h) : v)


/****************************/
/* memory allocation macros */
/****************************/

/* create a new instance of a structure (see Gem by Hultquist) */
#define NEWSTRUCT(x)	(struct x *)(malloc((unsigned)sizeof(struct x)))

/* create a new instance of a type */
#define NEWTYPE(x)  (x *)(malloc((unsigned)sizeof(x)))


/********************/
/* useful constants */
/********************/

#define PI	3.141592    /* the venerable pi */
#define PITIMES2    6.283185	/* 2 * pi */
#define PIOVER2	    1.570796	/* pi / 2 */
#define E	2.718282    /* the venerable e */
#define SQRT2	    1.414214	/* sqrt(2) */
#define SQRT3	    1.732051	/* sqrt(3) */
#define GOLDEN	    1.618034	/* the golden ratio */
#define DTOR	    0.017453	/* convert degrees to radians */
#define RTOD	    57.29578	/* convert radians to degrees */


/************/
/* booleans */
/************/

#define TRUE	    1
#define FALSE	    0
#define ON	1
#define OFF	    0
typedef int boolean;		/* boolean data type */
typedef boolean flag;		/* flag data type */


/******************/
/*   3d Library	  */
/******************/

/* returns squared length of input vector */
extern double V3SquaredLength(Vector3 *);
/* returns length of input vector */
extern double V3Length(Vector3 *);
/* negates the input vector and returns it */
extern Vector3 *V3Negate(Vector3 *);
/* normalizes the input vector and returns it */
extern Vector3 *V3Normalize(Vector3 *);
/* scales the input vector to the new length and returns it */
extern Vector3 *V3Scale(Vector3 *, double );
/* return vector sum c = a+b */
extern Vector3 *V3Add(Vector3 *, Vector3 *, Vector3 *);
/* return vector difference c = a-b */
extern Vector3 *V3Sub(Vector3 *, Vector3 *, Vector3 *);
/* return the dot product of vectors a and b */
extern double V3Dot(Vector3 *, Vector3 *);
/* linearly interpolate between vectors by an amount alpha */
/* and return the resulting vector. */
/* When alpha=0, result=lo.  When alpha=1, result=hi. */
extern Vector3 *V3Lerp(Vector3 *, Vector3 *, double , Vector3 *);
/* make a linear combination of two vectors and return the result. */
/* result = (a * ascl); + (b * bscl); */
extern Vector3 *V3Combine (Vector3 *, Vector3 *, Vector3 *, double , double);
/* multiply two vectors together component-wise and return the result */
extern Vector3 *V3Mul (Vector3 *, Vector3 *, Vector3 *);
/* return the distance between two points */
extern double V3DistanceBetween2Points(Point3 *, Point3 * );
/* return the cross product c = a cross b */
extern Vector3 *V3Cross(Vector3 *a, Vector3 *, Vector3 *);
/* create, initialize, and return a new vector */
extern Vector3 *V3New(double, double, double);
/* create, initialize, and return a duplicate vector */
extern Vector3 *V3Duplicate(Vector3 *);
#endif
