// ******************************************************************
//
// Physically Correct Direct Lighting For Distribution Ray Tracing
//             by Changyaw Wang
//
// sphere_luminaire.c
//
// ******************************************************************

#include "utility.h"

#ifndef PI
#define PI		3.14159265358979323846
#endif

// Selects a point visible from x given (r1,r2).  
// Here, visible means not SELF-shadowed.

void sphere::select_visible_point(
             const point& x,   // viewpoint
             const double r1,  // random number
             const double r2,  // random number
             point& on_light,  // point corresponding to (r1,r2)
             double& prob)     // probability of selecting on_light
{
   rotation_matrix m;
   double theta, phi, theta_max;
   vector v1, v2, u, v, w, t, psi;

   theta_max = asin(radius/distance(x,center));
   theta = acos(1.0 - r1 + r1 * cos(theta_max)); 
   phi   = 2.0 * PI * r2;
   psi  = spherical_to_vector(theta,phi);
   w = center - x;             // (u,v,w) is (X',Y',Z') in the text
   w.normalize();
   if(fabs(w.data[0]) >= fabs(w.data[1]))
      { t.data[0] = 0.0; t.data[1] = 1.0; t.data[2] = 0.0; } 
   else
      { t.data[0] = 1.0; t.data[1] = 0.0; t.data[2] = 0.0; }
   u = cross(w,t);
   u.normalize();
   v = cross(w,u);
   v.normalize();
   m.set_identity();
   m.set_xyz_to_uvw(u,v,w);
   psi = m * psi;
   psi.normalize();
   hit(x, psi, on_light);       // on_light is x" in the text
   v1 = on_light - center;
   v1.normalize();
   v2 = x - on_light;
   v2.normalize();
   prob = dot(v1,v2) * 0.5 / 
       (distance_squared(x,on_light) * PI * (1.0 - cos(theta_max))); 
}
