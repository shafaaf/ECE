/*
 * This implementation replicates the implicit list implementation
 * provided in the textbook
 * "Computer Systems - A Programmer's Perspective"
 * Blocks are never coalesced or reused.
 * Realloc is implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "mm.h"
#include "memlib.h"

//--------------------------------------------------------------------------------------------------------------------------------------------

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "0Xdeadbeef",
    /* First member's full name */
    "Shafaaf Khaled Hossain",
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

#define WSIZE       sizeof(void *)            /* word size (bytes) */	//Here 8
#define DSIZE       (2 * WSIZE)            /* doubleword size (bytes) */	//Here 16

//Todo: play around with this to get a good performance
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

/**********************************************
* For debugging.
* Just comment, uncomment out the DEBUG flags
* to remove print statements
***********************************************/
//#define DEBUG
#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

#define DEBUGV
#ifdef DEBUGV
# define DEBUG_PRINTV(x) printf x
#else
# define DEBUG_PRINTV(x) do {} while (0)
#endif

// Newly addedd macros: FIX
#define GET_NEXT(bp)    (char *)(bp)
#define GET_PREV(bp)    ((char *)(bp) + WSIZE)

#define LIST_SIZE 8


//This eventually points to between the prologue block
void* heap_listp = NULL;


//Segregated list data structure format to access prev, next in blocks
typedef struct segregatedArrayList{
	struct segregatedArrayList * prev;
	struct segregatedArrayList * next;
}segregatedArrayList;


//Array of explicit lists to make segregated lists to store blocks of different ranges
segregatedArrayList* mySegregatedArrayList[LIST_SIZE];

/**********************************************************
 * segregatedArrayListInit()
 * Initialize the segregatedArrayListInit will null
 **********************************************************/
 void segregatedArrayListInit(void){
	    DEBUG_PRINTV(("segregatedArrayListInit: Initializing my segregatedArrayList with NULL\n"));
	    int i;
 	    for(i = 0;i< LIST_SIZE; i++)
	    {
		mySegregatedArrayList[i] = NULL;
	    }
}

/**********************************************************
 * checkSegregatedFreeLists
 * Helper function to print all free blocks in the array of segragated free lists
 **********************************************************/
  void checkSegregatedFreeLists(void){
	DEBUG_PRINTV(("checkSegregatedFreeLists: Called checkSegregatedFreeLists()\n"));
	int i;
	segregatedArrayList* traversePointer;
	for(i=0;i<LIST_SIZE; i++)
	{
		DEBUG_PRINTV(("checkSegregatedFreeLists: Checking mySegregatedArrayList[%d] now:\n", i));
		traversePointer = mySegregatedArrayList[i];
		while(traversePointer != NULL)
		{
			DEBUG_PRINTV(("checkSegregatedFreeLists: At bp address %p, block present. Actual size from header is %lu bytes\n", traversePointer, GET_SIZE(HDRP((void*)traversePointer))));
			DEBUG_PRINTV(("checkSegregatedFreeLists: Next of bp address %p is %p.\n",traversePointer, traversePointer->next));
			traversePointer = traversePointer->next; 
		}
	}
	return;
	
}

/**********************************************************
 * mm_init
 * Initialize the heap, including "allocation" of the
 * prologue and epilogue
 **********************************************************/
 int mm_init(void)
 {
   DEBUG_PRINTV(("_______________________________________________________\n"));
   DEBUG_PRINTV(("mm_init: Called mm_init()\n"));
   if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
         return -1;
     PUT(heap_listp, 0);                         // alignment padding
     PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));   // prologue header
     PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));   // prologue footer
     PUT(heap_listp + (3 * WSIZE), PACK(0, 1));    // epilogue header
     heap_listp += DSIZE;

     DEBUG_PRINTV(("mm_init: heap_listp is at address %p\n", heap_listp));

     //Initialize segregated list
     segregatedArrayListInit();

     DEBUG_PRINTV(("_______________________________________________________\n"));
    
     return 0;
 }

