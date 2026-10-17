#include "doublyLinkedList.h"
#include <stdio.h>
#include <stdlib.h>
#include "uart.h"


void DoublyLinkedListInsertAtStart(doublyLinkedListNode_t **head, int data)
{
    doublyLinkedListNode_t *newNode =  (doublyLinkedListNode_t *) malloc(sizeof(doublyLinkedListNode_t));

    if (!newNode)
    {
        printf("Memory error\r\n");
        return;
    }

    newNode->prev = NULL;
    newNode->data = data;
    newNode->next = NULL;

    /* check if the current head is point to null. if that's the case, the new node becomes the head */
    if (*head == NULL)
    {
        *head = newNode;
        return;
    }

    newNode->next = *head;
    (*head)->prev = newNode;
    *head = newNode;
}

void DoublyLinkedListInsertAtEnd(doublyLinkedListNode_t **head, int data)
{
    doublyLinkedListNode_t *current = *head;
    doublyLinkedListNode_t *newNode = (doublyLinkedListNode_t *)malloc(sizeof(doublyLinkedListNode_t));

    if (!newNode)
    {
        printf("Memory error\r\n");
        return;
    }

    newNode->prev = NULL;
    newNode->data = data;
    newNode->next = NULL;

    if (*head == NULL)
    {
        *head = newNode;
        return;
    }

    /* transverse the list then append at the end of the list */
    while (current->next != NULL)
    {
        current = current->next;
    }
    newNode->prev = current;
    current->next = newNode;
}
void DoublyLinkedListInsertAtPosition(doublyLinkedListNode_t **head, int data, int position)
{
    doublyLinkedListNode_t *current = *head;
    doublyLinkedListNode_t *newNode = (doublyLinkedListNode_t *)malloc(sizeof(doublyLinkedListNode_t));

    if (!newNode)
    {
        printf("Memory error\r\n");
        return;
    }

    newNode->prev = NULL;
    newNode->data = data;
    newNode->next = NULL;

    if (position == 1)
    {
        newNode->next = *head;
        newNode->prev = NULL;

        if (*head)
        {
            (*head)->prev = newNode;
        }
        *head = newNode;
        return;
    }

    int k = 1;
    while ((k < position - 1) && (current->next != NULL))
    {
        current = current->next;
        k++;
    }

    if (k < position - 1)
    {
        printf("Invalid position passed.\r\n");
        return;
    }

    newNode->next = current->next;
    newNode->prev = current;

    if (current->next != NULL)
    {
        current->next->prev = newNode;
    }

    current->next = newNode;
}

int DoublyLinkedListLength(doublyLinkedListNode_t *head)
{
    doublyLinkedListNode_t *current = head;
    int count = 0;

    if (head == NULL)
    {
        return 0;
    }

    while (current != NULL)
    {
        current= current->next;
        count++;
    }

    return count;
}

void DoublyLinkedListPrint(doublyLinkedListNode_t *head)
{
    doublyLinkedListNode_t *current = head;
    if (head == NULL)
    {
        return;
    }
    while (current != NULL)
    {
        printf("%d --> ", current->data);
        current = current->next;
    }

    printf("NULL\r\n");
}

void DoublyLinkedListDeleteLastNode(doublyLinkedListNode_t **head)
{
    doublyLinkedListNode_t *temp = *head;
    doublyLinkedListNode_t *current = *head;

    if (*head == NULL)
    {
        printf("The list is empty.\r\n");
    }

    while (current->next != NULL)
    {
        current = current->next;
    }

    temp = current->prev;
    temp->next = current->next;
    free(current);

}

void DoublyLinkedListDeleteFrontNode(doublyLinkedListNode_t **head)
{
    doublyLinkedListNode_t *temp = *head;

    if (*head == NULL)
    {
        printf("The list is empty.\r\n");
    }

    temp = *head;
    *head = (*head)->next;
    (*head)->prev = NULL;
    free(temp);
}

void DoublyLinkedListDeletePositionNode(doublyLinkedListNode_t **head, int position)
{
    doublyLinkedListNode_t *temp = *head;
    doublyLinkedListNode_t *current = *head;
    int k = 1;

    if (*head == NULL)
    {
        printf("The list is empty.\r\n");
    }

    if (position == 1)
    {
        *head = (*head)->next;
        if (*head != NULL)
        {
            (*head)->prev = NULL;
        }
        free(temp);
    }

    while (k < position && temp->next != NULL)
    {
        temp = temp->next;
        k++;
    }

    if (k < position - 1)
    {
        printf("Invalid position passed.\r\n");
        return;
    }

    current = temp->prev;
    current->next = temp->next;
    if (temp->next)
    {
        temp->next->prev = current;
    }
    free(temp);
}
