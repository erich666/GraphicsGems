/* The following code implements the four basic interval arithmetic 
operations +, -, * and / within the C++ class interval as provided 
by the AT & T C++ Release 2.0 for the Sparc architecture. There is 
no checking that the intervals are valid intervals, i.e. if the 
interval is [lo.hi] then it is valid iff lo <= hi.

It is assumed that a mechanism exists to direct the result of a 
floating point operation so that it is chopped. This corresponds
to "tozero" in ANSI/IEEE Std. 754-1985 and it is performed by the 
routine
               ieee_flags
in this code.

The first executable statement of any program using this code must 
therefore be 
               setmode(1);
or a corresponding call in in the architecture used in order that 
the rounding mode will be the chopped mode which is assumed in the 
class. Changes have to be made to the routine help_ if another
rounding mode is used.
*/


#define	min(a, b)		(((a) < (b)) ? (a) : (b))
#define	MIN(a, b, c, d)		min(min(min(a, b), c), d)
#define	max(a, b)		(((a) > (b)) ? (a) : (b))
#define	MAX(a, b, c, d)		max(max(max(a, b), c), d)

#include <stdio.h>
#include <stream.h>
#include <sys/ieeefp.h>

extern "C" {
	int ieee_flags(char *, char *, char *in, char **);
      }
void setmode_(int *mode)
{
char *out;

	ieee_flags("set", "direction", *mode ? "tozero" : "nearest", &out);
	ieee_flags("get", "direction", NULL, &out);
}



/*

This routine adds 1 to the last digit of a double data item which is here
assumed to have 8 bytes where

seeeeeee eeeemmmm mmmmmmmm mmmmmmmm mmmmmmmm mmmmmmmm mmmmmmmmm mmmmmmmm

s=sign of number
e=exponent			01111111111 is an exponent of 0
m=mantissa bits with assumed leading 1

The algorithm works by adding one to the last two bytes of the mantissa.
If the result of this addition is zero then it overflows to the next two
bytes. This means that the next two bytes must be checked for overflow 
until the mantissa bits are exhausted. It should then be noted that the
exponent will be automatically incremented and the mantissa will have 
been set to all zeros.

No check is made to see if the exponent overflows. Negative numbers will 
turn into the next larger negative number.

This routine assumes that the number of bytes in a double is and integer
multiple of the number of bytes in an unsigned integer.
*/


void helpx_(double *a)
{
unsigned char *b;
unsigned int *c,i;

	b=(unsigned char *)a;
	i=sizeof(double)-sizeof(unsigned int);
	do {
		c=(unsigned int *)(b+i);
		(*c)++;
		i-=sizeof(unsigned int);
	} while ((*c)==0 && i>=0);
}

/* A printing utility for printing the octal contents of a double 
data item */

void print_bin(double a)
{
char *b;
int i,j,k;

	k=0;
	b=(char *)(&a);
	printf("%f ",a);
	for (i=0;i<sizeof(double);i++) {
		for(j=0;j<8;j++) {
			k++;
			if (k==2 || k==13) putchar(' ');
			printf("%d",(b[i]&0x80)==128);
			b[i]<<=1;
		}
	}
	printf("\n");
}

/*
The basic arithmetic operations are embedded in the class interval. In
each case the pair of double data items representing the interval [lo,hi]
is checked via the routine help_. This routine accesses helpx_ whenever
          lo<0
or
          hi>0.
In both cases the next double representable item is calculated in helpx_
and the result is returned to the calling operator. In this manner it is
guaranteed that the machine interval result will contain the interval
result that would have been computed using infinite (real) interval
arithmetic.
*/

class	interval {
        double lo,hi;


public:
	interval(double lo = 0, double hi = 0) {
		this->lo = lo;
		this->hi = hi;
	};

/* An interval printing utility */
        void print() {
		printf("[ %3lx, %3lx ]\n", lo, hi);
	};

	friend interval help_(interval a) {
	  if(a.lo < 0 ) {
	    helpx_(&a.lo);
	  }
	  if(a.hi > 0 ){
	    helpx_(&a.hi);
	  }
	  return(a);
	}	
 

	friend interval operator+(interval a, interval b){
		return(help_(interval(a.lo+b.lo,a.hi+b.hi)));
	};
	friend interval operator-(interval a, interval b){
		return(help_(interval(a.lo-b.hi,a.hi-b.lo)));
	};
	friend interval operator*(interval a, interval b){
                double ac,ad,bc,bd;
                ac=a.lo * b.lo;
		ad=a.lo * b.hi;
		bc=a.hi * b.lo;
		bd=a.hi * b.hi;
		return(help_(interval(MIN(ac,ad,bc,bd),
                                      MAX(ac,ad,bc,bd))));
	};
	friend interval operator/(interval a, interval b){
		if( b.lo == 0.0 || b.hi == 0.0 ){
			cout << form("\n bad interval for division");
		}
		return( a * help_(interval(1.0 / b.hi, 1.0 / b.lo)) );
	};
	friend double lower_bound(interval a) {
	  return(a.lo);
	};
	friend double upper_bound(interval a){
	  return(a.hi);
	};
};



main()
{
int mode = 1;
interval x(1.0, 2.0);
interval y(4.0, 5.0);
interval a(-1.0,1.0);
interval b,z;

        setmode_(&mode);
        a.print();
        print_bin(lower_bound(a));
        print_bin(upper_bound(a));
        b=help_(a);

        print_bin(lower_bound(b));
        print_bin(upper_bound(b));
        print_bin(-lower_bound(b));
        b.print();
	z = x + y;
	z.print();
	z = x - y;
	z.print();
	z = x * y;
	z.print();
	z = x / y;
	z.print();
}

