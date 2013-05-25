// ******************************************************************
//
// Physically Correct Direct Lighting For Distribution Ray Tracing
//             by Changyaw Wang
//
// triangle_luminaire.c
//
// ******************************************************************

#include "utility.h"

// Selects a point visible from x given (r1,r2).
// Here, visible means not SELF-shadowed.

void triangle::select_visible_point(
               const point& x,   // viewpoint
               const double r1,  // random number
               const double r2,  // random number
               point& on_light,  // point corresponding to (r1,r2)
               double& prob)     // probability of selecting on_light
{
   point pt, pt1, pt2, pt3;
   vector v1, v2, v3, psi, temp1, temp2;
   double u, v, area;

   v1 = p1 - x;
   v1.normalize();
   pt1 = x + v1;
   v2 = p2 - x;
   v2.normalize();
   pt2 = x + v2;
   v3 = p3 - x;
   v3.normalize();
   pt3 = x + v3;
   u = 1.0 - sqrt(1.0 - r1);
   v = r2 * sqrt(1.0 - r1);
  
   pt = pt1 + u*(pt2 - pt1) + v*(pt3 - pt1);
   psi = pt - x;
   psi.normalize();
   hit(x, psi, on_light);    
   temp1 = pt2 - pt1;
   temp2 = pt3 - pt1; 
   temp1 = cross(temp1,temp2);
   // area is the area of pt1,pt2,pt3
   area = 0.5 * sqrt(dot(temp1, temp1)); 
   temp1.normalize();
   prob =  distance_squared(x,pt)*dot(-1.0*psi,normal) /
          (distance_squared(x,on_light)*dot(-1.0*psi,temp1)*area); 
}

