/* Compute the Inverse Square Root
 * of an IEEE Single Precision Floating-Point number.
 *
 * Written by Ken Turkowski.
 */

/* Specified parameters */
#define LOOKUP_BITS    6   /* Number of mantissa bits for lookup */
#define EXP_POS       23   /* Position of the exponent */
#define EXP_BIAS     127   /* Bias of exponent */
/* The mantissa is assumed to be just down from the exponent */

/* Type of result */
#ifndef DOUBLE_PRECISION
 typedef float FLOAT;
#else /* DOUBLE_PRECISION */
 typedef double FLOAT;
#endif /* DOUBLE_PRECISION */

/* Derived parameters */
#define LOOKUP_POS   (EXP_POS-LOOKUP_BITS)  /* Position of mantissa lookup */
#define SEED_POS     (EXP_POS-8)            /* Position of mantissa seed */
#define TABLE_SIZE   (2 << LOOKUP_BITS)     /* Number of entries in table */
#define LOOKUP_MASK  (TABLE_SIZE - 1)           /* Mask for table input */
#define GET_EXP(a)   (((a) >> EXP_POS) & 0xFF)  /* Extract exponent */
#define SET_EXP(a)   ((a) << EXP_POS)           /* Set exponent */
#define GET_EMANT(a) (((a) >> LOOKUP_POS) & LOOKUP_MASK)  /* Extended mantissa
                                                           * MSB's */
#define SET_MANTSEED(a) (((unsigned long)(a)) << SEED_POS)  /* Set mantissa
                                                             * 8 MSB's */

#include <stdlib.h>
#include <math.h>

static unsigned char *iSqrt = NULL;

union _flint {
    unsigned long    i;
    float            f;
} _fi, _fo;

static void
MakeInverseSqrtLookupTable(void)
{
    register long f;
    register unsigned char *h;
    union _flint fi, fo;

    iSqrt = malloc(TABLE_SIZE);
    h = iSqrt;
    for (f = 0, h = iSqrt; f < TABLE_SIZE; f++) {
        fi.i = ((EXP_BIAS-1) << EXP_POS) | (f << LOOKUP_POS);
        fo.f = 1.0 / sqrt(fi.f);
        *h++ = ((fo.i + (1<<(SEED_POS-2))) >> SEED_POS) & 0xFF; /* rounding */
    }
    iSqrt[TABLE_SIZE / 2] = 0xFF;    /* Special case for 1.0 */
}

/* The following returns the inverse square root */
FLOAT
InvSqrt(float x)
{
    register unsigned long a = ((union _flint*)(&x))->i;
    register float arg = x;
    union _flint seed;
    register FLOAT r;

    if (iSqrt == NULL) MakeInverseSqrtLookupTable();

    seed.i = SET_EXP(((3*EXP_BIAS-1) - GET_EXP(a)) >> 1)
           | SET_MANTSEED(iSqrt[GET_EMANT(a)]);

    /* Seed: accurate to LOOKUP_BITS */
    r = seed.f;

    /* First iteration: accurate to 2*LOOKUP_BITS */
    r = (3.0 - r * r * arg) * r * 0.5;

    /* Second iteration: accurate to 4*LOOKUP_BITS */
    r = (3.0 - r * r * arg) * r * 0.5;

#ifdef DOUBLE_PRECISION
    /* Third iteration: accurate to 8*LOOKUP_BITS */
    r = (3.0 - r * r * arg) * r * 0.5;
#endif /* DOUBLE_PRECISION */
    return(r);
}
