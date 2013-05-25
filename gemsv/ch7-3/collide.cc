// -*- C++ -*-
// by Bill Bouma and George Vanecek Jr. Aug, 1994.
// Compile by: g++ -O2 -s -o cull cull.cc algebra3.o -lm
#include "algebra3.h"           // See Graphics Gems IV, pg534-557
typedef vec3          Point;    // Points are not Vectors
typedef vec3          Vector;   // Vectors are not Points
typedef unsigned int  Index;    // Array Indices
typedef unsigned int  Counter;

class Polygon {
public:
  Polygon           ( const char         pId,
                      const Vector&      nV,
                      const Counter      nPs,
                      const Point* const p )
  : id(pId), pts(p), nPts(nPs), normalVector(nV){ }
  const Vector& normal( ) const { return normalVector; }
  char            name( ) const { return id; }
  Counter      nPoints( ) const { return nPts; }
  const Point&   point( const Index i ) const { return pts[i]; }
private:
  const char          id;            // Unique Id
  const Counter       nPts;          // pts[0..nPts-1]
  const Point*  const pts;           // Points around Polygon
  const Vector        normalVector;  // Unit Vector
};

class MovingPolyhedron {
public:
  MovingPolyhedron ( const char           pId,
                     const Vector&        rv,
                     const Vector&        vv,
                     const Vector&        wv,
                     const mat4&          m,
                     const Counter        nP,
                     const Polygon* const ps )
  : id(pId), r(rv), v(vv), w(wv), R(m), polys(ps), nPolys(nP) { }
  const Polygon&    polygon( const Index i ) const { return polys[i]; }
  void                 cull( const MovingPolyhedron& ) const;
private:
  const char           id;      // Unique Id
  const Polygon* const polys;   // Points in local coordinates
  const Counter        nPolys;  // polys[0..nPolys-1]
  Vector               r;       // Center of Rotation (in world coords.)
  Vector               v;       // Linear Velocity (in world coords.)
  Vector               w;       // Angular Velocity (in world coords.)
  mat4                 R;       // Orientation Matrix
};

void MovingPolyhedron::cull( const MovingPolyhedron& j ) const
{
  const mat4   RIi = ((mat4&)R).transpose();
  const Vector aij = RIi * (v - j.v - (w ^ r) + (j.w ^ j.r)); 
  const Vector wij = RIi * (j.w - w);
  for( Index gi = 0; gi < nPolys; ++gi ) {
    const Polygon& g = polygon(gi);
    for( Index pi = 0; pi < g.nPoints(); ++pi )
      if( ( aij + (g.point(pi) ^ wij )) * g.normal() > 0.0 )
        break;
    cout << "Polygon " << g.name() << " of Polyhedron " << id
         << " is" << ( pi == g.nPoints()  ? " " : " not ")
         << "culled." << endl;
  }
}

const Counter NPolyPoints = 4;
const Counter NFaces      = 6;
static const Point leftPoints[NPolyPoints]  = {
  Point(-1,-1,-1), Point(-1,-1, 1), Point(-1, 1, 1), Point(-1, 1,-1) };
static const Point rightPoints[NPolyPoints] = {
  Point( 1,-1,-1), Point( 1, 1,-1), Point( 1, 1, 1), Point( 1,-1, 1) };
static const Point topPoints[NPolyPoints]   = {
  Point(-1, 1,-1), Point(-1, 1, 1), Point( 1, 1, 1), Point( 1, 1,-1) };
static const Point bottomPoints[NPolyPoints]= {
  Point(-1,-1,-1), Point( 1,-1,-1), Point( 1,-1, 1), Point(-1,-1, 1) };
static const Point backPoints[NPolyPoints]  = {
  Point(-1,-1,-1), Point(-1, 1,-1), Point( 1, 1,-1), Point( 1,-1,-1) };
static const Point frontPoints[NPolyPoints] = {
  Point(-1,-1, 1), Point( 1,-1, 1), Point( 1, 1, 1), Point(-1, 1, 1) };
static const Polygon cube[NFaces]= {
  Polygon( 'a', Vector(-1, 0, 0), NPolyPoints, leftPoints   ),
  Polygon( 'b', Vector( 1, 0, 0), NPolyPoints, rightPoints  ),
  Polygon( 'c', Vector( 0, 1, 0), NPolyPoints, topPoints    ),
  Polygon( 'd', Vector( 0,-1, 0), NPolyPoints, bottomPoints ),
  Polygon( 'e', Vector( 0, 0,-1), NPolyPoints, backPoints   ),
  Polygon( 'f', Vector( 0, 0, 1), NPolyPoints, frontPoints  )
};

int main()
{
  MovingPolyhedron A( 'A',
                      Vector(10,10, 0 ), // Position
                      Vector( 0, 0, 0 ), // Velocity
                      Vector( 0, 0, 0 ), // Angular Velocity
                      identity3D(),
                      NFaces, cube );
  MovingPolyhedron B( 'B',
                      Vector(10,10,10 ), // Position
                      Vector( 0, 0,-1 ), // Velocity
                      Vector( 0, 1, 0 ), // Angular Velocity
                      identity3D(),
                      NFaces, cube );
  A.cull( B );
  B.cull( A );               
}
