// ******************************************************************
//
// Physically Correct Direct Lighting For DistribOAution Ray Tracing
//             by Changyaw Wang
//
// sphere_luminaire.h
//
// ******************************************************************

class sphere : public geom_obj {
public:
    point center;
    double radius;

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

