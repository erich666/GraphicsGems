/****** rat.h ******/
/* Ken Shoemake, 1994 */

#ifndef _H_rat
#define _H_rat

#include <limits.h>
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define BITS (32-1)
#if (INT_MAX>=2147483647)
    typedef int INT32;
    typedef unsigned int UINT32;
#else
    typedef long INT32;
    typedef unsigned long UINT32;
#endif
typedef struct {INT32 numer,denom;} Rational;

Rational ratapprox(float x, INT32 limit);
#endif
