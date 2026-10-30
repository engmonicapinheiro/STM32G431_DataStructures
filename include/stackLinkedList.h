#ifndef __STACK_LINKED_LIST__
#define __STACK_LINKED_LIST__

#include <stdio.h>

struct LinkedListNode_stack
{
    int data;
    struct LinkedListNode_stack *next;
} *stackTop;

typedef struct LinkedListNode_stack linkedListNode_t;

void StackPush_llist(int value);
int StackPop_llist(void);
void PrintStack_llist(void);


#endif //STACK_LINKED_LIST