//--------------------------------------------------------------------------------------------------------------------------------------------

/**********************************************************
 * coalesce
 * Covers the 4 cases discussed in the text:
 * - both neighbours are allocated
 * - the next block is available for coalescing
 * - the previous block is available for coalescing
 * - both neighbours are available for coalescing
 **********************************************************/


//Todo: Just remove those coalised blocks from the free list
void *coalesce(void *bp)
{
    
    DEBUG_PRINTV(("coalesce: Now coalescing for bp address 0x%p\n",bp));
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {       /* Case 1 */
	    DEBUG_PRINTV(("coalesce: Case 1 for bp address 0x%p\n",bp));
        return bp;
    }

    //Next block free. 
    //todo: So need to remove from free list
    else if (prev_alloc && !next_alloc) { /* Case 2 */
	DEBUG_PRINTV(("coalesce: Case 2 for bp address 0x%p\n",bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
	//remove from free list
	removeFromFreeList(NEXT_BLKP(bp)); // remove next block from free list
        return (bp);
    }
    //Previous block free
    //todo: So need to remove from free list
    else if (!prev_alloc && next_alloc) { /* Case 3 */
	DEBUG_PRINTV(("coalesce: Case 3 for bp address 0x%p\n",bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
	//remove from free list
	removeFromFreeList(PREV_BLKP(bp)); // remove previous block from free list
        return (PREV_BLKP(bp));
    }

    //Both blocks free
    //todo: Remove both from free list
    else {            /* Case 4 */
	DEBUG_PRINTV(("coalesce: Case 4 for bp address 0x%p\n",bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)))  +
            GET_SIZE(FTRP(NEXT_BLKP(bp)))  ;
        PUT(HDRP(PREV_BLKP(bp)), PACK(size,0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size,0));
	//remove from free list
	removeFromFreeList(PREV_BLKP(bp)); // remove previous block from free list
	removeFromFreeList(NEXT_BLKP(bp)); // remove next block from free list
        return (PREV_BLKP(bp));
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------

/**********************************************************
 * extend_heap
 * Extend the heap by "words" words, maintaining alignment
 * requirements of course. Free the former epilogue block
 * and reallocate its new header
 **********************************************************/

//Todo: check this out - coaseslce called at end
void *extend_heap(size_t words)
{
    DEBUG_PRINTV(("extend_heap: In here and size is: %lu words. Will call coalesce.\n",words));

    char *bp;
    size_t size;

    //IMP = extend_heap already sets a header and footer for you.
    /* Allocate an even number of words to maintain alignments */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ( (bp = mem_sbrk(size)) == (void *)-1 )
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));                // free block header
    PUT(FTRP(bp), PACK(size, 0));                // free block footer
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));        // new epilogue header

    /* Coalesce if the previous block was free */
    //return coalesce(bp);	//was before
    coalesce(bp); //For now dont do this
    //addToFreeList(bp);
    return bp;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

/**********************************************************
 * find_fit for implcit free list already given to us
 * Traverse the heap searching for a block to fit asize
 * Return NULL if no free blocks can handle that size
 * Assumed that asize is aligned
 **********************************************************/

//Wont be used later. This was for implcit free list
void * find_fit(size_t asize)
{
    DEBUG_PRINTV(("find_fit: Will find a fit for size: %lu bytes\n",asize));
    void *bp;
    //Here heap_listp points to middle of prologue block of
    //total size of 16 bytes
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
    {
        if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp))))
        {
	    DEBUG_PRINTV(("find_fit: FOUND a fit. bp is %p, for requested size: %lu bytes\n", bp, asize));
            return bp;
        }
    }
    DEBUG_PRINTV(("find_fit: NO fit found.\n"));
    return NULL;
}


/**********************************************************
 * splitBlock
 * Splits remaining chunk of block.
	Todo: check this again
 *********************************************************/



/**********************************************************
 * Place
 * Only marks the block as allocated in header and footer
 **********************************************************/
