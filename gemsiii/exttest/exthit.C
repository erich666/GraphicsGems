/******************************************************************
exthit.C

This code implements a C++ class for fast n-dimensional extent
overlap checking. An instance of the class is initialized with a
call to the constructor. For details on the usage of the ExtHit
class see the header for the ExtHit header file

Authors: Len Wanger and Mike Fusco
		 SimGraphics Engineering
******************************************************************/

#ifndef _EXTHIT_H
#include "exthit.h"
#endif

#include <stdlib.h>

static int dim;

/******************************************************************
  ExtHit - Class constructor

  Inputs:
    size:	Maximum number of extents that can be held by this instance.

  Outputs:
    None.
******************************************************************/
ExtHit::ExtHit (int size)
{
  numColRecs = 0;
  maxSize = size;
  activeList = NULL;

  // Allocate internal tables
  collideList  = new CollideRecord[size];
  overlapList  = new MinMaxRecordPtr[2*size];
  overlapTable = (BOOL *) calloc ( (size*size), sizeof(BOOL) );
}

/******************************************************************
  ~ExtHit - Class destructor

  Inputs:
    None.

  Outputs:
    None.
******************************************************************/
ExtHit::~ExtHit ()
{
  // Reclaim the memory allocated for the internal tables 
  delete ( overlapList );
  delete ( overlapTable );
  delete ( collideList );
}

/******************************************************************
  add - Add an extent to be considered for overlap checking.

  Inputs:
    extent: The extent to consider for extent overlap checking.
	obj:	A pointer to the parent object for the extent.

  Outputs:
    A Boolean value: TRUE if the information was added successfully,
					 FALSE otherwise.
******************************************************************/
BOOL ExtHit::add( Extent &extent, Ptr obj )
{
  CollideRecord *cr = &collideList[numColRecs];

  // Make sure there is room to add the extent
  if ( numColRecs >= maxSize )
    return (FALSE);

  // Add the new CollideRecord to the activeList
  if (numColRecs == 0)
    {
      cr->prevActive = NULL;
	  activeList = cr;
	}
  else
    {
      cr->prevActive = &collideList[numColRecs-1];
      cr->prevActive->nextActive = cr;
	}

  // Initialize the new CollideRecord
  cr->nextActive = NULL;
  cr->index = numColRecs++;
  cr->obj = obj;
  cr->prevOpen = cr->nextOpen = NULL;
  cr->active = TRUE;
  cr->open = FALSE;

  // add the extent values to the CollideRecord
  for ( int i=0; i<num_dimensions; i++ )
    {
	  cr->min[i].value = extent.min[i];
	  cr->max[i].value = extent.max[i];
	  cr->min[i].ptr = cr->max[i].ptr = cr;
	}

  return (TRUE);
}

/******************************************************************
  test - Test for overlapping extents.

  Inputs:
    func: A user supplied routine to be called for all pairs of
		overlapping extents.
	data: User supplied data to be passed as the d argument in user
		supplied routine func.

  Outputs:
    None.
******************************************************************/
void ExtHit::test (void (*func)(Ptr d, Ptr obj1, Ptr obj2), Ptr data)
{
  for ( dim=0; dim<num_dimensions; dim++ )
    {
	  // Add the min and max values of each active extent to the overlapList
	  numOverlaps = 0;
	  CollideRecord *activeCr = activeList;

	  while ( activeCr )
	    {
		  // Add the min extent value to the overlapList
		  MinMaxRecord *mmr = &(activeCr->min[dim]);
		  overlapList[numOverlaps++] = mmr;

		  // Add the max extent value to the overlapList
		  mmr = &(activeCr->max[dim]);
		  overlapList[numOverlaps++] = mmr;

		  // Reset the active and open flags
		  mmr->ptr->active = mmr->ptr->open = FALSE;

		  // Mark extent as currently not open
		  mmr->ptr->prevOpen = mmr->ptr->nextOpen = NULL;

		  activeCr = activeCr->nextActive;
		}

	  // Sort the OverlapList by MinMaxRecord value
	  qsort( overlapList, numOverlaps, sizeof(MinMaxRecordPtr), minMaxCompare );

	  // Process all overlapping pairs in the overlapList
	  subtest ( dim, numOverlaps, func, data );
	}
}

