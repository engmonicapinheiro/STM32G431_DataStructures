#include "stack.h"
#include <stdlib.h>
#include <stdio.h>


stack_t * StackCreate(int capacity)
{
    stack_t *stack = (stack_t *) malloc(sizeof(stack_t));

    if (stack == NULL)
    {
        printf("Not able to create the stack \r\n");
        return NULL;
    }

    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (int *) malloc(stack->capacity * sizeof(int));

    if (stack->array == NULL)
    {
        printf("Not able to create a stack of this size \r\n");
        return NULL;
    }

    return stack;
}

void StackDoubleSize(stack_t *stack)
{
    stack->capacity *= 2;
    stack->array = (int *)realloc(stack->array, stack->capacity * sizeof(int));
}

int StackIsFull(stack_t *stack)
{
    return (stack->top == stack->capacity - 1);
}

int StackIsEmpty(stack_t *stack)
{
    return (stack->top == -1);
}

int StackGetSize(stack_t *stack)
{
    return(stack->top + 1);
}

void StackPush(stack_t *stack, int data)
{
    if (StackIsFull(stack))
    {
        StackDoubleSize(stack);
    }

    stack->array[++stack->top] = data;
}

int StackPop(stack_t *stack)
{
    if (StackIsEmpty(stack))
    {
        printf("The stack is empty \r\n");
        return -1;
    }

    return stack->array[stack->top--];
}

int StackPeek(stack_t *stack)
{
    if (StackIsEmpty(stack))
    {
        printf("The stack is empty \r\n");
        return -1;
    }

    return stack->array[stack->top];
}

void StackDestroy(stack_t *stack)
{
    if (stack != NULL)
    {
        if (stack->array)
        {
            free(stack->array);
        }
        free(stack);
    }
}