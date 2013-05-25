/************************************************************************
**    TEST FILE FOR graph (Dynamic Layout Alg)
**
**    MODUL - MANIPULATION OF THE GRAPH DATA STRUCTURE
**
** Author: dr. Szirmay-Kalos Laszlo (szirmay@fsz.bme.hu)
**	   Technical University of Budapest, Hungary
*************************************************************************/
#ifdef MSWINDOWS
#include "fileio.hxx"
#else
#include "fileio.hxx"
#endif

/************************************************************************/
/*    NODE								*/
/************************************************************************/
/*-------------------	node constructor      ------------------*/
/* Constructs node   object and initializes			*/
/* IN  : name and type (MOVEABLE or FIXED) of node		*/
/* OUT : -							*/
/*--------------------------------------------------------------*/
Node :: Node(pchar newname, TYPE ntype)
{
    newname[MAXNAME] = '\0';
    strcpy(name,newname);
    type  = ntype;
    force = vector(0.0, 0.0);
    pos	  = vector(0.0, 0.0);
    speed = vector(0.0, 0.0);
}


/************************************************************************/
/*    RELATION								*/
/************************************************************************/
/*------------------- relation constructor    ------------------*/
/* Constructs relation object and initializes			*/
/* IN  : name , related node, intensity,			*/
/*--------------------------------------------------------------*/
Relation :: Relation( pchar nname, Node * np, double r )
{
    SetRelation( nname, r );
    relation_to = np;
}

/*-------------------  SetRelation     -------------------------*/
/* Change the name intensity and relation of a constructed rel	*/
/* IN  : name, intensity					*/
/*--------------------------------------------------------------*/
void Relation :: SetRelation( pchar nname, double r )
{
    strcpy( name, nname );
    intensity = r;
}

/************************************************************************/
/*    NODE ELEM								*/
/************************************************************************/
/*------------------- NodeElem	 constructor  ------------------*/
/* Constructs a node elem of a list object and initializes	*/
/* IN  : name and type (MOVEABLE or FIXED) of node		*/
/*--------------------------------------------------------------*/
NodeElem :: NodeElem( pchar name, TYPE type )
	  : Node( name, type )
{
    next_node = NULL;
    relation = NULL;
}


/************************************************************************/
/*    RELATION ELEM							*/
/************************************************************************/
/*------------------- RelationNode constructor	----------------*/
/* Constructs relation node of a list object and initializes	*/
/* IN  : name , related node, intensity,		       */
/*--------------------------------------------------------------*/
RelationElem :: RelationElem( pchar name, Node * p, double r )
	      : Relation( name, p, r )
{
    next_relation = NULL;
}

/************************************************************************/
/*    GRAPH = NODE - RELATION DATA					*/
/*									*/
/*    The Graph data structure is a dynamic structure.			*/
/*									*/
/*    The nodes are placed on a singly linked list, where fix nodes	*/
/*    are on the beginning, and moveable nodes are on the end.		*/
/*    The nodes are also identified by serial numbers, the moveable	*/
/*    nodes are having positive while fixed nodes negative numbers.	*/
/*    Control pointers : currnode - points to the actual node		*/
/*			 relatenode - other node which forms a pair	*/
/*					with currnode for relation ops	*/
/*			 start_node - the beginning of the list		*/
/*			 last_node - the end of the list		*/
/*									*/
/*    The relations of a given node are stored on an other linked list	*/
/*    connected to the node of the given node. The relation node	*/
/*    contains name, type, intensity parameters and a pointer to the	*/
/*    related node. The relation of two node is stored on the		*/
/*    relation list of the node having smaller serial number!		*/
/*    Control pointers : currelation -points to the actual relation node*/
/*			 prevrelation - points to the relation just	*/
/*			    before currelation on the actual relation	*/
/*			    list.					*/
/*									*/
/*  STRUCTURE OVERVIEW: P = Node, R = RelationNode			*/
/*									*/
/*  start_node				lastnode			*/
/*     P ---> P ---> P ---> P ---> P ---> P ---> NULL			*/
/*     |	     ^	    |	   ^	  ^				*/
/*     R ------------|	    R------|	  |				*/
/*     |		    |		  |				*/
/*    NULL		    R-------------|				*/
/*			    |						*/
/*			   NULL						*/
/************************************************************************/
/*------------------ Graph Constructor -------------------------*/
/* Initializes Graph data structure				*/
/*--------------------------------------------------------------*/
Graph :: Graph()
{
    start_node = NULL;
    last_node  = NULL;
    currnode   = NULL;
    relatenode = NULL;
    currelation	 = NULL;
    prevrelation = NULL;
    nfixnode   = nmovnode = 0;
}

