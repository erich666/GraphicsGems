/******************************************************************
exthit.h

This is the header file for a C++ class for fast n-dimensional extent
overlap checking. An instance of the class is initialized with a
call to the constructor.

  ExtHit(size) - where size is the maximum number of extents that
  	can be held (and hence tested against each other) in the ExtHit
	class instance.

There are three other methods in the ExtHit class:

  ~ExtHit () - the ExtHit class destructor frees the memory allocated for
     the class instance's internal tables, and destroys the class instance.

  BOOL add( Extent &extent, Ptr obj ) - This method adds an extent to
    the ExtHit class instance. The extent argument is a structure containing
	the values for the extent, and the obj argument is a pointer to the
	the object for the new extent.

  void test (void (*func)(Ptr d, Ptr e1, Ptr e2), Ptr data) - This method
    peforms the actual extent overlap checking. The func argument is a
	pointer to a function to be called for each pair of objects whose
	extents overlap. The data argument allows user specified data to be
	passed to func. Func is called with three arguments; the user supplied
	data d, and pointers to the overlapping objects (the pointer passed
	as the obj argument in the add method).
******************************************************************/

#ifndef _EXTHIT_H
#define _EXTHIT_H

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

typedef void* Ptr;
typedef short BOOL;

// The number of dimensions to be used in extent checking 
// This constant should be changed to reflect the number
// of dimensions being checked.
const int num_dimensions=4;

typedef struct tagExtent {
  int min [num_dimensions];
  int max [num_dimensions];
} Extent;

typedef struct tagCollideRecord CollideRecord; 

typedef struct tagMinMaxRecord
{
 CollideRecord *ptr;		// The parent CollideRecord for this MMR
 int value;					// The extent dimension value for this MMR
} MinMaxRecord;

typedef MinMaxRecord *MinMaxRecordPtr;

typedef struct tagCollideRecord
{
  int index;				// The index number of the extent represented 
  Ptr obj;					// A pointer to the extent's parent geometry
  CollideRecord *nextOpen;		// The next element on the open list
  CollideRecord *prevOpen;		// The previous element on the open list
  CollideRecord *nextActive;	// The next element on the active list
  CollideRecord *prevActive;	// The previous element on the active list
  MinMaxRecord min [num_dimensions];	// MMR's for the minimum extent values
  MinMaxRecord max [num_dimensions];	// MMR's for the maximum extent values
  BOOL active;			// Flag specifying CR overlapped with another CR
  BOOL open;			// Flag specifying CR is currently on the open list
} CollideRecord;

/******************************************************************
 Definition for the ExtHit class
 ******************************************************************/
class ExtHit
{
  public:
	ExtHit (int size);						// Class constructor
	~ExtHit ();								// Class destructor
	BOOL add( Extent &extent, Ptr obj );	// Adds an extent to the collideList
	void test (void (*func)(Ptr d, Ptr e1, Ptr e2), Ptr data);
											// Perform extent overlap testing

  private:
	int	maxSize;				// Maximum number of extents that can be held
	int	numColRecs;				// Number of CollideRecords in the CollideList
	int	numOverlaps;			// Number of MMRs in the overlapList
	MinMaxRecord **overlapList;	// Extent values for a single dimension
	BOOL *overlapTable;			// Table specifying which extents overlapped
	CollideRecord *collideList;	// Array holding the extent information
	CollideRecord *activeList;	// The head of the list of active extents

	void subtest ( int dim, int oLstSize, void (*func)(Ptr d, Ptr p1, Ptr p2),
					Ptr data);
};

int minMaxCompare( const void* rec1, const void* rec2);

#endif

/******************************************************************
 End of exthit.h
 ******************************************************************/
