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

#define WSIZE 4
#define DWSIZE (WSIZE * 2)

#define CHUNK_SIZE (1 << 12)

#define GET(p) (*(unsigned int *) (p))
//* *p = v
#define PUT(p, v) (*(unsigned int *)(p) = (v))

#define PACK(size, is_allocated) ((size) | (is_allocated))

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

static void *heap_list;

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    // allocate 4 word size memory for start block(1), prologue block(2) and end block(1)
    if ((heap_list = mem_sbrk(WSIZE * 4)) == -1) {
        return -1;
    }

    // void *start here is 0x0
    // set start block
    PUT(heap_list, 0);
    // set prologue block
    PUT(heap_list + WSIZE, PACK(DWSIZE, 1));
    PUT(heap_list + 2 * WSIZE, PACK(DWSIZE, 1));
    // set end block
    PUT(heap_list + 3 * WSIZE, PACK(0, 1));

    heap_list += (2 * WSIZE);

    // create a CHUNK_SIZE block
    extend_heap(CHUNK_SIZE / WSIZE);

    return 0;
}

int extend_heap(int size) {
    void *old_top;
    if ((old_top = mem_sbrk()))
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
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














