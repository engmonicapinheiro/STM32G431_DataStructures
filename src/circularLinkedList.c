#include "circularLinkedList.h"
#include <stdlib.h>
#include <stdio.h>


void CircularLinkedListInsertAtStart(circularLinkedListNode_t **head, int data)
{
    circularLinkedListNode_t *current = *head;
    circularLinkedListNode_t *newNode = (circularLinkedListNode_t *)malloc(sizeof(circularLinkedListNode_t));

    if (!newNode)
    {
        printf("Memory error!\r\n");
        return;
    }

    newNode->data = data;
    newNode->next = newNode;

    if (current == NULL)
    {
        *head = newNode;
        return;
    }

    while (current->next != *head)
    {
        current = current->next;
    }

    newNode->next = *head;
    current->next = newNode;
    *head = newNode;
}

void CircularLinkedListInsertAtEnd(circularLinkedListNode_t **head, int data)
{
    circularLinkedListNode_t *current = *head;
    circularLinkedListNode_t *newNode = (circularLinkedListNode_t *)malloc(sizeof(circularLinkedListNode_t));

    if (!newNode)
    {
        printf("Memory error!\r\n");
        return;
    }

    newNode->data = data;
    newNode->next = newNode;

    if (current == NULL)
    {
        *head = newNode;
        return;
    }

    while (current->next != *head)
    {
        current = current->next;
    }

    newNode->next = *head;
    current->next = newNode;
}

int CircularLinkedListLength(circularLinkedListNode_t *head)
{
    circularLinkedListNode_t *current = head;
    int count = 0;

    if (head == NULL)
    {
        printf("List empty\r\n");
        return 0;
    }

    do
    {
        current = current->next;
        count++;
    }while (current != head);

    return count;
}

void CircularLinkedListPrint(circularLinkedListNode_t *head)
{
    circularLinkedListNode_t *current = head;

    if (head == NULL)
    {
        printf("List empty\r\n");
        return;
    }

    do
    {
        printf("%d --> ", current->data);
        current = current->next;
    }while (current != head);

    printf("\r\n");
}

void CircularLinkedListDeleteAtEnd(circularLinkedListNode_t **head)
{
    circularLinkedListNode_t *current = *head;
    circularLinkedListNode_t *temp = *head;

    if (*head == NULL)
    {
        printf("List empty\r\n");
        return;
    }

    while (current->next != *head)
    {
        temp = current;
        current = current->next;
    }

    /* skipping over current */
    temp->next = current->next;
    free(current);
}

void CircularLinkedListDeleteAtStart(circularLinkedListNode_t **head)
{
    circularLinkedListNode_t *current = *head;

    if (*head == NULL)
    {
        printf("List empty\r\n");
        return;
    }

    while (current->next != *head)
    {
        current = current->next;
    }
    current->next = (*head)->next;
    *head = (*head)->next;
    free(current);
}
