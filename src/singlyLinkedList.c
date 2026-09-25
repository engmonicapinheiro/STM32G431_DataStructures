#include "singlyLinkedList.h"

node_t *head = NULL;

void CreateList(void)
{

}

void LinkedListTransverseList(void)
{
    node_t *current = head;

    while (current != NULL)
    {
        //do something
        current = current->next;
    }
}

int LinkedListCountNodes(void)
{
    node_t *current = head;
    int count = 0;

    while (current != NULL)
    {
        count++;
        current = current->next;
    }

    return count;
}

char LinkedListSearchValueInNode(int value)
{
    node_t *current = head;
    node_t *position = NULL;
    char result = 0;

    while (current != NULL)
    {
        if (current->value == value)
        {
            position = current;
            result = 1;
        }
        else
        {
            position = position->next;
        }
    }
    position = NULL;
    return result;
}

/* to insert a new node in a Linked List:
 * case 1: the new node is inserted at the beginning of the list
 * case 2: the new node is inserted at the end of the list
 * case 3: the new node is inserted after a given node
 * case 4: the new node is inserted before a given node
 */
void LinkedListInsertAtBeginning(int value)
{
    //allocate memory for the new node
    node_t *newNode = (node_t *)malloc(sizeof(node_t));

    /* If the free memory has exhausted, then an overflow message is printed. */
    if (newNode == NULL)
    {
        printf("OVERFLOW: no free memory available.\n");
        return;
    }

    newNode->value = value;
    newNode->next = head;
    head = newNode;

    printf("Node with value %d was inserted at the beginning of the list.\n\r", newNode->value);
}

void LinkedListInsertAtEnd(int value)
{
    node_t *current = head;

    /* allocate memory for the new node */
    node_t *newNode = (node_t *)malloc(sizeof(node_t));

    /* if the free memory has exhausted, print an overflow message */
    if (newNode == NULL)
    {
        printf("OVERFLOW: no free memory available.\n");
        return;
    }

    newNode->value = value;
    newNode->next = NULL;

    /* if the list is empty, place the new node in it and finish */
    if (head == NULL)
    {
        head = newNode;
        return;
    }

    while (current->next != NULL)
    {
        current = current->next;
    }
    /* set the new node as the last one */
    current->next = newNode;

    printf("Node with value %d was inserted at the end of the list.\n\r", newNode->value);

}

void LinkedListInsertAfterNode(int value, node_t *position)
{
    /* allocate data for the new node */
    node_t *newNode = (node_t *)malloc(sizeof(node_t));

    if (newNode == NULL)
    {
        printf("OVERFLOW: no free memory available.\n");
        return;
    }

    /* take a pointer variable which points to the start of the list */
    node_t *current = position;

    /* used to store the address of the node preceding current */
    node_t *previous = current;

    /* assign the given data to the new node */
    newNode->value = value;

    while (previous->value != value)
    {
        previous = current;
        current = current->next;
    }

    /* the node with the desired value is reached, so adjust the next fields
     * from the new node and the previous node */
    previous->next = newNode;
    newNode->next = current;

    printf("Node with value %d was inserted after the node %d.\n\r", newNode->value, value);
}

void LinkedListInsertBeforeNode(int value, node_t *position)
{
    node_t *newNode = (node_t *)malloc(sizeof(node_t));

    if (newNode == NULL)
    {
        printf("OVERFLOW: no free memory available.\n");
        return;
    }

    newNode->value = value;

    node_t *current = position;
    node_t *previous = position;

    while (current->value != value)
    {
        previous = current;
        current = current->next;
    }
    previous->next = newNode;
    newNode->next = current;

    printf("Node with value %d was inserted before the node %d.\n\r", newNode->value, value);
}