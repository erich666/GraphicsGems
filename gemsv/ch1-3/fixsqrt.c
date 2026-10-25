/* The definitions below yield 2 integer bits, 30 fractional bits */
#define FRACBITS 30    /* Must be even! */
#define ITERS    (15 + (FRACBITS >> 1))
typedef long TFract;

#include <stdint.h>

TFract
FFracSqrt(TFract x)
{
    /* must be exactly 32 bits: remLo >> 30 relies on bits shifting out the top */
    register uint32_t root, remHi, remLo, testDiv, count;

    root = 0;         /* Clear root */
    remHi = 0;        /* Clear high part of partial remainder */
    remLo = x;        /* Get argument into low part of partial remainder */
    count = ITERS;    /* Load loop counter */

    do {
        remHi = (remHi << 2) | (remLo >> 30); remLo <<= 2;  /* get 2 bits of arg */
        root <<= 1;   /* Get ready for the next bit in the root */
        testDiv = (root << 1) + 1;    /* Test radical */
        if (remHi >= testDiv) {
            remHi -= testDiv;
            root += 1;
        }
    } while (count-- != 0);

    return(root);
}
