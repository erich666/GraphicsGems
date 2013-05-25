#include <quadedge.h>

/*********************** Basic Topological Operators ************************/

Edge* MakeEdge()
{
	QuadEdge *ql = new QuadEdge;
	return ql->e;
}

void Splice(Edge* a, Edge* b)
// This operator affects the two edge rings around the origins of a and b,
// and, independently, the two edge rings around the left faces of a and b.
// In each case, (i) if the two rings are distinct, Splice will combine
// them into one; (ii) if the two are the same ring, Splice will break it
// into two separate pieces.
// Thus, Splice can be used both to attach the two edges together, and
// to break them apart. See Guibas and Stolfi (1985) p.96 for more details
// and illustrations.
{
	Edge* alpha = a->Onext()->Rot();
	Edge* beta  = b->Onext()->Rot();

	Edge* t1 = b->Onext();
	Edge* t2 = a->Onext();
	Edge* t3 = beta->Onext();
	Edge* t4 = alpha->Onext();

	a->next = t1;
	b->next = t2;
	alpha->next = t3;
	beta->next = t4;
}

void DeleteEdge(Edge* e)
{
	Splice(e, e->Oprev());
	Splice(e->Sym(), e->Sym()->Oprev());
	delete e->Qedge();
}

/************* Topological Operations for Delaunay Diagrams *****************/

Subdivision::Subdivision(const Point2d& a, const Point2d& b, const Point2d& c)
// Initialize a subdivision to the triangle defined by the points a, b, c.
{
	Point2d *da, *db, *dc;
	da = new Point2d(a), db = new Point2d(b), dc = new Point2d(c);
	Edge* ea = MakeEdge();
	ea->EndPoints(da, db);
	Edge* eb = MakeEdge();
	Splice(ea->Sym(), eb);
	eb->EndPoints(db, dc);
	Edge* ec = MakeEdge();
	Splice(eb->Sym(), ec);
	ec->EndPoints(dc, da);
	Splice(ec->Sym(), ea);
	startingEdge = ea;
}

Edge* Connect(Edge* a, Edge* b)
// Add a new edge e connecting the destination of a to the
// origin of b, in such a way that all three have the same
// left face after the connection is complete.
// Additionally, the data pointers of the new edge are set.
{
	Edge* e = MakeEdge();
	Splice(e, a->Lnext());
	Splice(e->Sym(), b);
	e->EndPoints(a->Dest(), b->Org());
	return e;
}

void Swap(Edge* e)
// Essentially turns edge e counterclockwise inside its enclosing
// quadrilateral. The data pointers are modified accordingly.
{
	Edge* a = e->Oprev();
	Edge* b = e->Sym()->Oprev();
	Splice(e, a);
	Splice(e->Sym(), b);
	Splice(e, a->Lnext());
	Splice(e->Sym(), b->Lnext());
	e->EndPoints(a->Dest(), b->Dest());
}

/*************** Geometric Predicates for Delaunay Diagrams *****************/

inline Real TriArea(const Point2d& a, const Point2d& b, const Point2d& c)
// Returns twice the area of the oriented triangle (a, b, c), i.e., the
// area is positive if the triangle is oriented counterclockwise.
{
	return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
}

int InCircle(const Point2d& a, const Point2d& b,
			 const Point2d& c, const Point2d& d)
// Returns TRUE if the point d is inside the circle defined by the
// points a, b, c. See Guibas and Stolfi (1985) p.107.
{
	return (a.x*a.x + a.y*a.y) * TriArea(b, c, d) -
	       (b.x*b.x + b.y*b.y) * TriArea(a, c, d) +
	       (c.x*c.x + c.y*c.y) * TriArea(a, b, d) -
	       (d.x*d.x + d.y*d.y) * TriArea(a, b, c) > 0;
}

int ccw(const Point2d& a, const Point2d& b, const Point2d& c)
// Returns TRUE if the points a, b, c are in a counterclockwise order
{
	return (TriArea(a, b, c) > 0);
}

int RightOf(const Point2d& x, Edge* e)
{
	return ccw(x, e->Dest2d(), e->Org2d());
}

