#ifndef __SINGLY_LINKED_LIST__
#define __SINGLY_LINKED_LIST__

#include <stdlib.h>
#include <stdio.h>


typedef struct node
{
    int value;
    struct node *next;
} node_t;

extern node_t *head;

#endif  //SINGLY_LINKED_LIST