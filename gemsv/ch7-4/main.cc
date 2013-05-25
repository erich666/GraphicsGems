// -*- C++ -*-
// Gems V: Spatial Partitioning of a Polygon by a Plane
// by George Vanecek Jr, Sept. 1994

#include "polygon.h"

static void printPolys( const char* const label, const List<Polygon>& pL )
{
  if( pL.size() ) {
    cout << "----------" << endl
	 << pL.size() << " polygon(s) " << label << endl;
    forEachItemOnList( pL ) {
      cout << "  Polygon:" << endl;
      const Polygon* const g = getItem(Polygon);
      forEachDEdgeOfPoly(d1,g) {
	const Point& p = d1->srcPoint();
	cout << "  " << p.x() << ' ' << p.y() << ' ' << p.z() << endl;
      }
    }
  }
}

int main( )
{
  // Sample polygon shown in Figure 1(a) of the Gems V article.
  const Point pts[] = {
    Point( 0,0,0), Point( 2,0,0), Point( 3,3,0),
    Point( 4,0,0), Point( 9,0,0), Point( 9,3,0),
    Point(10,3,0), Point(10,0,0), Point(13,0,0),
    Point(13,3,0), Point(14,3,0), Point(14,6,0),
    Point( 6,6,0), Point( 6,2,0), Point( 7,2,0),
    Point( 7,5,0), Point(12,5,0), Point(12,1,0),
    Point(11,1,0), Point(11,4,0), Point( 8,4,0),
    Point( 8,1,0), Point( 5,1,0), Point( 5,3,0),
    Point( 4,3,0), Point( 4,6,0), Point( 3,6,0),
    Point( 2,3,0), Point( 1,6,0), Point( 0,6,0)
  };
  Polygon* g = new Polygon( 30, pts );
  cout << "Before:" << endl;
  forEachDEdgeOfPoly(d1,g)
    cout << d1->srcPoint() << endl;
  List<Polygon> above;
  List<Polygon> on;
  List<Polygon> below;
  split( g, Plane(Vector(0.0,1.0,0.0),-3.0), above, on, below);
  printPolys( "Above", above);
  printPolys( "On",    on);
  printPolys( "Below", below);
}
