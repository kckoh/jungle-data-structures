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
#define PREV_FREE(bp) (*(void **)((bp) + DSIZE))


/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

// pointer to the prologue block (starting block)
static char *heap_listp = 0;
static char *last_allocp;
static char *free_listp;

static void remove_from_free_list(void *bp){
    // if bp prev is null -> bp is the first element
    // bp is the first element
    // free_listp = bp's next
    // printf("remove_from_free_list: %p, size=%zu\n", bp, GET_SIZE(HDRP(bp)));

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


// static void add_to_free_list(void *bp){
//       // Case 1: Empty list
//       if (free_listp == NULL) {
//           NEXT_FREE(bp) = NULL;
//           PREV_FREE(bp) = NULL;
//           free_listp = bp;
//           return;
//       }

//       // Case 2: Insert before first block (bp has lowest address)
//       if ((char *)bp < free_listp) {
//           NEXT_FREE(bp) = free_listp;
//           PREV_FREE(bp) = NULL;
//           PREV_FREE(free_listp) = bp;
//           free_listp = bp;
//           return;
//       }

//       // Case 3: Find correct position in middle or end
//       void *cur = free_listp;
//       while (NEXT_FREE(cur) != NULL && NEXT_FREE(cur) < bp) {
//           cur = NEXT_FREE(cur);
//       }

//       // Insert bp after cur
//       NEXT_FREE(bp) = NEXT_FREE(cur);
//       PREV_FREE(bp) = cur;
//       if (NEXT_FREE(cur) != NULL) {
//           PREV_FREE(NEXT_FREE(cur)) = bp;
//       }
//       NEXT_FREE(cur) = bp;
//   }

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
        remove_from_free_list(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));

        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));

        add_to_free_list(bp);
    }

    // prev is free but next is allocated
    else if (!prev_alloc && next_alloc) { /* Case 3 */
        remove_from_free_list(PREV_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));

        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(PREV_BLKP(bp)), PACK(size, 0));  // ✓ PREV_BLKP 사용!

        bp = PREV_BLKP(bp);
        add_to_free_list(bp);
    }

    // prev and next are free
    else { /* Case 4 */
        remove_from_free_list(NEXT_BLKP(bp));
        remove_from_free_list(PREV_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
                GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
        add_to_free_list(bp);
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
    void *cur = free_listp;

    while(cur != NULL){

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
            // last_allocp = bp;
            return bp;
        }
        bp = NEXT_BLKP(bp);
    }

    return NULL;  // No fit found
}

static void *best_fit(size_t asize)
{
    void *bp = free_listp;
    void *best = NULL;

    while (bp != NULL) {
        size_t bsize = GET_SIZE(HDRP(bp));
        if (asize <= bsize) {
            if (best == NULL || bsize < GET_SIZE(HDRP(best)))
                best = bp;
            // optional small cutoff: if perfect fit, stop early
            if (bsize == asize) break;
        }
        bp = NEXT_FREE(bp);
    }
    return best;
}


// free block을 할당하고, 필요하면 분할하는 함수
static void place(void *bp, size_t asize){
    size_t csize = GET_SIZE(HDRP(bp));
    remove_from_free_list(bp);


    // 나머지 블록을 split한다
    // Minimum free block: header(4) + next_ptr(8) + prev_ptr(8) + footer(4) = 24 bytes
    if ((csize - asize) >= (3*DSIZE)) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));

        void *next_bp = NEXT_BLKP(bp);
        PUT(HDRP(next_bp), PACK(csize-asize, 0));
        PUT(FTRP(next_bp), PACK(csize-asize, 0));
        add_to_free_list(next_bp);
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
    if ((bp = best_fit(asize)) != NULL) {
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
   coalesce(ptr);

}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
 void *mm_realloc(void *ptr, size_t size)
 {
     if (ptr == NULL) return mm_malloc(size);
     if (size == 0) { mm_free(ptr); return NULL; }

     size_t oldsize = GET_SIZE(HDRP(ptr));
     size_t asize = ALIGN(size + DSIZE); // include overhead + alignment

     // 1️⃣ Shrink case → do nothing
     if (asize <= oldsize) return ptr;

     // 2️⃣ Try to extend forward (next block)
     void *next = NEXT_BLKP(ptr);
     if (!GET_ALLOC(HDRP(next)) && GET_SIZE(HDRP(next)) != 0) {
         size_t next_size = GET_SIZE(HDRP(next));
         if (oldsize + next_size >= asize) {
             remove_from_free_list(next); // <- remove from free list
             size_t new_size = oldsize + next_size;
             PUT(HDRP(ptr), PACK(new_size, 1));
             PUT(FTRP(ptr), PACK(new_size, 1));
             return ptr; // success, no memcpy
         }
     }

     // 3️⃣ Try to extend backward (previous free block)
     void *prev = PREV_BLKP(ptr);
     if (!GET_ALLOC(HDRP(prev)) && GET_SIZE(HDRP(prev)) != 0) {
         size_t prev_size = GET_SIZE(HDRP(prev));
         if (prev_size + oldsize >= asize) {
             remove_from_free_list(prev);
             size_t new_size = prev_size + oldsize;
             PUT(HDRP(prev), PACK(new_size, 1));
             PUT(FTRP(prev), PACK(new_size, 1));
             memcpy(prev, ptr, oldsize - DSIZE); // small copy
             return prev;
         }
     }

     // 4️⃣ Otherwise, fallback: malloc → copy → free
     void *newptr = mm_malloc(size);
     if (newptr == NULL) return NULL;

     size_t copySize = oldsize - DSIZE;
     if (size < copySize) copySize = size;
     memcpy(newptr, ptr, copySize);
     mm_free(ptr);
     return newptr;
 }
