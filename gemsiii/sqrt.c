/* fsqrt.c
 *
 * A fast square root program adapted from the code of
 * Paul Lalonde and Robert Dawson in Graphics Gems I.
 * The format of IEEE double precision floating point numbers is:
 *
 * SEEEEEEEEEEEMMMM MMMMMMMMMMMMMMMM MMMMMMMMMMMMMMMM MMMMMMMMMMMMMMMM
 *
 * S = Sign bit for whole number
 * E = Exponent bit (exponent in excess 1023 form)
 * M = Mantissa bit
 */

#include <stdio.h>
#include <math.h>

/* MOST_SIG_OFFSET gives the (int *) offset from the address of the double
 * to the part of the number containing the sign and exponent.
 * You will need to find the relevant offset for your architecture.
 */

#define MOST_SIG_OFFSET 1

/* SQRT_TAB_SIZE - the size of the lookup table - must be a power of four.
 */

#define SQRT_TAB_SIZE 16384

/* MANT_SHIFTS is the number of shifts to move mantissa into position.
 * If you quadruple the table size subtract two from this constant,
 * if you quarter the table size then add two.
 * Valid values are: (16384, 7) (4096, 9) (1024, 11) (256, 13)
 */

#define MANT_SHIFTS   7

#define EXP_BIAS   1023       /* Exponents are always positive     */
#define EXP_SHIFTS 20         /* Shifs exponent to least sig. bits */
#define EXP_LSB    0x00100000 /* 1 << EXP_SHIFTS                   */
#define MANT_MASK  0x000FFFFF /* Mask to extract mantissa          */

int        sqrt_tab[SQRT_TAB_SIZE];

void
init_sqrt_tab()
{
        int           i;
        double        f;
        unsigned int  *fi = (unsigned int *) &f + MOST_SIG_OFFSET;
        
        for (i = 0; i < SQRT_TAB_SIZE/2; i++)
        {
                f = 0; /* Clears least sig part */
                *fi = (i << MANT_SHIFTS) | (EXP_BIAS << EXP_SHIFTS);
                f = sqrt(f);
                sqrt_tab[i] = *fi & MANT_MASK;

                f = 0; /* Clears least sig part */
                *fi = (i << MANT_SHIFTS) | ((EXP_BIAS + 1) << EXP_SHIFTS);
                f = sqrt(f);
                sqrt_tab[i + SQRT_TAB_SIZE/2] = *fi & MANT_MASK;
        }
}

double
fsqrt(f)
double f;
{
        unsigned int e;
        unsigned int   *fi = (unsigned int *) &f + MOST_SIG_OFFSET;

        if (f == 0.0) return(0.0);
        e = (*fi >> EXP_SHIFTS) - EXP_BIAS;
        *fi &= MANT_MASK;
        if (e & 1)
                *fi |= EXP_LSB;
        e >>= 1;
        *fi = (sqrt_tab[*fi >> MANT_SHIFTS]) |
              ((e + EXP_BIAS) << EXP_SHIFTS);
        return(f);
}

void
dump_sqrt_tab()
{
        int        i, nl = 0;

        printf("unsigned int sqrt_tab[] = {\n");
        for (i = 0; i < SQRT_TAB_SIZE-1; i++)
        {
                printf("0x%x,", sqrt_tab[i]);
                nl++;
                if (nl > 8) { nl = 0; putchar('\n'); }
        }
        printf("0x%x\n", sqrt_tab[SQRT_TAB_SIZE-1]);
        printf("};\n");
}