/*------------------ RestoreNodes ----------------------------*/
/* Restores the node-relation data structure from a file	*/
/* The file type is TEXT.					*/
/* IN  : file name						*/
/* SIDE EFFECT: - node-relation data structure is destroyed	*/
/*		  then it is restored from the given file	*/
/*--------------------------------------------------------------*/
void Graph :: RestoreNodes ( pchar file_name )
{
    char    s[MAXLINE + 1];
    char    node_name[MAXNAME + 1];
    char    rel_node_name[MAXNAME + 1];
    char    relation_name[MAXNAME + 1];
    double  x, y;
    double  relation;
    FileIO  fi ( "r" );
    BOOL    first_rel;

    if ( !fi.OpenFile ( file_name ) ) app.Error("Input file does not exists");
/*
*    RESTORE NODES
*/
    for ( ; ; ) {
/*
*    TRY TO INPUT NODE NAME
*/
	if ( !fi.GetKeyWord ( "NAME" ) ) {
/*
*    FAILED -> END OF NODE LIST
*/
	    first_rel = TRUE;
	    break;
	} else {
	    if ( !fi.GetOperator ( '=' ) ) app.Error( "= expected", fi.GetLineNum() );
	    if ( !fi.GetString(node_name,MAXNAME) ) app.Error( "Name expected", fi.GetLineNum() );
/*
*    TRY TO INPUT NODE POSITION
*/
	    if ( !fi.GetKeyWord("POSITION") ) {
/*
*    FAILED -> ASSUME NO POSITION, GENERATE RANDOMLY
*/
		x = (OVERWINDOW_X - WALL_MARGIN * 2.0) /
		    (double)RAND_MAX * (double)rand() + WALL_MARGIN;
		y = (OVERWINDOW_Y - WALL_MARGIN * 2.0) /
		    (double)RAND_MAX * (double)rand() + WALL_MARGIN;
		if ( !fi.GetKeyAgain("TYPE") ) app.Error( "TYPE expected", fi.GetLineNum() );
	    } else {
		if ( !fi.GetOperator ( '=' ) ) app.Error( "= expected", fi.GetLineNum() );
		if ( !fi.GetVariable( &x, 0.0, OVERWINDOW_X ) || !fi.GetVariable( &y, 0.0, OVERWINDOW_Y ) )
			app.Error( "Coordinate out of space", fi.GetLineNum() );
		if ( !fi.GetKeyWord("TYPE") )  app.Error( "TYPE expected", fi.GetLineNum() );
	    }
/*
*    TRY TO INPUT TYPE PARAMETERS
*/
	    if ( !fi.GetOperator ( '=' ) ) app.Error( "= expected", fi.GetLineNum() );
	    if ( !fi.GetString( s, MAXLINE ) ) app.Error("Line too long", fi.GetLineNum() );
/*
*    ADD NEW NODE TO THE DATA STRUCTURE AND CHECK THE NAME IF UNIQUE
*/
	    if ( strcmp( s, "FIXED" ) == 0 ) {
		if ( !AddNode(node_name, FIXED_NODE) )	  app.Error("Not unique node name", fi.GetLineNum() );
	    } else if ( strcmp( s, "MOVEABLE" ) == 0 ) {
		if ( !AddNode(node_name, MOVEABLE_NODE) ) app.Error("Not unique node name", fi.GetLineNum() );
	    } else					  app.Error("Invalid Node type", fi.GetLineNum() );

	    currnode -> Position( ) = vector( x, y );
	}
    }
/*
*    RESTORE RELATIONS
*/
    for( ; ; ) {
/*
*    TRY TO GET RELATION LIST HEAD, IF FAIL -> END OF INPUT
*/
	if ( first_rel ) {
	    if ( !fi.GetKeyAgain ( "RELATIONS" ) )  break;
	    first_rel = FALSE;
	} else {
	    if ( !fi.GetKeyWord ( "RELATIONS" ) )  break;
	}
	if ( !fi.GetKeyWord("OF") )  app.Error("OF expected", fi.GetLineNum() );
	if ( !fi.GetString( node_name, MAXNAME ) ) app.Error("Name too long", fi.GetLineNum() );
	if ( !fi.GetKeyWord ( "NODE" ) ) app.Error("NODE expected", fi.GetLineNum() );
/*
*    IDENTIFY NODE
*/
	if ( !SearchNode( node_name ) ) app.Error("Not declared Node", fi.GetLineNum() );
/*
*    TRY TO INPUT RELATION LIST
*/
	for( ; ; ) {		// get the whole relation list of this node
/*
*    TRY TO GET RELATION NAME
*/
	    if ( !fi.GetKeyWord ( "RELATION" ) ) {
/*
*    FAILED -> CHECK END OF RELATION MARKER
*/
		if ( !fi.GetKeyAgain ( "END" ) ) app.Error("END expected", fi.GetLineNum() );
		else break;
	    }
	    if ( !fi.GetString( relation_name, MAXNAME ) ) app.Error("Name too long", fi.GetLineNum() );
/*
*    CHECK IF NO-NAME RELATION
*/
	    if ( strcmp( relation_name, "*" ) == 0 ) relation_name[0] = '\0';
	    if ( !fi.GetOperator ( ':' ) ) app.Error(": expected", fi.GetLineNum() );
/*
*    TRY TO GET RELATED NODE WITH RELATION PARAMETERS
*/
	    if ( !fi.GetKeyWord("RELATED") )   app.Error("RELATED expected", fi.GetLineNum() );
	    if ( !fi.GetKeyWord("TO") )	       app.Error("TO expected", fi.GetLineNum() );
	    if ( !fi.GetString( rel_node_name, MAXNAME ) ) app.Error("Name expected", fi.GetLineNum() );
	    if ( !fi.GetKeyWord("WITH") )      app.Error("WIDTH expected", fi.GetLineNum() );
	    if ( !fi.GetKeyWord("INTENSITY") ) app.Error("INTENSITY expected", fi.GetLineNum() );

	    if ( !fi.GetVariable( &relation, -MAXRELATION, MAXRELATION ) )
		app.Error("Relation is out of range", fi.GetLineNum() );
/*
*    BUILD THE NEW RELATION INTO THE DATA STRUCTURE
*/
	    NodeElem * tmpnode = currnode;
	    if ( !RelSearchNode( rel_node_name ) ) app.Error("Not declared Node", fi.GetLineNum() );
	    AddRelation( relation_name, relation );
	    currnode = tmpnode;
	}
    }

    fi.CloseFile();
}

