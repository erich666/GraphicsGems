// ******************************************************************
//
// Physically Correct Direct Lighting For Distribution Ray Tracing
//             by Changyaw Wang
//
// geometry_object.h
//
// ******************************************************************

//  Abstract class geom_obj may be subclassed to a particular type of 
//  object such as a sphere, or a triangle.

class point;

class geom_obj {
public:

    // Selects a point visible from x given (r1,r2).  
    // Here, visible means not SELF-shadowed.

    virtual void select_visible_point(
             const point& x,   // viewpoint
             const double r1,  // random number
             const double r2,  // random number
             point& on_light,  // point corresponding to (r1,r2)
             double& prob);    // probability of selecting on_light
};

