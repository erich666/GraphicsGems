/* alloc.c
 *
 * A simple fast memory allocation package.
 *
 * AllocInit()    - create an alloc pool, returns the old pool handle.
 * Alloc()        - allocate memory.
 * AllocReset()   - reset the current pool.
 * AllocSetPool() - set the current pool.
 * AllocFree()    - free the memory used by the current pool.
 *
 */

#include <stdio.h>

#include "alloc.h"

/* ALLOC_BLOCK_SIZE - adjust this size to suit your installation - it should
 * be reasonably * large otherwise you will be mallocing a lot.
 */

#define ALLOC_BLOCK_SIZE        (100*1024)

/* alloc_hdr_t - Header for each block of memory
 */

typedef
struct alloc_hdr_s
{
        struct alloc_hdr_s *next;   /* Next Block          */
        char               *block,  /* Start of block      */
                           *free,   /* Next free in block  */
                           *end;    /* block + block size  */
}
alloc_hdr_t;

/* alloc_root_t - Header for the whole pool
 */

typedef
struct alloc_root_s
{
        alloc_hdr_t *first,   /* First header in pool */
                    *current; /* Current header       */
}
alloc_root_t;

/* root - Pointer to the the current pool
 */

static alloc_root_t *root;

/* AllocHdr()
 *
 * Private routine to allocate a header and memory block.
 */

static
alloc_hdr_t *
AllocHdr()
{
        alloc_hdr_t        *hdr;
        char               *block;

        block = (char *) malloc(ALLOC_BLOCK_SIZE);
        hdr   = (alloc_hdr_t *) malloc(sizeof(alloc_hdr_t));

        if (hdr == NULL || block == NULL)
        {
                fprintf(stderr, "Out of memory\n");
                exit(1);
        }
        hdr->block = block;
        hdr->free  = block;
        hdr->next  = NULL;
        hdr->end   = block + ALLOC_BLOCK_SIZE;

        return(hdr);
}

/* AllocInit()
 *
 * Create a new memory pool with one block.
 * Returns pointer to the previous pool.
 */

alloc_handle_t *
AllocInit()
{
        alloc_handle_t        *old = (alloc_handle_t *) root;

        root = (alloc_root_t *) malloc(sizeof(alloc_root_t));
        root->first = AllocHdr();
        root->current  = root->first;
        return(old);
}

/* Alloc()
 *
 * Use as a direct replacement for malloc().  Allocates memory
 * from the current pool.
 */

char *
Alloc(size)
int        size;
{
        alloc_hdr_t        *hdr = root->current;
        char               *ptr;

        /* Align to 4 byte boundary - should be OK for most machines.
         * Change this if your machine has wierd alignment requirements
         */
        size = (size + 3) & 0xfffffffc;

        ptr = hdr->free;
        hdr->free += size;

        /* Check if the current block is exhausted */

        if (hdr->free >= hdr->end)
        {
                /* Is the next block already allocated? */

                if (hdr->next != NULL)
                {
                        /* re-use block */
                        hdr->next->free = hdr->next->block;
                        root->current = hdr->next;
                }
                else
                {
                        /* extend the pool with a new block */
                        hdr->next = AllocHdr();
                        root->current = hdr->next;
                }

                /* set ptr to the first location in the next block */
                ptr = root->current->free;
                root->current->free += size;
        }
        /* Return pointer to allocated memory */
        return(ptr);
}

/* AllocSetPool()
 *
 * Change the current pool.  Return the old pool.
 */

alloc_handle_t *
AllocSetPool(new)
alloc_handle_t        *new;
{
        alloc_handle_t *old = (alloc_handle_t *) root;

        root = (alloc_root_t *) new;
        return(old);
}

/* AllocReset()
 *
 * Reset the current pool for re-use.  No memory is freed, so
 * this is very fast.
 */

void
AllocReset()
{
        root->current = root->first;
        root->current->free = root->current->block;
}

/* AllocFreePool()
 *
 * Free the memory used by the current pool.
 * Don't use where AllocReset() could be used.
 */

void
AllocFreePool()
{
        alloc_hdr_t        *hdr = root->first;

        while (hdr != NULL)
        {
                free((char *) hdr->block);
                free((char *) hdr);
                hdr = hdr->next;
        }
        free((char *) root);
        root = NULL;
}
