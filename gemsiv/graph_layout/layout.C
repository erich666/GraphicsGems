/****************************************************************************
** DYNAMIC LAYOUT ALGORITHM OF GENERAL GRAPHS
**
** Author: dr. Szirmay-Kalos Laszlo (szirmay@fsz.bme.hu)
**	   Technical University of Budapest, Hungary
*****************************************************************************/

#ifdef MSWINDOWS
#include "graph.hxx"
#else
#include "graph.hxx"
#endif

/*
*    CONSTANTS
*/
const double TIME_STEP = 0.1;	     // time step of diff equ

const double MAX_FORCE = 500.0;	     // force shows instability
const double MIN_FORCE = 2.0;	     // force cosidered as 0
const double MAX_TIME_SCALE = 10.0;  // scale of max time of solution

const double MINFRICTION = 0.6;	     // friction boundaries
const double MAXFRICTION = 0.9;
const double MINIINERTIA = 0.1;	     // inverse inertia boundaries
const double MAXIINERTIA = 0.4;

const double ZERO_DIST =	10.0;  // distance considered as 0
const double WALL_OUT_DRIVE =	80.0;  // forces of the wall
const double WALL_MARGIN_DRIVE = 1.0;

const double SCALECONSTRAINT = OVERWINDOW_X / 3.5 / MAXRELATION;
const double MINCONSTRAINT = OVERWINDOW_X / 7.0;     // minimal constraint


