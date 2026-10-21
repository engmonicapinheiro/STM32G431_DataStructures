#ifndef __CIRCULAR_LINKED_LIST_H
#define __CIRCULAR_LINKED_LIST_H

struct circularLinkedListNode
{
    int data;
    struct circularLinkedListNode *next;
};

typedef struct circularLinkedListNode circularLinkedListNode_t;

void CircularLinkedListInsertAtStart(circularLinkedListNode_t **head, int data);
void CircularLinkedListInsertAtEnd(circularLinkedListNode_t **head, int data);
int CircularLinkedListLength(circularLinkedListNode_t *head);
void CircularLinkedListPrint(circularLinkedListNode_t *head);
void CircularLinkedListDeleteAtEnd(circularLinkedListNode_t **head);
void CircularLinkedListDeleteAtStart(circularLinkedListNode_t **head);

#endif //CIRCULAR_LINKED_LIST_H