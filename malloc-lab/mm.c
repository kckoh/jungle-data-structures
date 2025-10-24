/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <alloca.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};


// TODO:
// https://claude.ai/chat/eac24aeb-5755-40de-bfab-0c307af158ab
// add_to_free_list -> Done
// coalesce 수정
// 맨위에 잇는free list 추가 코드 삭제
// 각 케이스에 remove/add
// place 수정 -> remove_freom_free_list 추가
// 분할 수 남은 block을 add_to_free_list추가
// mm_free 수정 -> last_allocp 코드 삭제
/* Basic constants and macros */
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)

#define MAX(x,y) ((x) > (y)? (x) : (y))

#define PACK(size, alloc) ((size) | (alloc))

#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
// get_size -> returns the size of the whole block - header + footer (DSIZE) + payload address
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

#define NEXT_FREE(bp) (*(void **)bp)
#define PREV_FREE(bp) (*(void **)((bp) + WSIZE))


/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

// pointer to the prologue block (starting block)
static char *heap_listp = 0;

static char *free_listp;

static void remove_from_free_list(void *bp){
    // if bp prev is null -> bp is the first element
    // bp is the first element
    // free_listp = bp's next
    if (PREV_FREE(bp) == NULL) {
        free_listp = NEXT_FREE(bp);
    }
    // else bp's prev's next = bp next; 전에 있는 next 를 bp next로
    else {
        NEXT_FREE(PREV_FREE(bp)) = NEXT_FREE(bp);
    }

    // 그전에 있는 bp's next 의 prev = bp's prev 으로
    if (NEXT_FREE(bp) != NULL){
        PREV_FREE(NEXT_FREE(bp)) = PREV_FREE(bp);
    }

}

/*  Before:
free_listp → [A] ↔ [B]
After:
free_listp → [bp] ↔ [A] ↔ [B]
*/
static void add_to_free_list(void *bp){
    NEXT_FREE(bp) = free_listp;
    PREV_FREE(bp) = NULL;
    if(free_listp != NULL){
        PREV_FREE(free_listp) = bp;
    }
    free_listp = bp;

}

static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));


    // both prev and next are allocated
    if (prev_alloc && next_alloc) { /* Case 1 */
        add_to_free_list(bp);
        return bp;
    }

    // prev is allocated but next is free
    else if (prev_alloc && !next_alloc) { /* Case 2 */
        remove_from_free_list(NEXT_FREE(bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        add_to_free_list(bp);
    }

    // prev is free but next is allocated
    else if (!prev_alloc && next_alloc) { /* Case 3 */
        add_to_free_list(PREV_FREE(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
        add_to_free_list(bp);
    }

    // prev and next are free
    else { /* Case 4 */

        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
                GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    return bp;
}



// helper functions
static void *extend_heap(size_t words){
    char *bp;
    size_t size;
    // allocate an even number of words to maintain alignment
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    // bp definition and increase it by size
   if ((long)(bp = mem_sbrk(size)) == -1) return NULL;

   /* Initialize free block header/footer and the epilogue header */
   PUT(HDRP(bp), PACK(size, 0)); /* Free block header */
   PUT(FTRP(bp), PACK(size, 0)); /* Free block footer */
   PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */


   /* Coalesce if the previous block was free */
   return coalesce(bp);
}


/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{

    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;


    // initialize necessary headers and alignment

    PUT(heap_listp, 0); /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap_listp + (3*WSIZE), PACK(0, 1)); /* Epilogue header */

    heap_listp += (2*WSIZE);

    free_listp = NULL;

    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) return -1;

    return 0;
}

static void *first_fit(size_t asize){

    // find the first pointer
    void *cur = free_listp;

    while(cur != NULL){
        // not found
        if(GET_SIZE(HDRP(cur)) >= asize){
            return cur;
        }
        cur = NEXT_FREE(cur);
    }

    return NULL;
}

static void *next_fit(size_t asize) {
    char *bp = NEXT_BLKP(last_allocp);
    char *start = bp;

    // Search from last_allocp to end of heap
    while (GET_SIZE(HDRP(bp)) != 0) {
        if (!GET_ALLOC(HDRP(bp)) && GET_SIZE(HDRP(bp)) >= asize) {
            last_allocp = bp;
            return bp;
        }
        bp = NEXT_BLKP(bp);
    }

    // Hit epilogue, wrap to beginning
    bp = NEXT_BLKP(heap_listp);

    // Search from beginning until we reach where we started
    while (bp != start) {
        if (GET_SIZE(HDRP(bp)) == 0) break;  // Hit epilogue again
        if (!GET_ALLOC(HDRP(bp)) && GET_SIZE(HDRP(bp)) >= asize) {
            last_allocp = bp;
            return bp;
        }
        bp = NEXT_BLKP(bp);
    }

    return NULL;  // No fit found
}

// free block을 할당하고, 필요하면 분할하는 함수
static void place(void *bp, size_t asize){
    size_t csize = GET_SIZE(HDRP(bp));


    if ((csize - asize) >= (2*DSIZE)) {  // Minimum block size
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize-asize, 0));
        PUT(FTRP(bp), PACK(csize-asize, 0));
    }
    else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}



/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize; // adjusted block size
    size_t extendsize; // amount to extend heap if no fit
    char *bp;

    // ignore requests
    if (size == 0) return NULL;

    // Adjust block size to include overhead and alignment req
    // size smaller than DSIZE requires at leanst 16 bytes (header 4 + footer 4 + payload 8)
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    // round to the multiple of 8
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE -1)) / DSIZE);

    // Search the free list for a fit
    if ((bp = first_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    // no fit found. Get more memory and place the block
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;

    place(bp, asize);

    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    size_t size = GET_SIZE(HDRP(ptr));
   PUT(HDRP(ptr), PACK(size, 0));
   PUT(FTRP(ptr), PACK(size, 0));
   void *coalesced = coalesce(ptr);

    // Update last_allocp to the coalesced block to maintain next-fit behavior
    // last_allocp = coalesced;
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}