int LeftOf(const Point2d& x, Edge* e)
{
	return ccw(x, e->Org2d(), e->Dest2d());
}

int OnEdge(const Point2d& x, Edge* e)
// A predicate that determines if the point x is on the edge e.
// The point is considered on if it is in the EPS-neighborhood
// of the edge.
{
	Real t1, t2, t3;
	t1 = (x - e->Org2d()).norm();
	t2 = (x - e->Dest2d()).norm();
	if (t1 < EPS || t2 < EPS)
	    return TRUE;
	t3 = (e->Org2d() - e->Dest2d()).norm();
	if (t1 > t3 || t2 > t3)
	    return FALSE;
	Line line(e->Org2d(), e->Dest2d());
	return (fabs(line.eval(x)) < EPS);
}

/************* An Incremental Algorithm for the Construction of *************/
/************************ Delaunay Diagrams *********************************/

Edge* Subdivision::Locate(const Point2d& x)
// Returns an edge e, s.t. either x is on e, or e is an edge of
// a triangle containing x. The search starts from startingEdge
// and proceeds in the general direction of x. Based on the
// pseudocode in Guibas and Stolfi (1985) p.121.
{
	Edge* e = startingEdge;

	while (TRUE) {
		if (x == e->Org2d() || x == e->Dest2d())
		    return e;
		else if (RightOf(x, e))
			 e = e->Sym();
		else if (!RightOf(x, e->Onext()))
			 e = e->Onext();
		else if (!RightOf(x, e->Dprev()))
			 e = e->Dprev();
		else
		    return e;
	}
}

void Subdivision::InsertSite(const Point2d& x)
// Inserts a new point into a subdivision representing a Delaunay
// triangulation, and fixes the affected edges so that the result
// is still a Delaunay triangulation. This is based on the
// pseudocode from Guibas and Stolfi (1985) p.120, with slight
// modifications and a bug fix.
{
	Edge* e = Locate(x);
	if ((x == e->Org2d()) || (x == e->Dest2d()))  // point is already in
	    return;
	else if (OnEdge(x, e)) {
		e = e->Oprev();
		DeleteEdge(e->Onext());
	}

	// Connect the new point to the vertices of the containing
	// triangle (or quadrilateral, if the new point fell on an
	// existing edge.)
	Edge* base = MakeEdge();
	base->EndPoints(e->Org(), new Point2d(x));
	Splice(base, e);
	startingEdge = base;
	do {
		base = Connect(e, base->Sym());
		e = base->Oprev();
	} while (e->Lnext() != startingEdge);

	// Examine suspect edges to ensure that the Delaunay condition
	// is satisfied.
	do {
		Edge* t = e->Oprev();
		if (RightOf(t->Dest2d(), e) &&
			InCircle(e->Org2d(), t->Dest2d(), e->Dest2d(), x)) {
				Swap(e);
				e = e->Oprev();
		}
		else if (e->Onext() == startingEdge)  // no more suspect edges
			return;
		else  // pop a suspect edge
		    e = e->Onext()->Lprev();
	} while (TRUE);
}

/*****************************************************************************/

#include <gl.h>
static unsigned int timestamp = 0;

void Subdivision::Draw()
{
	if (++timestamp == 0)
		timestamp = 1;
	startingEdge->Draw(timestamp);
}

void Edge::Draw(unsigned int stamp)
// This is a recursive drawing routine that uses time stamps to
// determine if the edge has already been drawn. This is given
// here for testing purposes only: it is not efficient, and for
// large triangulations the stack might overflow. A better way
// of doing this (and other traversals of the edges) is to maintain
// a list of edges in the corresponding Subdivision object. This
// list should be updated every time an edge is created or destroyed.
{
	if (Qedge()->TimeStamp(stamp)) {

		// Draw the edge
		Point2d a = Org2d();
		Point2d b = Dest2d();
		bgnline();
		v2d((double*)&a);
		v2d((double*)&b);
		endline();

		// visit neighbors
		Onext()->Draw(stamp);
		Oprev()->Draw(stamp);
		Dnext()->Draw(stamp);
		Dprev()->Draw(stamp);
	}
}
