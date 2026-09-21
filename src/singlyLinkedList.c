#include "singlyLinkedList.h"

node_t *head = NULL;

void CreateList(void)
{

}

void TransverseList(void)
{
    node_t *current = head;

    while (current != NULL)
    {
        //do something
        current = current->next;
    }
}

int CountNodes(void)
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

// void SearchValueInNode(int value)
// {
//     node_t *current = head;
//     node_t *position = NULL;
//
//     while (current != NULL)
//     {
//         if (current->value == value)
//         {
//             position = current;
//         }
//     }
// }