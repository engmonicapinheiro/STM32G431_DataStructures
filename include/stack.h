#ifndef __STACK_H__
#define __STACK_H__

struct Stack
{
    int top;
    int capacity;
    int *array;
};

typedef struct Stack stack_t;

stack_t * StackCreate(int capacity);
void StackDoubleSize(stack_t *stack);
int StackIsFull(stack_t *stack);
int StackIsEmpty(stack_t *stack);
int StackGetSize(stack_t *stack);
void StackPush(stack_t *stack, int data);
int StackPop(stack_t *stack);
int StackPeek(stack_t *stack);
void StackDestroy(stack_t *stack);

#endif //STACK_H
