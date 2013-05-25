#include "pt2.h"

/*************************************************************/
/*                                                           */
/* Two utility functions to add and subtract 2D integer      */
/* points.                                                   */
/*                                                           */
/*************************************************************/

pt2* addPt2(pt2 *a, pt2 *b, pt2 *c)
{
c->x = a->x + b->x;
c->y = a->y + b->y;
return c;
}

pt2* subPt2(pt2 *a, pt2 *b, pt2 *c)
{
c->x = a->x - b->x;
c->y = a->y - b->y;
return c;
}
