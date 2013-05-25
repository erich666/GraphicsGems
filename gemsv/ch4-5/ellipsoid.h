/* ellipsoid.h */

#ifndef ellipsoid_H
#define ellipsoid_H

typedef struct point { float x, y, z; } point;
typedef struct vertex {
    point p, n;         /* point and unit normal */
} vertex;
typedef struct face {
    int v0, v1, v2;     /* indices of vertex array for a triangular face */
} face;
typedef struct object {
    int nv, nf;         /* numbers of elements in v and f */
    vertex *v; face *f; /* arrays of vertices and faces */
} object;

void ellipsoid_init (int n);
void ellipsoid_seq (object *ellipsoid, int n, float a, float b, float c);
void ellipsoid_par (object *ellipsoid, int n, float a, float b, float c);
void ellipsoid_print (object *ellipsoid);
void ellipsoid_done (void);
void ellipsoid_free (object *ellipsoid);

#endif /* ellipsoid_H */