/*------------------  SaveNodes	  ----------------------------*/
/* Saves the node-relation data structure to a file name      */
/* The file type is TEXT.					*/
/* IN  : file name						*/
/*--------------------------------------------------------------*/
BOOL Graph :: SaveNodes ( pchar file_name )
{
    char s[MAXLINE];
    FileIO fo ( "w" );

    if ( !fo.OpenFile (file_name) ) return FALSE;
/*
*    SAVE NODES
*/
    if ( !FirstNode() ) {
	fo.CloseFile( );
	return TRUE;
    }

    do {
	sprintf(s,
		"NAME = %s POSITION =  %6.3lf %6.3lf TYPE = %s\n",
		currnode -> GetName(),
		currnode -> Position().X(),
		currnode -> Position().Y(),
		(currnode -> GetType() == FIXED_NODE ? "FIXED" : "MOVEABLE"));
	fo.PutString( s );
    } while ( NextNode() );

/*
*    SAVE RELATIONS
*/
    FirstNode();
    do {
	   sprintf(s,
		    "\nRELATIONS OF %s NODE\n",
		  currnode -> GetName());
	  fo.PutString( s );

	  if ( !FirstRelation() ) {
	    fo.PutString( "END\n" );	    // END OF RELATION MARKER
	      continue;
	  }

	  do {
	      if ( strlen( currelation -> GetName() ) != 0 )
		 sprintf(s, "RELATION %s : ", currelation -> GetName() );
	      else
		 sprintf(s, "RELATION * : ");
	      fo.PutString( s );

	      sprintf(s,
		      "RELATED TO %s WITH INTENSITY %6.3lf \n",
		      currelation -> GetOtherNode() -> GetName(),
		      currelation -> GetRelation() );
	      fo.PutString( s );
	  } while ( NextRelation() );

	  fo.PutString( "END\n" );	  // END OF RELATION MARKER

    } while ( NextNode() );

    fo.CloseFile( );
    return TRUE;
}


