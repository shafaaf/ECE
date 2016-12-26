/*
 * This implementation replicates the implicit list implementation
 * provided in the textbook
 * "Computer Systems - A Programmer's Perspective"
 * Blocks are never coalesced or reused.
 * Realloc is implemented directly using mm_malloc and mm_free.
 *
 * Description: This lab assignment implements a dynamic storage allocator
	for C programs. To be specific the functionalities of C's malloc, free and realloc
	functions are implemented.
	An explicit free list is used to keep track of free blocks of varying size with a
	minimum size of 32 Bytes. Free blocks are inserted into the start of the list and 
	first fit policy is used when finding free blocks of the needed size. If a block of
	appropriate size can not be found, then extend_heap is called which internally calls
	sbrk to extend the size of the heap. This program is more memory efficient and 
	has higher throughout as compared to the implicit free list. The pointers to point to 
	next, previous free blocks are stored in the unused payload area of every free block.	
	What allocated block looks like:
		-Header(1 word), payload, footer(1 word)
	What free block looks like:
		-Header(1 word), previous pointer (1 word), next pointer(1 word), Unused payload, footer(1 word)
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "0xdeadbeef",
    /* First member's full name */
    "Shafaaf Hossain",
    /* First member's email address */
    "shafaaf.hossain@mail.utoronto.ca",
    /* Second member's full name (leave blank if none) */
    "Sai Kiran Varikooty",
    /* Second member's email address (leave blank if none) */
    "s.varikooty@mail.utoronto.ca"
};

/*************************************************************************
 * Basic Constants and Macros
 * You are not required to use these macros but may find them helpful.
*************************************************************************/
#define WSIZE       sizeof(void *)            /* word size (bytes) */
#define DSIZE       (2 * WSIZE)            /* doubleword size (bytes) */
#define CHUNKSIZE   (1<<7)      /* initial heap size (bytes) */

#define MAX(x,y) ((x) > (y)?(x) :(y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)          (*(uintptr_t *)(p))
#define PUT(p,val)      (*(uintptr_t *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)     (GET(p) & ~(DSIZE - 1))
#define GET_ALLOC(p)    (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)        ((char *)(bp) - WSIZE)
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

// My Macros to access next and previous pointers of explicit list
#define GET_PREV(bp) (char *)(bp)
#define GET_NEXT(bp) ((char *)(bp + WSIZE))


void* heap_listp = NULL;

//Use extend heap for sizes less than this threshold 
#define EXTEND_HEAP_THRESHOLD 1000

//Data structure format to easily access prev, next in free blocks 
typedef struct freeListBlock{
	struct freeListBlock * prev;
	struct freeListBlock * next;
}freeListBlock;

//Pointer to start explicit list
freeListBlock* explicitListPointer;

/**********************************************
* For printing debugging information.
* Just comment, uncomment out the DEBUG flags
* to remove print statements
***********************************************/

//#define DEBUG
#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

//#define DEBUGV
#ifdef DEBUGV
# define DEBUG_PRINTV(x) printf x
#else
# define DEBUG_PRINTV(x) do {} while (0)
#endif

/**********************************************************
 * mm_init
 * Initialize the heap, including "allocation" of the
 * prologue and epilogue
 * Set initial explicit list pointer to NULL
 **********************************************************/
 int mm_init(void)
 {
   DEBUG_PRINTV(("mm_init: \n"));
   if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
         return -1;
     PUT(heap_listp, 0);                         // alignment padding
     PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));   // prologue header
     PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));   // prologue footer
     PUT(heap_listp + (3 * WSIZE), PACK(0, 1));    // epilogue header
     heap_listp += DSIZE;
	
     //Initially explicit pointer points to NULL
     explicitListPointer = NULL;
     return 0;
 }

/**********************************************************
 * Coalesce
 * Covers the 4 cases discussed in the text:
 * - both neighbours are allocated
 * - the next block is available for coalescing
 * - the previous block is available for coalescing
 * - both neighbours are available for coalescing
 * - when coalescing, remove coalesced block from explicit list
 **********************************************************/
