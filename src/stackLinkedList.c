#include "stackLinkedList.h"
#include <stdlib.h>
#include <stdio.h>


void StackPush_llist(int value)
{
    linkedListNode_t *newNode = (linkedListNode_t *)malloc(sizeof(linkedListNode_t));

    if (newNode == NULL)
    {
        printf("Stack error\r\n");
    }
    else
    {
        newNode->data = value;
        newNode->next = stackTop;
        stackTop = newNode;
    }
}

int StackPop_llist(void)
{
    linkedListNode_t *temp;
    int poppedData = -999;

    if (stackTop == NULL)
    {
        printf("Stack is empty\r\n");
    }
    else
    {
        temp = stackTop;
        stackTop = stackTop->next;
        poppedData = temp->data;
        free(temp);
    }
    return poppedData;
}

void PrintStack_llist(void)
{
    linkedListNode_t *temp;
    temp = stackTop;
    while (temp != NULL)
    {
        printf("%d\r\n", temp->data);
        temp = temp->next;
    }
    printf("\r\n");
}