/*-------------------	  SetNodePos	  --------------------*/
/* Sets the position of a node				      */
/* IN  : new position						*/
/*--------------------------------------------------------------*/
void Graph :: SetNodePos ( vector p )
{
    if ( currnode != NULL ) currnode -> Position( ) = p;
}

/*------------------------ GetRelation	 -----------------------*/
/* Gets the relation intensity of the actual relation		*/
/* IN  : -							*/
/* OUT : intensity ( -MAXRELATION -> MAXRELATION )		*/
/*--------------------------------------------------------------*/
double Graph :: GetRelation( )
{
    if (currelation != NULL) return currelation -> GetRelation( );
    else		     return 0.0;
}

/*------------------------ GetRelationName ---------------------*/
/* Gets the name of the actual relation				*/
/* OUT : name of NULL if no relation				*/
/*--------------------------------------------------------------*/
pchar Graph :: GetRelationName( )
{
    if ( currelation != NULL ) return currelation -> GetName( );
    else		       return NULL;
}

/*------------------------   AddNode   -----------------------*/
/* Checks if a node having the same name exist and if not new */
/* node is allocated and added to the beginning of the list if*/
/* the node is FIXED or to the end if it is MOVEABLE	      */
/* IN  : name and type of the new node			      */
/* OUT : is this name unique ?				      */
/* SIDE EFFECT: currnode is set to the new node.	      */
/*		if FIXED node				      */
/*		  start_node adjusted, nfixnode incremented   */
/*		if MOVEABLE NODE			      */
/*		  last_node adjusted, nmovnode incremented    */
/*------------------------------------------------------------*/
BOOL Graph :: AddNode ( pchar name, char type )
{
/*
*    DECIDE IF THIS NAME IS UNIQUE, IF NOT RETURN ERROR
*/
    if ( SearchNode( name ) ) return FALSE;

    currnode = new NodeElem(name, type);

    if (start_node == NULL) {
/*
*    IF THIS IS THE FIRST NODE
*/
	start_node = last_node = currnode;
    } else {
	if ( type == FIXED_NODE ) {
/*
*    IF FIXED NODE -> ADD TO THE BEGINNING OF THE LIST
*/
	    currnode -> SetNext( start_node );
	    start_node = currnode;
	} else {
/*
*    IF MOVEABLE NODE -> ADD TO THE END OF THE LIST
*/
	    last_node -> SetNext( currnode );
	    last_node = currnode;
	}
    }
    if ( type == FIXED_NODE ) {
	nfixnode++;
	currnode -> SetSerNum( -nfixnode );
    } else {
	nmovnode++;
	currnode -> SetSerNum( nmovnode );
    }
    return TRUE;
}

/*------------------------  SearchNode	  --------------------*/
/* Searches node by name				      */
/* IN  : searched name					      */
/* OUT : is there node having this name ?		      */
/* SIDE EFFECT: currnode is set to the found node.	      */
/*------------------------------------------------------------*/
BOOL Graph :: SearchNode ( pchar name )
{
    if ( !FirstNode() ) return FALSE;
    do {
	if ( strcmp( currnode -> GetName(), name) == 0 ) return TRUE;
    } while ( NextNode () );

    return FALSE;
}

/*------------------------  RelSearchNode --------------------*/
/* Searches relate node by name				      */
/* IN  : picked position					*/
/* OUT : is there node in the pick aperture ?		      */
/* SIDE EFFECT:							*/
/*  To ensure that relatenode has greater serial number than  */
/*  currnode:						      */
/*     IF found node has smaller serial number than currnode*/
/*	      relatenode is set to currnode		    */
/*	      currnode is set to the found node		    */
/*     ELSE							*/
/*	      relatenode is set to the found node.	    */
/*  Initializes currelation to the relation of currnode and   */
/*  relatenode.						      */
/*--------------------------------------------------------------*/
BOOL Graph :: RelSearchNode ( pchar name )
{
    NodeElem * oldcurrnode = currnode;
    BOOL       found	   = SearchNode( name );

    if ( found ) {
	relatenode = currnode;
	currnode = oldcurrnode;
	SwapRelation( );
	SearchRelation( );
    } else {
	currelation = NULL;
	currnode = oldcurrnode;
    }
    return found;
}