void *coalesce(void *bp)
{
    DEBUG_PRINTV(("coalesce: \n"));
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

     /* Case 1 - No coalesce*/
    if (prev_alloc && next_alloc) {      
        return bp;
    }

    /* Case 2 - Coalesce with next block*/
    else if (prev_alloc && !next_alloc) { 
	removeExplicitList(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        return (bp);
    }

    /* Case 3 - Coalesce with previous block*/
    else if (!prev_alloc && next_alloc) {
	removeExplicitList(PREV_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        return (PREV_BLKP(bp));
    }

    /* Case 4 - Coalesce with previous and next block*/
    else {            
	removeExplicitList(PREV_BLKP(bp));
	removeExplicitList(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)))  +
            GET_SIZE(FTRP(NEXT_BLKP(bp)))  ;
        PUT(HDRP(PREV_BLKP(bp)), PACK(size,0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size,0));
        return (PREV_BLKP(bp));
    }
}

/**********************************************************
 * extend_heap
 * Extend the heap by "words" words, maintaining alignment
 * requirements of course. Free the former epilogue block
 * and reallocate its new header
 **********************************************************/
void *extend_heap(size_t words)
{
    DEBUG_PRINTV(("extend_heap: \n"));
    char *bp;
    size_t size;    

    /* Allocate an even number of words to maintain alignments */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ( (bp = mem_sbrk(size)) == (void *)-1 )
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));                // free block header
    PUT(FTRP(bp), PACK(size, 0));                // free block footer
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));        // new epilogue header

    return bp;
}


/**********************************************************
*  NOT BEING USED
 * find_fit - used for previous free implicit list. 
 * Traverse the heap searching for a block to fit asize
 * Return NULL if no free blocks can handle that size
 * Assumed that asize is aligned
 **********************************************************/
void * find_fit(size_t asize)
{
    void *bp;
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
    {
        if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp))))
        {
            return bp;
        }
    }
    return NULL;
}


/**********************************************************
 * findExplicit - Find a free block
 * Traverse the explicit list to find a free block
 **********************************************************/
void * findExplicit(size_t asize)
{
    //Freelist empty so nothing to remove
    if (explicitListPointer == NULL) 
    {
	return NULL;
    }

    DEBUG_PRINTV(("findExplicit: \n"));
    //traverse freelist
    freeListBlock *iterator = explicitListPointer;
    while(iterator!=NULL)
	{
		if ((asize <= GET_SIZE(HDRP(iterator))))
	        {
		    removeExplicitList(iterator);
	            return iterator;
        	}
		iterator = iterator->next;
	}
    return NULL;
}

/**********************************************************
 * addExplicitList
 * Add to explicit list. Use LIFO approach
 * 
 **********************************************************/
void * addExplicitList(void* bp)
{

    DEBUG_PRINTV(("addExplicitList (%ld)\n", bp));
    freeListBlock* newBlock = (freeListBlock*) bp;

    //If head of explicit list is NULL.
    if(explicitListPointer == NULL)
    {
	explicitListPointer = newBlock;
	newBlock->prev = NULL;
	newBlock->next = NULL;
    }

    //If head of explicit list not NULL, so add to front - LIFO
    else
    {
	newBlock->next = explicitListPointer;
	newBlock->prev = NULL;
	explicitListPointer->prev = newBlock;
	newBlock->prev = NULL;
	explicitListPointer = newBlock;
    }
	return;
}

/**********************************************************
 * removeExplicitList
 * Remove the bp from the explicit list
 **********************************************************/

void removeExplicitList(void* bp)
{

	//Casting block to remove
	freeListBlock* blockToRemove = (freeListBlock*) bp;
		
	//When list is empty or block to remove is NULL
	if((explicitListPointer == NULL) || (blockToRemove == NULL))
	{
    		return;
	}
	
	//Block to remove is at start of list	
	if (blockToRemove->prev == NULL)
	{
		//Block to remove is only one in the list
		if (blockToRemove->next == NULL)
		{
			explicitListPointer = NULL;
			return;	
		}
		
		//Block to remove is at head, but has other blocks
		else
		{
			blockToRemove->next->prev = NULL;
			explicitListPointer = blockToRemove->next;	
			return;
		}
	}	

	//Block to remove is not at start of list but at end
	if (blockToRemove->next == NULL) 
	{
		blockToRemove->prev->next = NULL;
		return;
	}

	//When block is in the midle of list
	blockToRemove->prev->next = blockToRemove->next;		
	blockToRemove->next->prev = blockToRemove->prev;
  	return;
}

/**********************************************************
 * place
 * Mark the block as allocated
 **********************************************************/
void place(void* bp, size_t asize)
{
  DEBUG_PRINTV(("place: \n"));
  /* Get the current block size */
  size_t bsize = GET_SIZE(HDRP(bp));

  PUT(HDRP(bp), PACK(bsize, 1));
  PUT(FTRP(bp), PACK(bsize, 1));
}