/******************************************************************
  minMaxCompare - Compare the value of two MinMaxRecords for sorting.

  Inputs:
    rec1: A MinMaxRecord to compare.
    rec2: A MinMaxRecord to compare.

  Outputs:
    -1: if rec1's MMR value is less than rec2's MMR value.
	 0:	if the two MMR values are equal
	 1: if rec1's MMR value is greater than rec2's MMR value.
******************************************************************/
int minMaxCompare( const void* rec1, const void* rec2)
{
  MinMaxRecord *mmr1 = *(MinMaxRecord **) rec1;
  MinMaxRecord *mmr2 = *(MinMaxRecord **) rec2;

  if (  mmr1->value > mmr2->value )
    return (1);
  else if (  mmr1->value < mmr2->value )
    return (-1);
  else // mmr1->value == mmr2->value 
	{
	  // Put minimum extent values before maximum extent value to
	  if ( mmr1->value == mmr1->ptr->min[dim].value )
	    return (-1);
	  else
	    return (1);
	}
}

/******************************************************************
  subtest - Find and process each set of overlapping pairs of extents
  	for a dimension (i.e. in the overlapList).

  Inputs:
    dim: 		The extent dimension being tested.
    oLstSize: 	The number of MinMaxRecords in the overlapList.
    func: 		User supplied function to be called for each overlapping pair.
    data: 		User supplied data to be passed in the call to func.

  Outputs:
    None.
******************************************************************/
void ExtHit::subtest ( int dim, int oLstSize, void (*func)(Ptr d, Ptr p1,
						 Ptr p2), Ptr data )
{
  // Reset the list of currently open CollideRecords.
  CollideRecord *openList = NULL;

  for (int i=0; i<oLstSize; i++)
    {
	  // Get CollideRecord for the overlapList entry
	  CollideRecord *rec = overlapList[i]->ptr;

	  if ( !rec->open )
		{
		  // Set rec's open flag
		  rec->open = TRUE;

		  // Set the overlapTable entry for all objects on the openList
		  CollideRecord *cr = openList;

		  while ( cr )
			{
			  int min = MIN(cr->index, rec->index);
			  int max = MAX(cr->index, rec->index);
			  int index = (min * maxSize) + max;

			  // check whether the entry for cr overlapping rec is set in
			  //   the overlapTable.
			  if ( overlapTable[index] == dim )
				{
				  if ( dim < (num_dimensions-1) )
					{
					  // Set the entry in the top half of the overlapTable
					  overlapTable[index] = dim+1;
					  cr->active = rec->active = TRUE;
					}
				  else
				    // The extents overlapped in all of the dimensions, call
					// the user supplied overlap function.
					(*func)(data, cr->obj, rec->obj );
				}

			  cr = cr->nextOpen;
			}

		  // Add rec at the head of the openList
		  rec->nextOpen = openList;
		  rec->prevOpen = NULL;

		  if ( openList )
			openList->prevOpen = rec;

		  // Update the head of the openList
		  openList = rec;
		}
	  else // the record is already on the openList
	    {
		  // remove the record from the openList
		  if ( rec->prevOpen ) 
			rec->prevOpen->nextOpen = rec->nextOpen; 
		  else 
			openList = rec->nextOpen; 

		  if ( rec->nextOpen ) 
			rec->nextOpen->prevOpen = rec->prevOpen;  

		  rec->open = FALSE;

		  // If the record is not active remove it from the activeList
		  if ( ! rec->active ) 
		    {
			  if ( rec->prevActive == NULL )
			    {
			      activeList = rec->nextActive;

				  if (rec->nextActive)
			        rec->nextActive->prevActive = NULL;
				}
			  else
			    {
			      rec->prevActive->nextActive = rec->nextActive;

				  if (rec->nextActive)
					rec->nextActive->prevActive = rec->prevActive;
				}
			}
		}
	}
}

//-------------------------------------------------------------
// End of exthit.C
//-------------------------------------------------------------