/*------------------ SearchRelation ----------------------------*/
/* Search for a relation between currnode and relatenode.   */
/* If this relation doesnot exist currelation is NULL and	*/
/* prevrelation points to the end of the relation list of	*/
/* currnode.						      */
/* IN  : -							*/
/* OUT : EMPTY_LIST  - No relation list				*/
/*	   FIRST_FOUND - The first relation node found		*/
/*	   FOUND	 - Not the first node found		*/
/*	   NOT_FOUND	 - No such relation			*/
/* SIDE EFFECT: currelation= searched relation or NULL		*/
/*		prevrelation= the previous relation or the last */
/*			      node of the relation list or NULL */
/*			      if no node at all			*/
/*--------------------------------------------------------------*/
int Graph :: SearchRelation ( )
{
    currelation = currnode -> GetRelation( );
    prevrelation = currnode -> GetRelation( );
    if (currelation == NULL) return EMPTY_LIST;
    if (currelation -> GetOtherNode() == relatenode) return FIRST_FOUND;

    currelation = prevrelation -> GetNext();

    for ( ; ; ) {
	if (currelation == NULL)			 return NOT_FOUND;
	if (currelation -> GetOtherNode() == relatenode) return FOUND;
	prevrelation = currelation;
	currelation = prevrelation -> GetNext();
    }
}

/*------------------------  SwapRelation    --------------------*/
/*  To ensure that relatenode has greater serial number than  */
/*  currnode:						      */
/*	IF relatenode node has smaller serial number than   */
/*	relatenode and currnode are swapped		    */
/* IN  : -							*/
/* OUT : -							*/
/*--------------------------------------------------------------*/
void Graph :: SwapRelation ( )
{
    NodeElem * tmpnode;

    if ( currnode == NULL || relatenode == NULL ) return;

    if ( currnode -> GetSerNum() > relatenode -> GetSerNum() ) {
	tmpnode = currnode;
	currnode = relatenode;
	relatenode = tmpnode;
    }
}

/*--------------------- AddRelation ----------------------------*/
/* Adds new or changes the parameters of an existing relation.	*/
/* If this is a new relation RelationNode is allocated and	*/
/* placed on the end of relation list of currnode.	      */
/* The parameters are set according to the explicit parameters	*/
/*  and the implicit relatenode par.			      */
/* IN  : name,intensity, type					*/
/* OUT : -							*/
/* SIDE EFFECT: currelation= new or changed relation		*/
/*--------------------------------------------------------------*/
void Graph :: AddRelation ( pchar name, double rel )
{
/*
*    CHECK IF THIS RELATION EXISTS OR FIND THE END OF RELATION LIST
*/
    switch ( SearchRelation( ) ) {

    case FIRST_FOUND:	     //	   THIS RELATION HAS BEEN ALREADY DEFINED
    case FOUND :
	currelation -> SetRelation( name, rel );
	return;

    case NOT_FOUND:	       //    NOT FIRST ADD NEW RELATION TO THE END OF LIST
	currelation = new RelationElem ( name, relatenode, rel );
	prevrelation -> SetNext ( currelation );
	return;

    case EMPTY_LIST:	    //	   THIS IS GOING TO BE THE FIRST
	currelation = new RelationElem ( name, relatenode, rel );
	currnode -> SetRelation( currelation );
	return;
    }
}

/*--------------------- FirstNode ----------------------------*/
/* Select currnode as start_node (beginning of the list	    */
/* IN  : -							*/
/* OUT : Are nodes on the list				      */
/*--------------------------------------------------------------*/
BOOL Graph :: FirstNode ( )
{
    if ( (currnode = start_node) == NULL ) return FALSE;
    else				   return TRUE;
}

