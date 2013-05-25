/****** rat.c ******/
/* Ken Shoemake, 1994 */

#include <math.h>
#include "rat.h"

static void Mul32(UINT32 x, UINT32 y, UINT32 *hi, UINT32 *lo)
{
    UINT32 xlo = x&0xFFFF, xhi = (x>>16)&0xFFFF;
    UINT32 ylo = y&0xFFFF, yhi = (y>>16)&0xFFFF;
    UINT32 t1, t2, t3;
    UINT32 lolo, lohi, t1lo, t1hi, t2lo, t2hi, carry;
    *lo = xlo * ylo; *hi = xhi * yhi;
    t1 = xhi * ylo; t2 = xlo * yhi;
    lolo = *lo&0xFFFF; lohi = (*lo>>16)&0xFFFF;
    t1lo = t1&0xFFFF; t1hi = (t1>>16)&0xFFFF;
    t2lo = t2&0xFFFF; t2hi = (t2>>16)&0xFFFF;
    t3 = lohi + t1lo + t2lo;
    carry = (t3>>16)&0xFFFF; lohi = t3&0xFFFF;
    *hi += t1hi + t2hi + carry; *lo = (lohi<<16) + lolo;
}

/* ratapprox(x,n) returns the best rational approximation to x whose numerator
    and denominator are less than or equal to n in absolute value. The denominator
    will be positive, and the numerator and denominator will be in lowest terms.
    IEEE 32-bit floating point and 32-bit integers are required.
   All best rational approximations of a real x may be obtained from x's
    continued fraction representation, x = c0 + 1/(c1 + 1/(c2 + 1/(...)))
    by truncation to k terms and possibly "interpolation" of the last term.
    The continued fraction expansion itself is obtained by a variant of the
    standard GCD algorithm, which is folded into the recursions generating
    successive numerators and denominators. These recursions both have the
    same form: f[k] = c[k]*f[k-1] + f[k-2]. For further information, see
    Fundamentals of Mathematics, Volume I, MIT Press, 1983.
 */
Rational ratapprox(float x, INT32 limit)
{
    float tooLargeToFix = ldexp(1.0, BITS);         /* 0x4f000000=2147483648.0 */
    float tooSmallToFix = ldexp(1.0, -BITS);        /* 0x30000000=4.6566e-10 */
    float halfTooSmallToFix = ldexp(1.0, -BITS-1);  /* 0x2f800000=2.3283e-10 */
    int expForInt = 24;     /* This exponent in float makes mantissa an INT32 */
    static Rational ratZero = {0, 1};
    INT32 sign = 1;
    BOOL flip = FALSE;      /* If TRUE, nk and dk are swapped */
    int scale;              /* Power of 2 to get x into integer domain */
    UINT32 ak2, ak1, ak;    /* GCD arguments, initially 1 and x */
    UINT32 ck, climit;      /* ck is GCD quotient and c.f. term k */
    INT32 nk, dk;           /* Result num. and den., recursively found */
    INT32 nk1 = 0, dk2 = 0; /* History terms for recursion */
    INT32 nk2 = 1, dk1 = 1;
    BOOL hard = FALSE;
    Rational val;
    
    if (limit <= 0) return (ratZero);       /* Insist limit > 0 */
    if (x < 0.0) {x = -x; sign = -1;}
    val.numer = sign; val.denom = limit;
    /* Handle first non-zero term of continued fraction,
        rest prepared for integer GCD, sure to fit.
     */
    if (x >= 1.0) {/* First continued fraction term is non-zero */
            float rest;
            if (x >= tooLargeToFix || (ck = x) >= limit)
                {val.numer = sign*limit; val.denom = 1; return (val);}
            flip = TRUE;        /* Keep denominator larger, for fast loop test */
            nk = 1;  dk = ck;   /* Make new numerator and denominator */
            rest = x - ck;
            frexp(1.0,&scale);
            scale = expForInt - scale;
            ak = ldexp(rest, scale);
            ak1 = ldexp(1.0, scale);
        } else {/* First continued fraction term is zero */
            int n;
            UINT32 num = 1;
            if (x <= tooSmallToFix) {       /* Is x too tiny to be 1/INT32 ? */
                if (x <= halfTooSmallToFix) return (ratZero);
                if (limit > (UINT32)(0.5/x)) return (val);
                else return (ratZero);
            }
            /* Treating 1.0 and x as integers, divide 1/x in a peculiar way
                to get accurate remainder
             */
            frexp(x,&scale);
            scale = expForInt - scale;
            ak1 = ldexp(x, scale);
            n = (scale<BITS)?scale:BITS;    /* Stay within UINT32 arithmetic */
            num <<= n;
            ck = num/ak1;                   /* First attempt at 1/x */
            ak = num%ak1;                   /* First attempt at remainder */
            while ((scale -= n) > 0) {/* Shift quotient, remainder until done */
                n = (scale<8)?scale:8;      /* The 8 is 24 bits of x in 32 bits */
                num = ak<<n;
                ck = (ck<<n) + (num/ak1);
                ak = num%ak1;               /* Reduce remainder */
            }
            /* All done with divide */
            if (ck >= limit) {              /* Is x too tiny to be 1/limit ? */
                if (2*limit > ck)
                    return (val);
                else return (ratZero);
            }
            nk = 1;  dk = ck;               /* Make new numer and denom */
        }
    while (ak != 0) {                   /* If possible, quit when have exact result */
        ak2 = ak1;  ak1 = ak;           /* Prepare for next term */
        nk2 = nk1;  nk1 = nk;           /* (This loop does almost all the work) */
        dk2 = dk1;  dk1 = dk;
        ck = ak2/ak1;                   /* Get next term of continued fraction */
        ak = ak2 - ck*ak1;              /* Get remainder (GCD step) */
        climit = (limit - dk2)/dk1;     /* Anticipate result of recursion on denom */
        if (climit <= ck) {hard = TRUE; break;} /* Do not let denom exceed limit */
        nk = ck*nk1 + nk2;              /* Make new result numer and denom */
        dk = ck*dk1 + dk2;
    }
    if (hard) {
        UINT32 twoClimit = 2*climit;
        if (twoClimit >= ck) {      /* If climit < ck/2 no improvement possible */
            nk = climit*nk1 + nk2;  /* Make limited numerator and denominator */
            dk = climit*dk1 + dk2;
            if (twoClimit == ck) {  /* If climit == ck improvement not sure */
                /* Using climit is better only when dk2/dk1 > ak/ak1 */
                /* For full precision, test dk2*ak1 > dk1*ak */
                UINT32 dk2ak1Hi, dk2ak1Lo, dk1akHi, dk1akLo;
                Mul32(flip?nk2:dk2, ak1, &dk2ak1Hi, &dk2ak1Lo);
                Mul32(flip?nk1:dk1, ak, &dk1akHi, &dk1akLo);
                if ((dk2ak1Hi < dk1akHi)
                || ((dk2ak1Hi == dk1akHi) && (dk2ak1Lo <= dk1akLo)))
                    { nk = nk1;  dk = dk1; }    /* Not an improvement, so undo step */
            }
        }
    }
    if (flip) {val.numer = sign*dk;  val.denom = nk;}
    else      {val.numer = sign*nk;  val.denom = dk;}
    return (val);
}