/**********************************************************
 * mm_free
 * Free the block
 **********************************************************/
void mm_free(void *bp)
{
    DEBUG_PRINTV(("mm_free: \n"));
    if(bp == NULL){
      return;
    }
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size,0));
    PUT(FTRP(bp), PACK(size,0));
    addExplicitList(bp);
    //mm_check
}

/**********************************************************
 * mm_malloc
 * Allocate a block of size bytes.
 * The type of search is determined by find_fit
 * The decision of splitting the block, or not is determined
 *   in place(..)
 * If no block satisfies the request, the heap is extended
 **********************************************************/

void *mm_malloc(size_t size)
{
    DEBUG_PRINTV(("mm_malloc: \n"));
    size_t asize; /* adjusted block size */
    size_t extendsize; /* amount to extend heap if no fit */
    char * bp;

    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1))/ DSIZE);


    /* Search the free list for a fit if greater than threshold*/
    if (asize > EXTEND_HEAP_THRESHOLD) 
    {
    	if ((bp = findExplicit(asize)) != NULL) 
    	{
        	place(bp, asize);
        	return bp;
    	}
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    place(bp, asize);

    //mm_check
    return bp;

}

/**********************************************************
 * mm_realloc
 * Implemented simply in terms of mm_malloc and mm_free
 *********************************************************/
void *mm_realloc(void *ptr, size_t size)
{
    DEBUG_PRINTV(("mm_realloc: \n"));
    /* If size == 0 then this is just free, and we return NULL. */
    if(size == 0){
      mm_free(ptr);
      return NULL;
    }
    /* If oldptr is NULL, then this is just malloc. */
    if (ptr == NULL)
      return (mm_malloc(size));

    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;

    /* Copy the old data. */
    copySize = GET_SIZE(HDRP(oldptr));
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

/**********************************************************
 * mm_check
 * Check the consistency of the memory heap
 * Return nonzero if the heap is consistant.
 *********************************************************/
int mm_check(void)
{
    
    void *iterator = heap_listp;
    
    // For printing out list of values
    for (iterator = heap_listp; GET_SIZE(HDRP(iterator)) > 0; iterator = NEXT_BLKP(iterator))
    {
	//printf("\nBlock:\t\t%p\tValue:\t\t%lu\t\tAlloc:%lu\n",  (char *) iterator, GET(iterator), GET_ALLOC(HDRP(iterator)));
    }
    
    // Scan free list for allocation bit
    freeListBlock* freeTraverse = explicitListPointer;
    while (freeTraverse != NULL) 
    {
	if (GET_ALLOC(HDRP(freeTraverse)) != 0) 
	{
		return -1; // Free block not marked as free, return -1
	}
	freeTraverse = freeTraverse->next;
    }
	
    // Check if continguous blocks not coalesced
    freeListBlock* contiguousTraverse = explicitListPointer;
    while (contiguousTraverse != NULL) 
    {
	if (contiguousTraverse->next != NULL) 
	{
		freeListBlock *nextBlock = contiguousTraverse;
		if ((GET_ALLOC(HDRP(contiguousTraverse)) == 0) && (GET_ALLOC(HDRP(nextBlock)) == 0)) 
		{
			return -2; // Contiguous blocks not coalesced, return -2
		}
	}
	contiguousTraverse = contiguousTraverse->next;
    }
    // Check if all free blocks are on free list
    for (iterator = heap_listp; GET_SIZE(HDRP(iterator)) > 0; iterator = NEXT_BLKP(iterator))
    {
	if(GET_ALLOC(HDRP(iterator)) == 0)
	{
		if (findInFreeList(iterator) == -1) 
		{
			return -3; // Block is free but not in free List
		}
	} 
    }
    // Check if pointers in free list point to valid free blocks
    freeListBlock* traversal = explicitListPointer;
    while (traversal != NULL) 
    {
	if ( GET_ALLOC(HDRP(traversal)) != 0) 
	{
		return -4; // Does not point to a valid free block
	}
	traversal = traversal->next;
    } 
    
  return 1;
}

/**************************************************
 * findInFreeList:
 * Check if the given block pointer exists in the 
 * free list. If block exists in free list,
 * return 1. Else return -1.
 **************************************************/

int findInFreeList(void* bp) {
	freeListBlock* iterator = explicitListPointer;
	while (iterator != NULL) {
		if (iterator == (freeListBlock *) bp) {
			return 1;
		}
		iterator = iterator->next;
	}
	return -1;
}
/***************************************************/







