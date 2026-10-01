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

/* functions to print and process the menu */
int PrintLinkedListMenu(void);
void LinkedListMenu(int option);


/* Linked List functions */
void CreateLinkedList(void);
void LinkedListDisplay(void);
void LinkedListTransverseList(void);
int LinkedListCountNodes(void);
char LinkedListSearchValueInNode(int value);
void CreateSingleNodeInLinkedList(void);
void LinkedListDeleteList(void);

/* to insert a new node in a Linked List:
 * case 1: the new node is inserted at the beginning of the list
 * case 2: the new node is inserted at the end of the list
 * case 3: the new node is inserted after a given node
 * case 4: the new node is inserted before a given node
 */
void LinkedListInsertAtBeginning(int value);
void LinkedListInsertAtEnd(int value);
void LinkedListInsertAfterNode(int newValue, int targetValue);
void LinkedListInsertBeforeNode(int newValue, int targetValue);

/* to delete a node from a linked list:
 * case 1: the first node is deleted
 * case 2: the last node is deleted
 * case 3: the node after a given node is deleted
 */
void LinkedListDeleteAtBeginning(void);
void LinkedListDeleteAtEnd(void);
void LinkedListDeleteAfterNode(int value);


#endif  //SINGLY_LINKED_LIST