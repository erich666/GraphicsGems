#ifndef FIXPOINT_H
#define FIXPOINT_H

/* Requires: LOBITS to be divisible by 2, HIBITS<=16, and LOBITS <=16. */

#define HIBITS 16
#define LOBITS 16

#define LOMASK       (~(0xffffffff << LOBITS))
#define HIMASK       ((~(0xffffffff << HIBITS)) << LOBITS)
#define SIGNBIT      (1 << (HIBITS+LOBITS-1))
#define OVERFLOWMASK (SIGNBIT | ~(HIMASK | LOMASK))

typedef int fixpoint;
typedef struct {unsigned int hi, lo, neg;} dblfixpoint;

extern int fp_error;
extern  fixpoint fp_max();
extern  fixpoint fp_min();
extern  int fp_integer();
extern  int fp_fraction();
extern  double fp_fraction_double();

extern  fixpoint fp_multiply();
extern  void fp_print();
extern  fixpoint fp_fix();
extern double fp_double(fixpoint x);

extern int fp_dblnegative();
extern dblfixpoint fp_dblnegate();
extern dblfixpoint fp_dblmultiply();
extern int fp_dbllessthan();
extern dblfixpoint fp_dbladd();
extern fixpoint fp_trunc();

#endif
