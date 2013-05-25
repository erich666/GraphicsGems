/* bspMemory.c: module to allocate and free memory and also to count them.
 * Copyright (c) Norman Chin 
 */
#include "bsp.h"		
#include <malloc.h>

static long memoryCount= 0L;

/* Allocates memory of num bytes */
char *MYMALLOC(unsigned num)
{
   char *memory= malloc(num);	/* checked for null by caller */

   ++memoryCount;		/* increment memory counter for debugging */
   return(memory);
} /* myMalloc() */

/* Frees memory pointed to by ptr */
void MYFREE(char *ptr) 
{
   --memoryCount;		/* decrement memory counter for debugging */
   free(ptr);
} /* myFree() */

/* Returns how many memory blocks are still allocated up to this point */
long MYMEMORYCOUNT(void)
{
   return(memoryCount);
} /* myMemoryCount() */

/*** bspMemory.c ***/

