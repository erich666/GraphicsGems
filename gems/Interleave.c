/*
Bit Interleaving for Quad- or Octrees
by Clifford A. Shaffer
from "Graphics Gems", Academic Press, 1990
*/

#include "GraphicsGems.h"
#define B_MAX_DEPTH 14	/* maximum depth allowed */

/* byteval is the lookup table for coordinate interleaving.  Given a
   4 bit portion of the (x, y) coordinates, return the bit interleaving.
   Notice that this table looks like the order in which the pixels of
   a 16 X 16 pixel image would be visited. */
int byteval[16][16] = 
    {  0,  1,  4,  5, 16, 17, 20, 21, 64, 65, 68, 69, 80, 81, 84, 85,
       2,  3,  6,  7, 18, 19, 22, 23, 66, 67, 70, 71, 82, 83, 86, 87,
       8,  9, 12, 13, 24, 25, 28, 29, 72, 73, 76, 77, 88, 89, 92, 93,
      10, 11, 14, 15, 26, 27, 30, 31, 74, 75, 78, 79, 90, 91, 94, 95,
      32, 33, 36, 37, 48, 49, 52, 53, 96, 97,100,101,112,113,116,117,
      34, 35, 38, 39, 50, 51, 54, 55, 98, 99,102,103,114,115,118,119,
      40, 41, 44, 45, 56, 57, 60, 61,104,105,108,109,120,121,124,125,
      42, 43, 46, 47, 58, 59, 62, 63,106,107,110,111,122,123,126,127,
     128,129,132,133,144,145,148,149,192,193,196,197,208,209,212,213,
     130,131,134,135,146,147,150,151,194,195,198,199,210,211,214,215,
     136,137,140,141,152,153,156,157,200,201,204,205,216,217,220,221,
     138,139,142,143,154,155,158,159,202,203,206,207,218,219,222,223,
     160,161,164,165,176,177,180,181,224,225,228,229,240,241,244,245,
     162,163,166,167,178,179,182,183,226,227,230,231,242,243,246,247,
     168,169,172,173,184,185,188,189,232,233,236,237,248,249,252,253,
     170,171,174,175,186,187,190,191,234,235,238,239,250,251,254,255};

/* bytemask is the mask for byte interleaving - masks out the 
   non-significant bit positions.  This is determined by the
   depth of the node. For example, a node of depth 0 is at the root.
  Thus, there are no branchs and no bits are significant. 
  The bottom 4 bits (the depth) are always retained. 
  Values are in octal notation. */
int bytemask[B_MAX_DEPTH + 1] = {017,
     030000000017,036000000017,037400000017,037700000017,
     037760000017,037774000017,037777000017,037777600017,
     037777740017,037777770017,037777776017,037777777417,
     037777777717,037777777777};


long *interleave(addr, x, y, depth, max_depth)
/* Return the interleaved code for a quadtree node at depth depth 
whose upper left hand corner has coordinates (x, y) in a tree with maximum
depth max_depth.  This function receives and returns a 
pointer to addr, which is either a long integer or (more typically)
an array of long integers whose first integer contains the 
interleaved code. */
 long *addr;
 int max_depth, depth;
 int x, y;
{

/* Scale x, y values to be consistent with maximum coord size */
/* and depth of tree */
 x <<= (B_MAX_DEPTH - max_depth);
 y <<= (B_MAX_DEPTH - max_depth);

/* calculate the bit interleaving of the x, y values that have now
   been appropriately shifted, and place this interleave in the address
   portion of addr.  Note that the binary representations of x and y are
   being processed from right to left */

 *addr = depth;
 *addr |= byteval[y & 03][x & 03] << 4;
 *addr |= byteval[(y >> 2) & 017][(x >> 2) & 017] << 8;
 *addr |= byteval[(y >> 6) & 017][(x >> 6) & 017] << 16;
 *addr |= byteval[(y >> 10) & 017][(x >> 10) & 017] << 24;
 *addr &= bytemask[depth];

/* if there were unused portions of the x and y addresses then  */
/* the address was too large for the depth values given.  */
/*  Return address built */
 return (addr);
}



