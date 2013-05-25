// ******************************************************************
//
// Physically Correct Direct Lighting For Distribution Ray Tracing
//             by Changyaw Wang
//
// triangle_luminaire.h
//
// ******************************************************************

class triangle : public geom_obj {
public:
    point p1;            // vertex
    point p2;            // vertex
    point p3;            // vertex
    vector normal;       // normal vector

    void hit(const point& x,   // viewpoint
	     const vector& v,  // viewing direction
	     const point on_light);   // hit point
	     
    // Selects a point visible from x given (r1,r2).
    // Here, visible means not SELF-shadowed.

    virtual void select_visible_point(
             const point& x,   // viewpoint
             const double r1,  // random number
             const double r2,  // random number
             point& on_light,  // point corresponding to (r1,r2)
             double& prob);    // probability of selecting on_light
};

