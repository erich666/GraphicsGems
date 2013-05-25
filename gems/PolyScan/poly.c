/*
 * poly.c: simple utilities for polygon data structures
 */

#include "poly.h"

Poly_vert *poly_dummy;		/* used superficially by POLY_MASK macro */

/*
 * poly_print: print Poly p to stdout, prefixed by the label str
 */

void poly_print(str, p)
char *str;
Poly *p;
{
    int i;

    printf("%s: %d sides\n", str, p->n);
    poly_vert_label("        ", p->mask);
    for (i=0; i<p->n; i++) {
	printf("   v[%d] ", i);
	poly_vert_print("", &p->vert[i], p->mask);
    }
}

void poly_vert_label(str, mask)
char *str;
unsigned long mask;
{
    printf("%s", str);
    if (mask&POLY_MASK(sx))   printf("   sx  ");
    if (mask&POLY_MASK(sy))   printf("   sy  ");
    if (mask&POLY_MASK(sz))   printf("   sz  ");
    if (mask&POLY_MASK(sw))   printf("   sw  ");
    if (mask&POLY_MASK(x))    printf("   x   ");
    if (mask&POLY_MASK(y))    printf("   y   ");
    if (mask&POLY_MASK(z))    printf("   z   ");
    if (mask&POLY_MASK(u))    printf("   u   ");
    if (mask&POLY_MASK(v))    printf("   v   ");
    if (mask&POLY_MASK(q))    printf("   q   ");
    if (mask&POLY_MASK(r))    printf("   r   ");
    if (mask&POLY_MASK(g))    printf("   g   ");
    if (mask&POLY_MASK(b))    printf("   b   ");
    if (mask&POLY_MASK(nx))   printf("   nx  ");
    if (mask&POLY_MASK(ny))   printf("   ny  ");
    if (mask&POLY_MASK(nz))   printf("   nz  ");
    printf("\n");
}

void poly_vert_print(str, v, mask)
char *str;
Poly_vert *v;
unsigned long mask;
{
    printf("%s", str);
    if (mask&POLY_MASK(sx)) printf(" %6.1f", v->sx);
    if (mask&POLY_MASK(sy)) printf(" %6.1f", v->sy);
    if (mask&POLY_MASK(sz)) printf(" %6.2f", v->sz);
    if (mask&POLY_MASK(sw)) printf(" %6.2f", v->sw);
    if (mask&POLY_MASK(x))  printf(" %6.2f", v->x);
    if (mask&POLY_MASK(y))  printf(" %6.2f", v->y);
    if (mask&POLY_MASK(z))  printf(" %6.2f", v->z);
    if (mask&POLY_MASK(u))  printf(" %6.2f", v->u);
    if (mask&POLY_MASK(v))  printf(" %6.2f", v->v);
    if (mask&POLY_MASK(q))  printf(" %6.2f", v->q);
    if (mask&POLY_MASK(r))  printf(" %6.4f", v->r);
    if (mask&POLY_MASK(g))  printf(" %6.4f", v->g);
    if (mask&POLY_MASK(b))  printf(" %6.4f", v->b);
    if (mask&POLY_MASK(nx)) printf(" %6.3f", v->nx);
    if (mask&POLY_MASK(ny)) printf(" %6.3f", v->ny);
    if (mask&POLY_MASK(nz)) printf(" %6.3f", v->nz);
    printf("\n");
}
