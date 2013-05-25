#ifndef QUADEDGE_H
#define QUADEDGE_H

#include <geom2d.h>

class QuadEdge;

class Edge {
	friend QuadEdge;
	friend void Splice(Edge*, Edge*);
  private:
	int num;
	Edge *next;
	Point2d *data;
  public:
	Edge()			{ data = 0; }
	Edge* Rot();
	Edge* invRot();
	Edge* Sym();
	Edge* Onext();
	Edge* Oprev();
	Edge* Dnext();
	Edge* Dprev();
	Edge* Lnext();
	Edge* Lprev();
	Edge* Rnext();
	Edge* Rprev();
	Point2d* Org();
	Point2d* Dest();
	const Point2d& Org2d() const;
	const Point2d& Dest2d() const;
	void  EndPoints(Point2d*, Point2d*);
	QuadEdge* Qedge()	{ return (QuadEdge *)(this - num); }
	void Draw(unsigned int);
};

class QuadEdge {
	friend Edge *MakeEdge();
  private:
	Edge e[4];
	unsigned int ts;
  public:
	QuadEdge();
	int TimeStamp(unsigned int);
};

class Subdivision {
  private:
	Edge *startingEdge;
	Edge *Locate(const Point2d&);
  public:
	Subdivision(const Point2d&, const Point2d&, const Point2d&);
	void InsertSite(const Point2d&);
	void Draw();
};

inline QuadEdge::QuadEdge()
{
	e[0].num = 0, e[1].num = 1, e[2].num = 2, e[3].num = 3;
	e[0].next = &(e[0]); e[1].next = &(e[3]);
	e[2].next = &(e[2]); e[3].next = &(e[1]);
	ts = 0;
}

inline int QuadEdge::TimeStamp(unsigned int stamp)
{
	if (ts != stamp) {
		ts = stamp;
		return TRUE;
	} else
		return FALSE;
}

/************************* Edge Algebra *************************************/

inline Edge* Edge::Rot()
// Return the dual of the current edge, directed from its right to its left.
{
	return (num < 3) ? this + 1 : this - 3;
}

inline Edge* Edge::invRot()
// Return the dual of the current edge, directed from its left to its right.
{
	return (num > 0) ? this - 1 : this + 3;
}

inline Edge* Edge::Sym()
// Return the edge from the destination to the origin of the current edge.
{
	return (num < 2) ? this + 2 : this - 2;
}

inline Edge* Edge::Onext()
// Return the next ccw edge around (from) the origin of the current edge.
{
	return next;
}

inline Edge* Edge::Oprev()
// Return the next cw edge around (from) the origin of the current edge.
{
	return Rot()->Onext()->Rot();
}

inline Edge* Edge::Dnext()
// Return the next ccw edge around (into) the destination of the current edge.
{
	return Sym()->Onext()->Sym();
}

inline Edge* Edge::Dprev()
// Return the next cw edge around (into) the destination of the current edge.
{
	return invRot()->Onext()->invRot();
}

inline Edge* Edge::Lnext()
// Return the ccw edge around the left face following the current edge.
{
	return invRot()->Onext()->Rot();
}

inline Edge* Edge::Lprev()
// Return the ccw edge around the left face before the current edge.
{
	return Onext()->Sym();
}

inline Edge* Edge::Rnext()
// Return the edge around the right face ccw following the current edge.
{
	return Rot()->Onext()->invRot();
}

inline Edge* Edge::Rprev()
// Return the edge around the right face ccw before the current edge.
{
	return Sym()->Onext();
}

/************** Access to data pointers *************************************/

inline Point2d* Edge::Org()
{
	return data;
}

inline Point2d* Edge::Dest()
{
	return Sym()->data;
}

inline const Point2d& Edge::Org2d() const
{
	return *data;
}

inline const Point2d& Edge::Dest2d() const
{
	return (num < 2) ? *((this + 2)->data) : *((this - 2)->data);
}

inline void Edge::EndPoints(Point2d* or, Point2d* de)
{
	data = or;
	Sym()->data = de;
}

#endif /* QUADEDGE_H */
