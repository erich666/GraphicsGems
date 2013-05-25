#include <stdio.h>
#include "fixpoint.h"

static int verbose = 0;

/* The dblfixpoint representation is signed magnitude, with
  hi is 2n bits of integer (n <= 16)
  lo is 2m bits of fraction (m <=16)
number = hi + lo * 2^(-2m)
*/

int fp_dblnegative(dblfixpoint x) { return (x.neg); }

dblfixpoint fp_dblnegate(dblfixpoint x)
{ x.neg = !x.neg;   return x; }

dblfixpoint fp_dblmultiply(fixpoint x, fixpoint y)
{
  dblfixpoint answer;
  unsigned int a, b, c, d;
  unsigned int xhi, xlo, yhi, ylo;

  xhi = (x<0) ? -fp_integer(x) : fp_integer(x);
  yhi = (y<0) ? -fp_integer(y) : fp_integer(y);
  xlo = fp_fraction(x);
  ylo = fp_fraction(y);

  a = xhi * yhi;
  b = xhi * ylo;
  c = xlo * yhi;
  d = xlo * ylo;

  a += ((b & HIMASK) >> LOBITS);
  a += ((c & HIMASK) >> LOBITS);
  b = (b & LOMASK) + (c & LOMASK) + ((d & HIMASK) >> LOBITS);
  a += ((b & HIMASK) >> LOBITS);
  answer.hi = a;
  answer.lo = ((b & LOMASK) << LOBITS) | (d & LOMASK);
  answer.neg = 0;

  if (((x<0) && (y>0)) || ((x>0) && (y<0))) answer = fp_dblnegate(answer);
  return(answer);
}


int fp_dbllessthan(dblfixpoint x, dblfixpoint y)
{
  int bothneg, xneg, yneg;
  xneg = fp_dblnegative(x);
  yneg = fp_dblnegative(y);
  bothneg = xneg && yneg;
  if (xneg && !yneg) return 1;
  if (yneg && !xneg) return 0;
  if (bothneg) {
    x = fp_dblnegate(x);
    y = fp_dblnegate(y);
  }
  if (x.hi < y.hi) return !bothneg;
  if (y.hi < x.hi) return  bothneg;
  if (x.lo < y.lo) return !bothneg;
  return bothneg;
}

fixpoint fp_trunc(dblfixpoint a)
{
  int hi = (a.hi << LOBITS);
  int lo = (a.lo >> LOBITS);

  if ((hi >> LOBITS) != a.hi) {
	 printf("fp_trunc() Overflow converting hibits 0x%08x to 0x%08x fixpoint in (%d,%d) bits\n", a.hi, hi, HIBITS, LOBITS);
  }

  if (a.neg)
	 return (-(hi + lo));
  else
	 return (hi + lo);
}


unsigned int add_with_carry(unsigned int a, unsigned int b,
									 int carry_in, int *carry_out)
{
  int ahi, bhi;
  unsigned int answer;

  *carry_out = 0;

  ahi = (0x80000000 & a) ? 1 : 0;   /* Is high bit on? */
  bhi = (0x80000000 & b) ? 1 : 0;

  a &= 0x7fffffff;
  b &= 0x7fffffff;

  answer = a + b + carry_in;	  /* this can't overflow (I think) */
  if (answer & 0x80000000) {
	 if (ahi && bhi) *carry_out = 1;	/* Hi bit is on, leave it on */
	 else if (ahi || bhi) {				/* Turn hi bit off, turn on carry */
		*carry_out = 1;
		answer &= 0x7fffffff;
	 }
  }
  else {                    /* else (if answer high bit is off) */
	 if (ahi && bhi) *carry_out = 1;
	 else if (ahi || bhi) answer |= 0x80000000;
  }
  return(answer);
}


dblfixpoint fp_dbladd(dblfixpoint a, dblfixpoint b)
{
  dblfixpoint answer;

  if (verbose) printf("\nfpdbladd\n");

  if ((a.neg && b.neg) || (!a.neg && !b.neg)) {
	 int carry;
	 answer.neg = a.neg;
	 answer.lo = add_with_carry(a.lo, b.lo, 0, &carry);

	 if (verbose && ((answer.lo >> 2*LOBITS) || carry))
	        printf("  Lobits overflow (ok, put into hibits).\n");

	 /*This puts lobit overflow into .hi, and removes from .lo */
	 answer.hi = a.hi + b.hi + (answer.lo >> 2*LOBITS) + carry;
	 answer.lo &= (LOMASK | (LOMASK << LOBITS));
  }
  else {
	 if (a.neg) {
		unsigned int tmp;
		if (verbose) printf("\tCase: a.neg, b.pos, a<b, swapping\n");
		a.neg = 0; b.neg =1;
		tmp = a.hi; a.hi = b.hi; b.hi = tmp;
		tmp = a.lo; a.lo = b.lo; b.lo = tmp;
	 }
	 if (a.hi < b.hi) {
		if (a.lo <= b.lo) {
		  if (verbose) printf("\tCase: a.pos, b.neg, a.hi<b.hi, a.lo<=b.lo \n");
		  answer.neg = 1;
		  answer.lo = b.lo - a.lo;
		  answer.hi = b.hi - a.hi;
		}
		else {
		  if (verbose) printf("\tCase: a.pos, b.neg, a.hi<b.hi, a.lo > b.lo \n");
		  answer.neg = 1;
		  answer.lo = (b.lo - a.lo) & (LOMASK | (LOMASK << LOBITS));
		  answer.hi = b.hi - a.hi - 1;
		}
	 }
	 else if ((a.hi == b.hi) && (a.lo < b.lo)) {
		if (verbose) printf("\tCase: a.pos, b.neg, a.hi = b.hi, a.lo < b.lo \n");
		  answer.neg = 1;
		  answer.lo = b.lo - a.lo;
		  answer.hi = 0;
	 }
	 else {
		if (a.lo >= b.lo) {
		  if (verbose) printf("\tCase: a.pos, b.neg, a.hi > b.hi, a.lo >= b.lo\n");
		  answer.neg = 0;
		  answer.lo = a.lo - b.lo;
		  answer.hi = a.hi - b.hi;
		}
		else {
		  if (verbose) printf("\tCase: a.pos, b.neg, a.hi > b.hi, a.lo < b.lo\n");
		  answer.neg = 0;
		  answer.lo = (a.lo - b.lo) & (LOMASK | (LOMASK << LOBITS));
		  answer.hi = a.hi - b.hi - 1;
		}
	 }
  }
  return(answer);
}

