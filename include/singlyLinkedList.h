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

void LinkedListTransverseList(void);
int LinkedListCountNodes(void);
char LinkedListSearchValueInNode(int value);

/* to insert a new node in a Linked List:
 * case 1: the new node is inserted at the beginning of the list
 * case 2: the new node is inserted at the end of the list
 * case 3: the new node is inserted after a given node
 * case 4: the new node is inserted before a given node
 */
void LinkedListInsertAtBeginning(int value);
void LinkedListInsertAtEnd(int value);
void LinkedListInsertAfterNode(int value, node_t *position);
void LinkedListInsertBeforeNode(int value, node_t *position);

#endif  //SINGLY_LINKED_LIST