/*--------------------- FirstMoveNode --------------------------*/
/* Select currnode as first moveable node on the list	    */
/* IN  : -							*/
/* OUT : Are moveable nodes on the list			      */
/*--------------------------------------------------------------*/
BOOL Graph :: FirstMoveNode ( )
{
    if ( (currnode = start_node) == NULL ) return FALSE;

    while ( currnode -> GetType() != MOVEABLE_NODE ) {
	currnode = currnode -> GetNext();
	if ( currnode == NULL ) return FALSE;
    }
    return TRUE;
}

/*---------------------	 NextNode ----------------------------*/
/* Let currnode be the next after currnode		      */
/* IN  : maximal serial number to be considered of ALL_NODES  */
/* OUT : Was it the last node?				      */
/* SIDE EFFECT: currnode = NULL if no more nodes	      */
/*------------------------------------------------------------*/
BOOL Graph :: NextNode ( int maxsernum )
{
    if ( maxsernum == ALL_NODES ) {
	if (( currnode = currnode -> GetNext() ) == NULL ) return FALSE;
    } else {
	if (( currnode = currnode -> GetNext() ) == NULL ||
	      currnode -> GetSerNum() > maxsernum ) return FALSE;
    }
    return TRUE ;
}

/*---------------------	 FirstRelation	------------------------*/
/* Select currrelation as first relation of the relation list	*/
/* of currnode.						      */
/* IN  : -							*/
/* OUT : Has the currnode any relation?			      */
/*--------------------------------------------------------------*/
BOOL Graph :: FirstRelation ( )
{
    if ( (currelation = currnode -> GetRelation()) == NULL ) {
	relatenode = NULL;
	return FALSE;
    } else {
	relatenode = (NodeElem *)( currelation -> GetOtherNode() );
	return TRUE;
    }
}

/*---------------------	 NextRelation --------------------------*/
/* Let currelation the next after currelation			*/
/* IN  : -							*/
/* OUT : Was it the last relation of the list?			*/
/*--------------------------------------------------------------*/
BOOL Graph :: NextRelation ( )
{
    if ( (currelation = currelation -> GetNext()) == NULL ) {
	relatenode = NULL;
	return FALSE;
    } else {
	relatenode = (NodeElem *)( currelation -> GetOtherNode() );
	return TRUE;
    }
}

/*------------------   RandomArrange ---------------------------*/
/* Random arrangement of nodes					*/
/*--------------------------------------------------------------*/
void Graph :: RandomArrange( )
{
/*
*    SKIP FIXED NODES
*/
    if ( !FirstMoveNode() ) return;
/*
*     MAIN CYCLE OF PLACING MOVEABLE NODES RANDOMLY
*/
    do {
	currnode -> Position( ) = vector((OVERWINDOW_X - WALL_MARGIN * 2.0) /
					 (double)RAND_MAX * (double)rand() + WALL_MARGIN,
					 (OVERWINDOW_Y - WALL_MARGIN * 2.0) /
					 (double)RAND_MAX * (double)rand() + WALL_MARGIN );
    } while ( NextNode() );
}

/************************************************************************/
/*  OBJECT SPACE							*/
/************************************************************************/
/*----------------- ObjectSpace Constructor --------------------*/
/* Initializes object space window				*/
/* IN  : -							*/
/* OUT : -							*/
/*--------------------------------------------------------------*/
ObjectSpace :: ObjectSpace( )
	:vwindow( 0, 0, (CoOrd)OVERWINDOW_Y, (CoOrd)OVERWINDOW_X ),
	 viewport( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT )
{
}

/*------------------------ SetScale ----------------------------*/
/* Initializes window -> viewport transform			*/
/* IN  : -							*/
/* OUT : -							*/
/*--------------------------------------------------------------*/
void ObjectSpace :: SetScale()
{
    scale_x = (double)viewport.Width() / (double)vwindow.Width();
    scale_y = (double)viewport.Height() / (double)vwindow.Height();
}


/*--------------------- SetViewPort ----------------------------*/
/* Sets viewport (Canvas RectAngle)				*/
/* IN  : new viewport						*/
/* OUT : -							*/
/* SIDE EFFECT: Recalculates window->viewport transform		*/
/*--------------------------------------------------------------*/
void ObjectSpace :: SetViewPort( RectAngle v )
{
    viewport = v;
    SetScale();
}


