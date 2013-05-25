/************************************************************************
**    TEST FILE FOR graph (Dynamic Layout Alg)
**
**    HEADER - GRAPH DATA STRUCTURE MANIPULATION
**
** Author: dr. Szirmay-Kalos Laszlo (szirmay@fsz.bme.hu)
**	   Technical University of Budapest, Hungary
*************************************************************************/
#ifdef MSWINDOWS
#include "mswindow.hxx"
#include "vector.hxx"
#include "defines.h"
#else
#include "window.hxx"
#include "vector.hxx"
#include "defines.h"
#endif

typedef char TYPE;

/************************************************************************/
class Node {
/************************************************************************/
    char   name[MAXNAME + 1];	// node name
    TYPE   type;		// fixed or movable
    vector pos;			// actual position
    vector speed;		// speed
    vector force;		// driving force to this node
public:
	   Node( pchar, TYPE );	     // constructor

    vector& Position( void )	     { return pos;	    }
    vector& Speed( void )	     { return speed;	    }
    vector& Force( void )	     { return force;	    }

    void   AddForce( vector& f )     { force += f;	    }

    pchar  GetName( void )	     { return name;	    }
    TYPE   GetType( void )	     { return type;	    }
};

/************************************************************************/
class Relation {
/************************************************************************/
    char     name[MAXNAME + 1];	   // relation name
    double   intensity;		   // relation intensity
    Node   * relation_to;	   // related node
public:
	     Relation( pchar , Node *, double );
    void     SetRelation( pchar, double );
    pchar    GetName( void )		 { return name;		 }
    double   GetRelation( void )	 { return intensity;	 }
    Node *   GetOtherNode( void )	 { return relation_to;	 }

};

/************************************************************************/
class RelationElem : public Relation {
/************************************************************************/
    RelationElem  *    next_relation;		      // next on the list
public:
    RelationElem( pchar name, Node * p, double r );
    void	   SetNext( RelationElem * rn ) { next_relation = rn;	}
    RelationElem * GetNext( void )		{ return next_relation; }
};

/************************************************************************/
class NodeElem : public Node {
/************************************************************************/
    int		    ser_num;	    // serial number in list
    NodeElem	 *  next_node;	    // pointer to next node
    RelationElem *  relation;	    // first relation of this node
public:
		    NodeElem(pchar, TYPE);
    void	    SetNext( NodeElem *p )	    { next_node = p;	     }
    void	    SetRelation( RelationElem *p )  { relation = p;	     }
    void	    SetSerNum( int sernum )	    { ser_num = sernum;	     }
    NodeElem	 *  GetNext( void )		    { return next_node;	     }
    RelationElem *  GetRelation( void )		    { return relation;	     }
    int		    GetSerNum( void )		    { return ser_num;	     }
};


/************************************************************************/
class Graph {
/************************************************************************/
    int		   nfixnode;	     // number of fix nodes
    int		   nmovnode;	     // number of movable nodes
    NodeElem *	   currnode;	     // current node
    NodeElem *	   relatenode;	     // actual relation of curr
    NodeElem *	   start_node;	     // start of list
    NodeElem *	   last_node;	     // end of list
    RelationElem * currelation;	     // relation of nodes list
    RelationElem * prevrelation;     // previous to currelation

    void	 SwapRelation( void );	  // swap currnode and relatenode
					  // if currnode is further in the
					  // list
public:
		 Graph( void );

    void	 SetNodePos( vector );	  // sets position of currnode
    void	 SetRelation( double );	  // sets intensity of currelation
    NodeElem *	 GetNode( void )	  { return currnode;	       }
    NodeElem *	 GetRelateNode( void )	  { return relatenode;	       }
    double	 GetRelation( void );	  // get intensity of currelation
    pchar	 GetRelationName( void ); // get name of currelation
    BOOL	 AddNode( pchar, TYPE );  // add new node to the list
    void	 AddRelation( pchar, double ); // add new relation
    BOOL	 SearchNode( pchar );	  // search node by name
    BOOL	 RelSearchNode( pchar );
    int		 SearchRelation( void );  // search relation of currnode and relatenode

    BOOL	 SaveNodes( pchar );	  // save to a file
    void	 RestoreNodes( pchar );	  // restore from file

    int		 Placement( void );	  // place nodes step-by-step
    void	 RandomArrange( void );	  // arrange nodes randomly
    int		 DynamicLayout( int );	  // dynamic layout algorithm

    BOOL	 FirstNode( void );	  // select first node on the list
    BOOL	 FirstMoveNode( void );	  // select first moveable node
    BOOL	 NextNode( int max = ALL_NODES ); // select next to currnode
    BOOL	 FirstRelation( void );	  // select first relation of currnode
    BOOL	 NextRelation( void );	  // select next relation

};

/************************************************************************/
class ObjectSpace : public Graph {
/************************************************************************/
    double	     scale_x;		// scale of window->viewport transform
    double	     scale_y;
    RectAngle	     vwindow;	    // object space window
    RectAngle	     viewport;	    // viewport
    void	     SetScale( void );	// calculate scale from vwindow and viewport
public:
	   ObjectSpace( void );

    void   SetViewPort( RectAngle );
    void   SetWindow( RectAngle	 );

    Point  ScreenPos( NodeElem * );    // get screen coordinates of node
    Point  ScreenPos( vector );	       // window -> viewport transform
    Point  ScreenPos( void )	      { return ScreenPos( GetNode( ) );	   }
    Point  RelScreenPos( void )	      { return ScreenPos( GetRelateNode() );}
};


/************************************************************************/
class GraphWindow : public AppWindow {
/************************************************************************/
	ObjectSpace graph;

	void	ExposeAll( ExposeEvent *  );
	void	KeyPressed( KeyEvent * );
	void	ShowNode( void );
	void	ShowRelation( void );

public:
		GraphWindow(int argc, char * argv[] );
};