void place(void* bp, size_t asize)
{   
	//old code given to us
	DEBUG_PRINTV(("place: for bp: %p, asize passed in is: %lu, and actual size in header is: %lu\n", bp, asize, GET_SIZE(HDRP(bp)))); 
	//Right now just sets headers and footers to allocated.
	size_t bsize = GET_SIZE(HDRP(bp));
	PUT(HDRP(bp), PACK(bsize, 1));
	PUT(FTRP(bp), PACK(bsize, 1));
}

/**********************************************************
 * Finds a location in the segregated list to put in our 
	free block based on size_t size
 **********************************************************/

int getSegregatedListIndex(size_t size)
{
    DEBUG_PRINT(("getSegregatedListIndex: Finding block for size : %lu\n", size));

    int index = 0;
    
    //If size too big, always go to last indexs
    if(size > 4096)
    {
	index = 7;
	return index;
    }
    unsigned int n = size;
    n--;
    n |= n >> 1;   
    n |= n >> 2; 
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    n++; 
    DEBUG_PRINT(("getSegregatedListIndex: n is: %d\n", n));
    
    DEBUG_PRINT(("getSegregatedListIndex: n>>1: %d\n", n>>1));
    
    while (n > 0) 
    { 
     	n = n >> 1;
	index++;
    }

	index -= 7;


    DEBUG_PRINT(("getSegregatedListIndex: Index: %d\n", index));
	    
    return index;
}

/**********************************************************
 * Remove a block from the Free List
 * Assume headers, footers, sizes are fixed already
 **********************************************************/
void removeFromFreeList(void *bp)
{
	size_t size = GET_SIZE(HDRP(bp));
	int index = getSegregatedListIndex(size);
	
	DEBUG_PRINTV(("removeFromFreeList: Removing block at bp: %p, with size of %lu, and at index: %d\n", bp, size, index));

	segregatedArrayList* blockToRemove =  (segregatedArrayList*) bp;
	
	//block to remove is at head of list of the index
	if(mySegregatedArrayList[index] == blockToRemove)
	{	
		DEBUG_PRINTV(("removeFromFreeList: Block to remove is at head of list of index: %d\n", index));
		//First block in list is the removing block and nothing else afterward
		if(blockToRemove->next == NULL)
		{	
			DEBUG_PRINTV(("removeFromFreeList: Block to remove has a next of NULL\n"));
			mySegregatedArrayList[index] = NULL;
			return;
		}
		else	//block to remove is head, but list has other things
		{	
			DEBUG_PRINTV(("removeFromFreeList: Block to remove has something else for next\n"));
			mySegregatedArrayList[index] = blockToRemove->next;
			blockToRemove->next->prev = NULL;
			return;		
		}
	}

	//Block to remove is not at head of list of the index
	DEBUG_PRINTV(("removeFromFreeList: Block to remove is NOT at head of list of index: %d\n", index));
	blockToRemove->prev->next = blockToRemove->next;
	
	//if not last
	if(blockToRemove->next != NULL)	
	{
		DEBUG_PRINTV(("removeFromFreeList:  Block to remove has something else for next\n"));
		blockToRemove->next->prev = blockToRemove->prev;
	}
	else
	{
		DEBUG_PRINTV(("removeFromFreeList:  Block to remove has a next of NULL.\n"));
	}
	return;
}

/**********************************************************
 * addToFreeList
 * Maintains an array of lists which have varying ranges of free block sizes
 * Add the block to correct free list in array of segregated lists
 * Assume headers, footers, sizes are fixed already
 * Use getSegregatedListIndex to find a spot and use LIFO policy
 **********************************************************/