/****************************************************************************/
/* DYNAMIC LAYOUT base on MECHANICAL SYSTEM ANALOGY			    */
/* IN  : The serial number of the maximal moveable node to be considered    */
/* OUT : STOPPED  = All objects stopped					    */
/*	 INSTABLE = Instable, force goes to infinity			    */
/*	 TOO_LONG = Too much time elapsed				    */
/****************************************************************************/
int Graph :: DynamicLayout( int maxsernum )
/*--------------------------------------------------------------------------*/
{
/*
*    LOCALS
*/
    double constraint, friction, iinertia, dist;
    vector drive;			// drive forces
    vector direction;			// direction of drives
    double MAX_TIME = MAX_TIME_SCALE * (nmovnode + nfixnode + 1);
/*
*    INIT SPEED OF MOVEABLE NODES TO 0
*/
    if ( !FirstMoveNode() ) return STOPPED;
    do	currnode -> Speed( ) = vector(0.0, 0.0); while ( NextNode( maxsernum ) );
/*
*    MAIN CYCLE OF TIME IN THE SOLUTION OF DIFF EQUATION
*/
    for ( double t = 0.0 ; t < MAX_TIME ; t += TIME_STEP ) {
/*
*    INITIALIZE FORCE IN NODES TO 0
*/
	FirstNode();
	do currnode -> Force( ) = vector( 0.0, 0.0 ); while ( NextNode( maxsernum ) );
/*
*    CALCULATE FRICTION AND RESPONSE VALUES FROM t
*/
	friction = MINFRICTION + (MAXFRICTION - MINFRICTION) * t / MAX_TIME;
	iinertia = MAXIINERTIA - (MAXIINERTIA - MINIINERTIA) * t / MAX_TIME;
/*
*     CALCULATE DRIVE FORCE BETWEEN EACH PAIR OF NODES
*/
	FirstNode();
	do {
	  relatenode = currnode -> GetNext();
	  while ( relatenode != NULL && relatenode -> GetSerNum() <= maxsernum ) {

	    direction = currnode -> Position( ) - relatenode -> Position( );
	    dist  = direction.Size();
	    if ( dist < ZERO_DIST ) dist = ZERO_DIST;
/*
*    CALCULATE FORCE FROM THEIR RELATION
*/
	    switch( SearchRelation() ) {
	    case EMPTY_LIST:
	    case NOT_FOUND:
		constraint = MINCONSTRAINT + MAXRELATION * SCALECONSTRAINT;
		break;
	    case FOUND:
	    case FIRST_FOUND:
		constraint = MINCONSTRAINT + (MAXRELATION - currelation->GetRelation()) * SCALECONSTRAINT;
		break;
	    }
	    //	SET FORCE
	    drive = (constraint - dist) / dist * direction;
	    drive /= (double)(maxsernum + nfixnode);
	    currnode -> AddForce(drive);
	    relatenode -> AddForce(-drive);

	    relatenode = relatenode -> GetNext();
	  }
	} while ( NextNode( maxsernum ) );
/*
*    ADD ADDITIONAL FORCES AND DETERMINE MAXIMAL FORCE
*/
	double max_force = 0.0;

	FirstMoveNode();
	do {
/*
*   CALCULATE DRIVE FORCE OF BOUNDARIES AND ADD TO RELATION FORCES
*/
	   dist = currnode -> Position().X();
	   /*
	   *	FORCE OF LEFT WALL
	   */
	   if (dist < 0) {	      // OUT LEFT
	     drive = vector( -dist * WALL_OUT_DRIVE + WALL_MARGIN * WALL_MARGIN_DRIVE, 0.0 );
	     currnode -> AddForce(drive);
	   } else if (dist < WALL_MARGIN) {    // IN LEFT MARGIN
	     drive = vector((WALL_MARGIN - dist) * WALL_MARGIN_DRIVE, 0.0);
	     currnode -> AddForce(drive);
	   }
	   /*
	   *	FORCE OF THE RIGHT WALL
	   */
	   dist = currnode -> Position().X() - OVERWINDOW_X;

	   if (dist > 0) {	      // OUT RIGHT
	     drive = vector( -dist * WALL_OUT_DRIVE + WALL_MARGIN * WALL_MARGIN_DRIVE, 0.0);
	     currnode -> AddForce(drive);
	   } else if (-dist < WALL_MARGIN) {	// IN RIGHT MARGIN
	     drive = vector((-WALL_MARGIN - dist) * WALL_MARGIN_DRIVE, 0.0);
	     currnode -> AddForce(drive);
	   }

	   dist = currnode -> Position().Y();
	   /*
	   *	FORCE OF BOTTOM WALL
	   */
	   if (dist < 0) {	      // OUT BOTTOM
	     drive = vector(0.0, -dist * WALL_OUT_DRIVE + WALL_MARGIN * WALL_MARGIN_DRIVE );
	     currnode -> AddForce(drive);
	   } else if (dist < WALL_MARGIN) {    // IN BOTTOM MARGIN
	     drive = vector(0.0, (WALL_MARGIN - dist) * WALL_MARGIN_DRIVE);
	     currnode -> AddForce(drive);
	   }
	   /*
	   *	FORCE OF THE TOP WALL
	   */
	   dist = currnode -> Position().Y() - OVERWINDOW_Y;

	   if (dist > 0) {				      // OUT TOP
	     drive = vector( 0.0, -dist * WALL_OUT_DRIVE + WALL_MARGIN * WALL_MARGIN_DRIVE );
	     currnode -> AddForce(drive);
	   } else if (-dist < WALL_MARGIN) {		    // IN TOP MARGIN
	     drive = vector(0.0, (-WALL_MARGIN - dist) * WALL_MARGIN_DRIVE);
	     currnode -> AddForce(drive);
	   }
/*
*    MOVE NODE BY FORCE
*/
	    vector old_speed = currnode -> Speed( );
	    currnode -> Speed( ) = (1.0 - friction) * old_speed + iinertia * currnode -> Force( );
	    currnode -> Position( ) += 0.5 * (old_speed + currnode -> Speed( ) );

/*
*    CALCULATE MAXIMUM FORCE
*/
	    double abs_force = currnode -> Force().Size( );
	    if ( abs_force > max_force) max_force = abs_force;

	} while ( NextNode( maxsernum ) );
/*
*    STOP CALCULATION IF
*/
	if ( max_force < MIN_FORCE ) return STOPPED;  // All objects stopped
	if ( max_force > MAX_FORCE ) return INSTABLE; // Instable, force goes to infinity
    }
    return TOO_LONG; // Too much time elapsed
}

