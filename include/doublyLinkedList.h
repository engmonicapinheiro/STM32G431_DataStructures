#ifndef __DOUBLY_LINKED_LIST_H__
#define __DOUBLY_LINKED_LIST_H__

struct doublyLinkedList_node
{
    int data;
    struct doublyLinkedList_node *prev;
    struct doublyLinkedList_node *next;
};

typedef struct doublyLinkedList_node doublyLinkedListNode_t;

void DoublyLinkedListInsertAtStart(doublyLinkedListNode_t **head, int data);
void DoublyLinkedListInsertAtEnd(doublyLinkedListNode_t **head, int data);
void DoublyLinkedListInsertAtPosition(doublyLinkedListNode_t **head, int data, int position);
int DoublyLinkedListLength(doublyLinkedListNode_t *head);
void DoublyLinkedListPrint(doublyLinkedListNode_t *head);
void DoublyLinkedListDeleteLastNode(doublyLinkedListNode_t **head);
void DoublyLinkedListDeleteFrontNode(doublyLinkedListNode_t **head);
void DoublyLinkedListDeletePositionNode(doublyLinkedListNode_t **head, int position);

#endif //DOUBLY_LINKED_LIST_H