//todo: Called after coaslesced. Fix that
void addToFreeList(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));
    DEBUG_PRINTV(("addToFreeList: Add to free list for bp address: 0x%p with size in bytes, in hex: 0x%x, in dec: %lu\n", bp, size, size));
    
    //Find a proper index
    int segregatedIndex = getSegregatedListIndex(size);
    DEBUG_PRINTV(("addToFreeList: getSegregatedListIndex returns index: %d for size: %lu\n", segregatedIndex, size));

    segregatedArrayList* listPointer = mySegregatedArrayList[segregatedIndex];
    segregatedArrayList* blockToAdd = (segregatedArrayList*) bp;
    
    //No block already present in that array index's list
    if(listPointer == NULL)
    {
	DEBUG_PRINTV(("addToFreeList: listPointer points to NULL.\n"));
	blockToAdd->prev = NULL;
	blockToAdd->next = NULL;
        mySegregatedArrayList[segregatedIndex] = blockToAdd;	
	DEBUG_PRINTV(("addToFreeList: Added in block at index: %d\n", segregatedIndex));
        
    }

    //Other blocks already present in that list
    else
    {
	DEBUG_PRINTV(("addToFreeList: listPointer not NULL.\n"));
	
	blockToAdd->next = listPointer;
	blockToAdd->prev = NULL;

	listPointer->prev = blockToAdd;
	mySegregatedArrayList[segregatedIndex] = blockToAdd;
	DEBUG_PRINTV(("addToFreeList: Added in block at index: %d\n", segregatedIndex));
    }

    //Prints out segregated list to debug.
    checkSegregatedFreeLists();
}

/**********************************************************
 * findFitFreeList
 * Find a spot in the free list
 * Removes whole block
 * Splits block if possible, and add in remaining chunk
 * Return the usalble block
 **********************************************************/
void* findFitFreeList(size_t asize)
{
    DEBUG_PRINTV(("findFitFreeList: Will find a fit for size: %lu bytes\n", asize));
    
    //Find index in segregated array list
    int index = getSegregatedListIndex(asize);
    DEBUG_PRINTV(("findFitFreeList: Got an index: %d\n",index));

    segregatedArrayList* listPointer = NULL;

    int i;
    for(i = index; i < LIST_SIZE; i++)
    {
	DEBUG_PRINTV(("findFitFreeList: Searching for block on mySegregatedArrayList[%d]\n", i));
	listPointer = mySegregatedArrayList[i];
	while (listPointer != NULL) {
		DEBUG_PRINTV(("findFitFreeList: Going through list of blocks on mySegregatedArrayList[%d]\n", i));
		//Can use this block
		if(GET_SIZE(HDRP(listPointer))	>= asize)
		{
			DEBUG_PRINTV(("findFitFreeList: Found free block size >= to asize. Requested asize: %lu bytes, found block's actual size from header is: %lu bytes\n",asize, GET_SIZE(HDRP(listPointer))));
			//Get size of returning block			
			size_t foundBlockSize = GET_SIZE(HDRP(listPointer));
			//Get size of partition block			
			size_t partitionBlockSize = foundBlockSize - asize;
			
			//Cant split this block			
			if(foundBlockSize - asize < 32)
			{	
				DEBUG_PRINTV(("findFitFreeList: CANT split. First remove whole block from list.\n"));
				removeFromFreeList(listPointer);
				DEBUG_PRINTV(("findFitFreeList: Removed. So return this free block's bp\n"));
				return listPointer;
			}
			//can split the block
			else
			{	
				//Remove whole block from list
				DEBUG_PRINTV(("findFitFreeList: CAN split. First remove whole block from list.\n"));
				removeFromFreeList(listPointer);
				
				//Actual splitting					
				//Work with returning block
				PUT(HDRP(listPointer), PACK(asize, 1));
				PUT(FTRP(listPointer), PACK(asize, 1));
				DEBUG_PRINTV(("findFitFreeList: After split, returning block has bp: %p, actual size: %lu byes\n", listPointer, GET_SIZE(HDRP(listPointer))));

				//Work with partitionBlock
				segregatedArrayList* partitionBlock= NEXT_BLKP(listPointer);
				PUT(HDRP(NEXT_BLKP(listPointer)), PACK(partitionBlockSize, 0));
				PUT(FTRP(NEXT_BLKP(listPointer)), PACK(partitionBlockSize, 0));
				DEBUG_PRINTV(("findFitFreeList: After split, partition block has bp: %p, actual size: %lu byes\n", partitionBlock, GET_SIZE(HDRP(partitionBlock))));
				//Add chunk back to list
				DEBUG_PRINTV(("findFitFreeList: Adding partitionBlock back to free list\n"));
				addToFreeList(partitionBlock);

				return listPointer;
			}
		}
			listPointer = listPointer->next;
			
	}
    }
    DEBUG_PRINTV(("findFitFreeList: Found no block on any list. Need to extend heap\n"));
    return NULL;
}