/*--------------------- ScreenPos ------------------------------*/
/* Transform a point from object space to screen space		*/
/* IN  : object space position					*/
/* OUT : screen space coordinates of point			*/
/*--------------------------------------------------------------*/
Point ObjectSpace :: ScreenPos( vector p )
{
    CoOrd x = (CoOrd)((p.X()-(double)vwindow.HorPos()) * scale_x);
    CoOrd y = (CoOrd)((p.Y()-(double)vwindow.VerPos()) * scale_y);
    return Point(x,y);
}

/*--------------------- ScreenPos ------------------------------*/
/* Gets the position of a NODE in screen coordinate system	*/
/* IN  : pointer to the NODE					*/
/* OUT : screen space coordinates of NODE position		*/
/*--------------------------------------------------------------*/
Point ObjectSpace :: ScreenPos( NodeElem * pnode )
{
    return ScreenPos( pnode -> Position() );
}

/************************************************************************/
/*  GRAPH WINDOW							*/
/************************************************************************/
/*-----------------   GraphWindow constructor ------------------*/
/* Reads the input file defined in argv[1]			*/
/*--------------------------------------------------------------*/
GraphWindow :: GraphWindow( int argc, char * argv[] )
	     : AppWindow( argc, argv )
{
	if ( argc > 1 ) {
		graph.RestoreNodes( argv[1] );
	} else	app.Error( "Input file missing" );
}

/*------------------------   ExposeAll	  ----------------------*/
/* Redraw the graph on the screen				*/
/*--------------------------------------------------------------*/
void GraphWindow :: ExposeAll( ExposeEvent * event )
{
    Text( "<L> = Layout Algorithm", Point(20, 20) );
    Text( "<R> = Random Arrange", Point(20, 40) );
    Text( "<Q> = Quit & Save", Point(20, 60) );

/*
*    SET WINDOW	 - VIEWPORT TRANSFORM
*/
    if ( event ) graph.SetViewPort( Canvas() );
/*
*    DISPLAY RELATIONS
*/
    graph.FirstNode();
    do {
	if ( graph.FirstRelation() ) {
	    do ShowRelation(); while ( graph.NextRelation() );
	}
    } while ( graph.NextNode() );
/*
*    DISPLAY NODES
*/
    if ( !graph.FirstNode() ) return;
    do ShowNode( ); while ( graph.NextNode() );
}

/*---------------------	  MouseButtonDn	  ----------------------*/
/* React to Mouse button down event!				*/
/*--------------------------------------------------------------*/
void GraphWindow :: KeyPressed( KeyEvent * event )
{
	switch ( event -> GetASCII() ) {
	case 'L':
	case 'l': switch ( graph.Placement() ) {
		  case STOPPED: RePaint();
				break;
		  case INSTABLE:app.Warning("Instable system");
				break;
		  case TOO_LONG:app.Warning("Solution takes too long");
				break;
		  }
		  break;
	case 'R':
	case 'r': graph.RandomArrange();
		  RePaint();
		  break;
	case 'Q':
	case 'q': graph.SaveNodes( "ggg.dat" );
		  app.Quit();
	}
}

/*---------------------	  ShowNode    ------------------------*/
/* Shows current node as a rectangle and a text		      */
/*------------------------------------------------------------*/
void GraphWindow :: ShowNode( )
{

    DrawRectangle( RectAngle( graph.ScreenPos().X() - NODESIZE_X / 2,
			      graph.ScreenPos().Y() - NODESIZE_Y / 2,
			      NODESIZE_X, NODESIZE_Y) );

    Text( graph.GetNode() -> GetName(), graph.ScreenPos() );
}

/*---------------------	  ShowRelation	  ----------------------*/
/* Shows the current relation as a line and a text		*/
/*--------------------------------------------------------------*/
void GraphWindow :: ShowRelation( )
{
    MoveTo( graph.ScreenPos() );
    LineTo( graph.RelScreenPos() );
    Text( graph.GetRelationName(),
	  Point( (graph.ScreenPos().X() + graph.RelScreenPos().X()) / 2,
		 (graph.ScreenPos().Y() + graph.RelScreenPos().Y()) / 2 ) );
}

/************************************************************************/
/*  WINDOW MANAGER INDEPENDENT ENTRY POINT				*/
/************************************************************************/
void App :: Start( int argc, char * argv[] )
{
	GraphWindow graphwindow( argc, argv );
	graphwindow.MessageLoop( );
}
