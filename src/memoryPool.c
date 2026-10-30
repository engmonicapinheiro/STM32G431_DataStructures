#include "memoryPool.h"
#include <stdlib.h>

static MemoryPool_t memPool;

/* @brief Initialises the memory pool by linking all blocks
 * into a free list
 */
void MemoryPoolInit(void)
{
    memPool.freeList = (MemBlock_t *) memPool.pool;

    /* link all blocks together in a free list */
    for (int i = 0; i < MEMORY_POOL_SIZE - 1; ++i)
    {
        ((MemBlock_t *)memPool.pool[i])->next = (MemBlock_t *)memPool.pool[i + 1];
    }
    ((MemBlock_t  *)memPool.pool[MEMORY_POOL_SIZE - 1])->next = NULL;  //end of list
}

/* @brief this function allocates a block of memory from the pool
 * @retval pointer to the allocated block, or NULL if pool is exhausted
 */
void *MemoryPoolAllocate(void)
{
    if (memPool.freeList == NULL)
    {
        return NULL;  //no free blocks available
    }

    MemBlock_t *allocatedBlock = memPool.freeList;  //taking the first block
    memPool.freeList = allocatedBlock->next;   //moving the free list pointer
    return (void *)allocatedBlock;
}

/* @brief this function frees a previously allocated block, returning it to the pool
 * @param pointer to the block to be freed
 */
void MemoryPoolFree(void *block)
{
    if (block == NULL)
    {
        return;
    }

    MemBlock_t *tempBlock = (MemBlock_t *)block;
    tempBlock->next = memPool.freeList;  //return the block to the free list
    memPool.freeList = tempBlock;
}

uint32_t MemoryPoolGetFreeBlockCount(void)
{
    uint32_t count = 0;
    MemBlock_t *current = memPool.freeList;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }

    return count;
}