/* The next two arrays are used in calculating the (x, y) coordinates
   of the upper left-hand corner of a node from its bit interleaved
   address.  Given an 8 bit number, the arrays return the effect of
   removing every other bit (the y bits precede the x bits). */

int xval[256] = { 0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3,
		         4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 7,
		         0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3,
		         4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 7,
		         8, 9, 8, 9,10,11,10,11, 8, 9, 8, 9,10,11,10,11,
		        12,13,12,13,14,15,14,15,12,13,12,13,14,15,14,15,
		         8, 9, 8, 9,10,11,10,11, 8, 9, 8, 9,10,11,10,11,
		        12,13,12,13,14,15,14,15,12,13,12,13,14,15,14,15,
		         0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3,
		         4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 7,
		         0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3,
		         4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 7,
		         8, 9, 8, 9,10,11,10,11, 8, 9, 8, 9,10,11,10,11,
		        12,13,12,13,14,15,14,15,12,13,12,13,14,15,14,15,
		         8, 9, 8, 9,10,11,10,11, 8, 9, 8, 9,10,11,10,11,
		        12,13,12,13,14,15,14,15,12,13,12,13,14,15,14,15};


int yval[256] = { 0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3,
		      0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3,
		      4, 4, 5, 5, 4, 4, 5, 5, 6, 6, 7, 7, 6, 6, 7, 7,
		      4, 4, 5, 5, 4, 4, 5, 5, 6, 6, 7, 7, 6, 6, 7, 7,
		      0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3,
		      0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3,
		      4, 4, 5, 5, 4, 4, 5, 5, 6, 6, 7, 7, 6, 6, 7, 7,
		      4, 4, 5, 5, 4, 4, 5, 5, 6, 6, 7, 7, 6, 6, 7, 7,
		      8, 8, 9, 9, 8, 8, 9, 9,10,10,11,11,10,10,11,11,
		      8, 8, 9, 9, 8, 8, 9, 9,10,10,11,11,10,10,11,11,
		     12,12,13,13,12,12,13,13,14,14,15,15,14,14,15,15,
		     12,12,13,13,12,12,13,13,14,14,15,15,14,14,15,15,
		      8, 8, 9, 9, 8, 8, 9, 9,10,10,11,11,10,10,11,11,
		      8, 8, 9, 9, 8, 8, 9, 9,10,10,11,11,10,10,11,11,
		     12,12,13,13,12,12,13,13,14,14,15,15,14,14,15,15,
		     12,12,13,13,12,12,13,13,14,14,15,15,14,14,15,15};





int getx(addr, max_depth)
/* Return the x coordinate of the upper left hand corner of addr for a
   tree with maximum depth max_depth. */
 long *addr;
 int max_depth;
{
 register x;

 x = xval[(*addr >> 4) & 017];
 x |= xval[(*addr >> 8) & 0377] << 2;
 x |= xval[(*addr >> 16) & 0377] << 6;
 x |= xval[(*addr >> 24) & 0377] << 10;
 x >>= B_MAX_DEPTH - max_depth;
 return (x);
}



int QKy(addr, max_depth)
/* Return the y coordinate of the upper left hand corner of addr for a
   tree with maximum depth max_depth. */

 long *addr;
 int max_depth;
{
 register y;

 y = yval[(*addr >> 4) & 017];
 y |= yval[(*addr >> 8) & 0377] << 2;
 y |= yval[(*addr >> 16) & 0377] << 6;
 y |= yval[(*addr >> 24) & 0377] << 10;
 y >>= B_MAX_DEPTH - max_depth;
 return (y);
}

int getdepth(addr)
/* Return the depth of the node.  Simply return the bottom 4 bits. */

 long *addr;
{

 return(*addr & 017);
}
