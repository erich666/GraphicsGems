#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  double x, y;
} point_t, vector_t;

typedef struct {
  point_t v0, v1;
  int is_inserted;
  int root0, root1;
} segment_t;

typedef struct {
  int lseg, rseg;
  point_t hi, lo;
  int u0, u1;
  int d0, d1;
  int sink;
  int usave, uside;
  int state;
} trap_t;

typedef struct {
  int nodetype;
  int segnum;
  point_t yval;
  int trnum;
  int parent;
  int left, right;
} node_t;

typedef struct {
  int vnum;
  int next;
  int prev;
} monchain_t;

typedef struct {
  point_t pt;
  int vnext[4];			/* next vertices for the 4 chains */
  int vpos[4];			/* position of v in the 4 chains */
  int nextfree;
} vertexchain_t;

struct global_s {
  int nseg;
};

#define T_X     1
#define T_Y     2
#define T_SINK  3

#define QSIZE   800		/* maximum table sizes */
#define TRSIZE  400		/* max# trapezoids */
#define SEGSIZE 100		/* max# of segments */

#define TRUE  1
#define FALSE 0

#define FIRSTPT 1		/* checking whether pt. is inserted */ 
#define LASTPT  2

#define EPS 0.000005

#define INFINITY 1<<30

#define C_EPS 1.0e-7

#define S_LEFT 1		/* for merge-direction */
#define S_RIGHT 2

#define ST_VALID 1		/* for trapezium table */
#define ST_INVALID 2

#define SP_SIMPLE_LRUP 1	/* for splitting trapezoids */
#define SP_SIMPLE_LRDN 2
#define SP_2UP_2DN     3
#define SP_2UP_LEFT    4
#define SP_2UP_RIGHT   5
#define SP_2DN_LEFT    6
#define SP_2DN_RIGHT   7
#define SP_NOSPLIT    -1	

#define TR_FROM_UP 1		/* for traverse-direction */
#define TR_FROM_DN 2

#define TRI_LHS 1
#define TRI_RHS 2


#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define CROSS(v0, v1, v2) (((v1).x - (v0).x)*((v2).y - (v0).y) - \
			   ((v1).y - (v0).y)*((v2).x - (v0).x))

#define DOT(v0, v1) ((v0).x * (v1).x + (v0).y * (v1).y)

#define MODULO_NEXT(v0, n) (((v0) - 1) % (n) + 1)

#define FP_EQUAL(s, t) (fabs(s - t) <= C_EPS)


/* Global variables */

node_t qs[QSIZE];		/* Query structure */
trap_t tr[TRSIZE];		/* Trapezoid structure */
segment_t seg[SEGSIZE];		/* Segment table */

struct global_s global;