/************************************************************************/
/*    INITIAL PLACEMENT ALGORITHM					*/
/* OUT : STOPPED  = All objects stopped					*/
/*	 INSTABLE = Instable, force goes to infinity			*/
/*	 TOO_LONG = Too much time elapsed				*/
/************************************************************************/
int Graph :: Placement( )
/*----------------------------------------------------------------------*/
{
    vector	candidate;		// candidate position
    vector	relate_cent;		// center related objects
    vector	notrel_cent;		// center of related object
    vector	center( OVERWINDOW_X / 2, OVERWINDOW_Y / 2 );
    int		nrel;			// number of related objects
    int		nnotrel;		// displayed nodes
    double	perturb_x = OVERWINDOW_X / (double)RAND_MAX ;
    double	perturb_y = OVERWINDOW_Y / (double)RAND_MAX ;

/*
*    SKIP FIXED NODES
*/
    if ( !FirstMoveNode() ) return STOPPED;
/*
*     MAIN CYCLE OF INTRODUCING MOVABLE NODES STEP-BY-STEP
*/
    for( int inode = 1; ; inode++ ) {
/*
*    CALCULATE THE CENTER OF GRAVITY OF ALREADY INTRODUCED NODES
*    relate_cent IS FOR RELATED NODES
*    notrel_cent IS FOR NON_RELATED NODES
*/
	relate_cent = vector(0.0, 0.0);
	notrel_cent = vector(0.0, 0.0);
	nrel = 0;
	nnotrel = 0;		    // displayed nodes
	relatenode = currnode;

	for( FirstNode(); currnode != relatenode; NextNode() ) {
	    switch ( SearchRelation() ) {
	    case EMPTY_LIST:
	    case NOT_FOUND:
		    notrel_cent += currnode -> Position();
		    nnotrel++;
		    break;
	    case FIRST_FOUND:
	    case FOUND:
		    relate_cent += currnode -> Position();
		    nrel++;
		    break;
	    }
	}
	if ( nrel != 0 )       relate_cent /= (double)nrel;
	if ( nnotrel != 0 )    notrel_cent /= (double)nnotrel;
/*
*    IF THIS IS THE FIRST POINT -> PUT TO THE MIDDLE
*/
	if (nrel == 0 && nnotrel == 0) candidate = center;
	else
/*
*    IF NO NOT_RELATED NODE -> PUT TO THE CENTRE OF GRAVITY OF RELATED NODES
*/
	if ( nnotrel == 0 ) candidate = relate_cent;
	else
/*
*    IF NO RELATED NODE -> PUT TO THE MIRROR OF THE nrel_cent ON THE CENTRE
*/
	if ( nrel == 0 )  candidate = 2.0 * center - notrel_cent;
	else
/*
*    BOTH TYPE OF NODES EXIST ->
*    CALCULATE THE CANDIDATE POINT AS THE HALF MIRROR OF notrel_cent TO relate_cent
*/
	    candidate = 2.0 * relate_cent - 1.0 * notrel_cent;
/*
*    PERTURBATE RANDOMLY
*/
	candidate += vector( perturb_x / (double)(nfixnode + inode + 5) *
			     (double)( rand() - RAND_MAX / 2),
			     perturb_y / (double)(nfixnode + inode + 5) *
			     (double)( rand() - RAND_MAX / 2 ));
/*
*    DECIDE IF IT IS OUTSIDE -> FIND THE NEAREST INSIDE POINT
*/
	if ( candidate.X() < WALL_MARGIN)
	    candidate = vector( 2.0 * WALL_MARGIN, candidate.Y() );
	if ( candidate.X() > OVERWINDOW_X - WALL_MARGIN)
	    candidate = vector(OVERWINDOW_X - 2.0 * WALL_MARGIN, candidate.Y());

	if ( candidate.Y() < WALL_MARGIN)
	    candidate = vector( candidate.X(), 2.0 * WALL_MARGIN );
	if ( candidate.Y() > OVERWINDOW_Y - WALL_MARGIN)
	    candidate = vector(candidate.X(), OVERWINDOW_Y - 2.0 * WALL_MARGIN );

/*
*    SET POSITION OF THE NEW NODE
*/
	relatenode -> Position( ) = candidate;
/*
*    ARRANGE ALREADY DEFINED NODES BY DYNAMIC LAYOUT -> IGNORE EDGE CONSTRAINTS
*/
	NodeElem * oldcurrnode = currnode;
	char ret = DynamicLayout( inode );
	currnode = oldcurrnode;

	if ( ret != STOPPED || !NextNode() ) return ret;
    }
}
