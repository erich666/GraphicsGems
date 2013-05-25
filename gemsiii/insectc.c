/* Faster Line Segment Intersection   */
/* Franklin Antonio                   */

/* return values */
#define DONT_INTERSECT 0
#define DO_INTERSECT   1
#define PARALLEL       2


/* The SAME_SIGNS macro assumes arithmetic where the exclusive-or    */
/* operation will work on sign bits.  This works for twos-complement,*/
/* and most other machine arithmetic.                                */
#define SAME_SIGNS( a, b ) \
	(((long) ((unsigned long) a ^ (unsigned long) b)) >= 0 )


/* The use of some short working variables allows this code to run   */
/* faster on 16-bit computers, but is not essential.  It should not  */
/* affect operation on 32-bit computers.  The short working variables*/
/* to not restrict the range of valid input values, as these were    */
/* constrained in any case, due to algorithm restrictions.           */


int lines_intersect(x1,y1,x2,y2,x3,y3,x4,y4,x,y) 
long x1,y1,x2,y2,x3,y3,x4,y4,*x,*y;
{

long Ax,Bx,Cx,Ay,By,Cy,d,e,f,num,offset;
short x1lo,x1hi,y1lo,y1hi;

Ax = x2-x1;
Bx = x3-x4;

if(Ax<0) {						/* X bound box test*/
  x1lo=(short)x2; x1hi=(short)x1;
  } else {
  x1hi=(short)x2; x1lo=(short)x1;
  }
if(Bx>0) {
  if(x1hi < (short)x4 || (short)x3 < x1lo) return DONT_INTERSECT;
  } else {
  if(x1hi < (short)x3 || (short)x4 < x1lo) return DONT_INTERSECT;
  }

Ay = y2-y1;
By = y3-y4;

if(Ay<0) {						/* Y bound box test*/
  y1lo=(short)y2; y1hi=(short)y1;
  } else {
  y1hi=(short)y2; y1lo=(short)y1;
  }
if(By>0) {
  if(y1hi < (short)y4 || (short)y3 < y1lo) return DONT_INTERSECT;
  } else {
  if(y1hi < (short)y3 || (short)y4 < y1lo) return DONT_INTERSECT;
  }


Cx = x1-x3;
Cy = y1-y3;
d = By*Cx - Bx*Cy;					/* alpha numerator*/
f = Ay*Bx - Ax*By;					/* both denominator*/
if(f>0) {						/* alpha tests*/
  if(d<0 || d>f) return DONT_INTERSECT;
  } else {
  if(d>0 || d<f) return DONT_INTERSECT;
  }

e = Ax*Cy - Ay*Cx;					/* beta numerator*/
if(f>0) {						/* beta tests*/
  if(e<0 || e>f) return DONT_INTERSECT;
  } else {
  if(e>0 || e<f) return DONT_INTERSECT;
  }

/*compute intersection coordinates*/

if(f==0) return PARALLEL;
num = d*Ax;						/* numerator */
offset = SAME_SIGNS(num,f) ? f/2 : -f/2;		/* round direction*/
*x = x1 + (num+offset) / f;				/* intersection x */

num = d*Ay;
offset = SAME_SIGNS(num,f) ? f/2 : -f/2;
*y = y1 + (num+offset) / f;				/* intersection y */

return DO_INTERSECT;
}


