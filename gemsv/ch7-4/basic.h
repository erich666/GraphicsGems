// -*- C++ -*-
// basic.h by George Vanecek, Jr. June 1994

#ifndef _BASIC_H_
#define _BASIC_H_

#ifndef _IOSTREAM_H
#include <ostream.h>
#endif

#ifndef __MATH_H__
#include <math.h>
#endif

#ifndef _ASSERT_H_
#include <assert.h>
#endif

typedef int     Counter;	// 0,1,2,...
typedef int     Index;		// Array Index: 0,1,2,...
enum Boolean { FALSE, TRUE };
enum Where   {			// Point/Plane Classification
  NOWHERE,
  ABOVE      = 0x01,
  ON         = 0x02,
  ONABOVE    = 0x03,		// ON    | ABOVE
  BELOW      = 0x04,
  ABOVEBELOW = 0x05,		// ABOVE | BELOW
  ONBELOW    = 0x06,		// ON    | BELOW
  CROSS      = 0x07		// ABOVE | ON    | BELOW
};

template<class Type>
inline void swap( Type& a, Type& b )
{
  const Type c = a;
  a = b;
  b = c;
}

#ifndef NULL
  #define NULL 0
#endif

#endif
