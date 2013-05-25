#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fixpoint.h"

/* A non-optimized fixpoint pkg, with some overflow checking. */

#define TwosComplement(_x_)  (-(_x_))

int fp_verbose;
int fp_error = 0;        /* will contain error indicators for fp_multiply */
int fp_print_error = 1;  /* if 1, print overflow errors */


/* Return the largest (smallest) number representable in this format */
fixpoint fp_max() {  return(~OVERFLOWMASK); }
fixpoint fp_min() {  return(OVERFLOWMASK); }

/* return integer part */
int fp_integer(fixpoint x)
{
  fixpoint floor = x >> LOBITS;
  fixpoint bits = (x & LOMASK);

  if (x < 0) {
	 if (!bits) return(floor);
	 return(floor+1);
  }
  else return (floor);
}

/* return fraction part, in bits */
int fp_fraction(fixpoint x)
{
  if (x < 0) return(TwosComplement(x) & LOMASK);
  else return(x & LOMASK);
}

/* return fraction part, as a double */
double fp_fraction_double(fixpoint x)
{
  if (x < 0) return(-(TwosComplement(x) & LOMASK) / ((double) (1 << LOBITS)));
  else return((x & LOMASK) / ((double) (1 << LOBITS)));
}

/* in 2s complement, integers have all lower order bits = 0,
	and truncating the fraction = floor.
 */
fixpoint fp_floor(fixpoint x)
{
  fixpoint answer = x & HIMASK;
  return (answer);
}


/* The std integer truncating divide does a floor operation,
  but for negative numbers it does floor(abs(x/y)), so
  we need to case on sign of x to fix that.

 if x%y has no remainder, then the divide is on an integer

Overflow can't happen in fp_floor_div, since x/y <= x for y>=1, which
must be the case since y is an integer and y>0.

*/

fixpoint fp_floor_div(fixpoint x, fixpoint y)
{
  fixpoint answer;
  
  if (y == 0) {
	 printf("Error: fp_floor_div -- can't divide by 0!\n");
	 return(fp_fix(0));
  }
  else if (y < 0) {
	 printf("Sorry, fp_floor_div(a,b) doesn't currently work for b<0.\n");
	 return(fp_fix(0));
  }
  else if (x >= 0) {
	 fixpoint tmp = x/y;
	 answer = tmp << LOBITS;
  }
  else {
	 fixpoint tmp = ((x/y) + (((x % y) == 0) ? 0 : -1));
	 answer = tmp << LOBITS;
  }
  return (answer);
}


fixpoint fp_multiply(fixpoint x, fixpoint y)
{
  fixpoint answer;
  unsigned int xhi, xlo, yhi, ylo, tmp;

  fp_error = 0;

  xhi = (x<0) ? -fp_integer(x) : fp_integer(x);
  yhi = (y<0) ? -fp_integer(y) : fp_integer(y);
  xlo = fp_fraction(x);
  ylo = fp_fraction(y);

  /* If xhi and yhi are both < 16 bits, this can't have machine overflow
	  (overflow of the 32bit int). But it CAN get larger than HIBITS, or
	  crush the sign bit, causing overflow of our fixpoint representation.
	*/
  tmp = (xhi * yhi);
  if (tmp & (((int) OVERFLOWMASK) >> LOBITS)) {
	 if (fp_print_error)
		printf ("ERROR: fp_multiply() xhi*yhi = 0x%08x overflows by 0x%08x\n",
				  tmp,
				  (tmp & (((int) OVERFLOWMASK) >> LOBITS)));
	 fp_error = -1;
	 abort();
	 return(0);
  }

  answer = (tmp << LOBITS) +
           (xhi * ylo) + (xlo * yhi) +
			  (((xlo * ylo) >> LOBITS) & LOMASK);

  if (answer & ~(HIMASK | LOMASK)) { 
	 if (fp_print_error)
		printf ("ERROR: fp_multiply() answer = 0x%08x, overflow 0x%08x\n",
				  answer, (answer & ~(HIMASK | LOMASK)));
	 fp_error = -2;
	 abort();
	 return(0);
  }

  /* Also, it can fill up the high order (sign) bit, which is overflow. */
  if (answer & SIGNBIT) {
	 if (fp_print_error)
		printf ("ERROR: fp_multiply() SIGNBIT overflow for answer = 0x%08x\n",
				  answer);
	 fp_error = -3;
	 return(0);
  }

  /* "this should never happen" */
  if ((xlo >> 16) && (ylo >> 16)) {
	 if (fp_print_error)
		printf("++++++++++++  ERROR -- OVERFLOW OF LOW BITS**********\n");
	 fp_error = -4;
	 abort();
	 return(0);
  }

  if (((x<0) && (y>0)) || ((x>0) && (y<0))) answer = -answer;

  return(answer);
}


/* Turn a double into a fixpoint number int two's complement. 
	Negative numbers become:  ~a + 1
 */
fixpoint fp_fix(double x)
{
  int negative = (x < 0);
  double i;				/* integer part */
  double fraction;	/* fraction part, positive only */
  fixpoint p;			/* fixpoint version of abs(x) */


  if ((x < fp_integer(fp_min())) || (x >= fp_integer(fp_max())))
	 printf("sorry, %g doesn't fit in (%d hi, %d lo) bit fix point.\n",
			  x, HIBITS, LOBITS);

  x = fabs(x);
  i = floor(x);
  fraction = x - i;

  p = (((int) i) << LOBITS) | ((int) ((x-i)*(1<<LOBITS)));

  if (negative) return TwosComplement(p);
  else return (p);
}

/* Print as a binary number */
void fp_printb(fixpoint x)
{
  int i;
  unsigned int mask;

  if (x<0) x = -x;
  mask = 1 << (HIBITS + LOBITS - 1); /* start printing hi bit */
  for(i=0; i<HIBITS; i++) {
	 if (mask & x) putchar('1');
	 else          putchar('0');
	 mask = mask >> 1;
  }
  putchar('.');
  for(i=0; i<LOBITS; i++) {
	 if (mask & x) putchar('1');
	 else          putchar('0');
	 mask = mask >> 1;
  }
}

/* Print as a hex number, but gets things a bit screwed
	up unless HIBITS=LOBITS=16
*/
void fp_printx(fixpoint x)
{
  printf("%4x.%04x", fp_integer(x) & LOMASK, fp_fraction(x));
}

double fp_double(fixpoint x)
{
  return(((double) fp_integer(x)) + fp_fraction_double(x));
}

void fp_print(fixpoint x)
{
  printf("%.11g", fp_double(x));
}


void printnbits()
{
  printf("HIBITS = %d, LOBITS = %d, OVERFLOWMASK = 0x%8x, SIGNBIT = 0x%08x\n",
			HIBITS, LOBITS, OVERFLOWMASK, SIGNBIT);
  printf("HIMASK = 0x%08x, LOMASK = 0x%08x\n", HIMASK, LOMASK);
  printf("overflowmask >> lobits = 0x%08x\n", ((int) OVERFLOWMASK) >> LOBITS);
}
