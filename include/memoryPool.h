#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__

#include <stdint.h>

/* configuration parameters */
#define MEMORY_POOL_SIZE  10   //number of memory blocks
#define MEMORY_BLOCK_SIZE 32   //size of each memory block in bytes
#define ALIGNMENT          4   //align memory blocks for 4-bytes boundaries

/* memory pool data structure */
typedef struct MemBlock
{
    struct MemBlock *next;  //pointer to the next free block
} MemBlock_t;

typedef struct
{
    /* pre-allocated memory */
    uint8_t pool[MEMORY_POOL_SIZE][MEMORY_BLOCK_SIZE];
    MemBlock_t *freeList;   //linked list of free blocks
} MemoryPool_t;

void MemoryPoolInit(void);
void *MemoryPoolAllocate(void);
void MemoryPoolFree(void *block);
uint32_t MemoryPoolGetFreeBlockCount(void);

#endif //MEMORY_POOL_H