/**********************************************************
 * mm_free
 * Free the block by changing headers and footers.
 * Coalesce with neighbouring blocks. Also remove coalesced blocks from free list
 * Add the block to segregated free list
 **********************************************************/
void mm_free(void *bp)
{
    
    DEBUG_PRINTV(("_______________________________________________________\n"));
    DEBUG_PRINTV(("mm_free: Made a free request for bp address: 0x%p with size in bytes from header, in hex: 0x%x, in dec: %lu\n", bp, GET_SIZE(HDRP(bp)), GET_SIZE(HDRP(bp))));
    if(bp == NULL){
      return;
    }
    //show the block to be free
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size,0));
    PUT(FTRP(bp), PACK(size,0));
    coalesce(bp); //todo: put back in later and remove coalesced blocks from free list
    addToFreeList(bp); // todo: 
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
    size_t asize; /* adjusted block size */
    size_t extendsize; /* amount to extend heap if no fit */
    char * bp;
    
    DEBUG_PRINTV(("_______________________________________________________\n"));
    DEBUG_PRINTV(("mm_malloc: Requested allocation size of %lu bytes\n",size));

    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    // Minimum block size is 32 bytes i.e 4 words
    // User does not mention header, footer size so need to account for those with additinal 2 words. 
    // So align to 16 byte boundary and + 16 bytes
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1))/ DSIZE);

    DEBUG_PRINTV(("mm_malloc: After adjusting, allocation size is %lu bytes\n", asize));
    
    if(asize != size)
    {
	//printf("mm_malloc: Size was changed from %lu to %lu\n",size, asize);
    }

    //Todo: Here was previously calling implicit list's find fit. But call mine.
    /* Search the free list for a fit */
    DEBUG_PRINTV(("mm_malloc: Now call MY find_fit to search free list to insert in.\n"));
    if ((bp = findFitFreeList(asize)) != NULL) {	//was ((bp = find_fit(asize)) != NULL)
    	DEBUG_PRINTV(("mm_malloc: Found a fit. Returned bp is %p, size requested is %lu, actual size of block returned (using header) is %lu bytes! So NOW place.\n", bp, asize, GET_SIZE(HDRP(bp))));
        place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    DEBUG_PRINTV(("mm_malloc: No fit found. Get more memory by calling extend_heap with a size of %lu\n", extendsize));
    
    //Todo: Unsure of extensize amount to use. This extend only as much as is requested by asize
    //extendsize = asize;

    if ((bp = extend_heap(extendsize/WSIZE)) == NULL){	//remember extend_heap makes headers and footers and calls coalesce also
	DEBUG_PRINTV(("mm_malloc: Weird case! Extend heap returns NULL!\n"));
	return NULL;
    }
    
    DEBUG_PRINTV(("mm_malloc: Extended heap. bp is now %p. Will now place for requested %lu bytes\n", bp, asize));
    place(bp, asize);	//just sets up header and footer

    //checkSegregatedFreeLists
    return bp;
}


/**********************************************************
 * mm_realloc
 * Implemented simply in terms of mm_malloc and mm_free
 *********************************************************/
void *mm_realloc(void *ptr, size_t size)
{
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

//--------------------------------------------------------------------------------------------------------------------------------------------

/**********************************************************
 * mm_check
 * Check the consistency of the memory heap
 * Return nonzero if the heap is consistant.
 *********************************************************/
int mm_check(void){
  return 1;
}
