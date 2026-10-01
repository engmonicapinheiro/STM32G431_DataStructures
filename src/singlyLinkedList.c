#include "singlyLinkedList.h"
#include "uart.h"

node_t *head = NULL;

int PrintLinkedListMenu(void)
{
    int option;

    printf("\r\n\r\n****** MAIN MENU ******\r\n\r\n");
    printf("1: Create a list\r\n");
    printf("2: Display the list\r\n");
    printf("3: Add a node at the beginning\r\n");
    printf("4: Add a node at the end\r\n");
    printf("5: Add a node before a given node\r\n");
    printf("6: Add a node after a given node\r\n");
    printf("7: Delete a node at the beginning of the list\r\n");
    printf("8: Delete a node at the end of the list\r\n");
    printf("9: Delete a node after a given node\r\n");
    printf("10: Delete the whole list\r\n");
    printf("11: Count how many nodes there are in the list\r\n");

    option = UartReadInt();

    return option;
}

void LinkedListMenu(int option)
{
    switch (option)
    {
        case 1:
            {
                /* create a list */
                printf("\r\noption 1 chosen\r\n");
                CreateLinkedList();
                break;
            }

        case 2:
            {
                /* display the whole list */
                printf("\r\noption 2 chosen\r\n");
                LinkedListDisplay();
                break;
            }

        case 3:
            {
                /* Add a node at the beginning */
                printf("\r\noption 3 chosen\r\n");
                printf("\r\nChoose a value for the new node\r\n");
                int value = UartReadInt();
                LinkedListInsertAtBeginning(value);
                break;
            }

        case 4:
            {
                /* Add a node at the end */
                printf("\r\noption 4 chosen\r\n");
                printf("\r\nChoose a value for the new node\r\n");
                int value = UartReadInt();
                LinkedListInsertAtEnd(value);
                break;
            }

        case 5:
            {
                /* Add a node before a given node */
                printf("\r\noption 5 chosen\r\n");
                printf("\r\nInsert the position of the node\r\n");
                int value = UartReadInt();
                printf("\r\nWhere do you want to place this node?\r\n");
                int position = UartReadInt();
                LinkedListInsertBeforeNode(value, position);
                break;
            }

        case 6:
            {
                /* Add a node after a given node */
                printf("\r\noption 6 chosen\r\n");
                printf("\r\nInsert the position of the node\r\n");
                int value = UartReadInt();
                printf("\r\nWhere do you want to place this node?\r\n");
                int position = UartReadInt();
                LinkedListInsertAfterNode(value, position);
                break;
            }
        case 7:
            {
                /* Delete a node at the beginning of the list */
                printf("\r\noption 7 chosen\r\n");
                LinkedListDeleteAtBeginning();
                break;
            }
        case 8:
            {
                /* Delete a node at the end of the list */
                printf("\r\noption 8 chosen\r\n");
                LinkedListDeleteAtEnd();
                break;
            }

        case 9:
            {
                /* Delete a node after a given node */
                printf("\r\noption 9 chosen\r\n");
                printf("\r\nInsert the position of the node\r\n");
                int value = UartReadInt();
                LinkedListDeleteAfterNode(value);
                break;
            }

        case 10:
            {
                /* Delete the whole list */
                printf("\r\noption 10 chosen\r\n");
                LinkedListDeleteList();
                break;
            }
        case 11:
                {
                    /* count the nodes in the list*/
                    printf("\r\noption 11 chosen\r\n");
                    int total = LinkedListCountNodes();
                    printf("\r\nThere are %d nodes in the list\r\n", total);
                    break;
                }

        default:
            printf("\n\rInvalid option\n\r");
            break;
    }
}

void CreateSingleNodeInLinkedList(void)
{
    node_t *newNode;

    printf("\r\nEnter a value for the first node\r\n");

    int value = UartReadInt();

    newNode = (node_t *)malloc(sizeof(node_t));

    if (newNode == NULL)
    {
        printf("OVERFLOW: no free memory available.\r\n");
        return;
    }

    newNode->value = value;
    newNode->next = NULL;
    head = newNode;

    printf("\r\nList created successfully\r\n");
}

void CreateLinkedList(void)
{
    int value;
    printf("\r\nEnter a value for a node. Enter -1 to stop\r\n");

    while (1)
    {
        value = UartReadInt();
        if (value == -1)
        {
            break;
        }

        LinkedListInsertAtEnd(value);
        printf("\r\nEnter a value for a node. Enter -1 to stop\r\n");
    }

    printf("\r\nList created successfully\r\n");

}

void LinkedListDisplay(void)
{
    node_t *current = head;
    while(current != NULL)
    {
        printf("\t %d ->", current->value);
        current = current->next;
    }
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

    while (current != NULL)
    {
        if (current->value == value)
        {
            return 1;
        }
        current = current->next;

    }
    return 0;
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

void LinkedListInsertAfterNode(int newValue, int targetValue)
{
    if (head == NULL)
    {
        printf("\r\nThe list is empty\r\n");
        return;
    }

    node_t *target = head;
    while (target != NULL && target->value != targetValue)
    {
        target = target->next;
    }

    if (target == NULL)
    {
        printf("Target value %d was not found.\r\n", targetValue);
        return;
    }

    /* inserting after the last node is the same as inserting at the end */
    if (target->next == NULL)
    {
        LinkedListInsertAtEnd(newValue);
        return;
    }

    /* allocate data for the new node */
    node_t *newNode = (node_t *)malloc(sizeof(node_t));

    if (newNode == NULL)
    {
        printf("OVERFLOW: no free memory available.\n");
        return;
    }

    newNode->value = newValue;
    newNode->next = target->next;
    target->next = newNode;

    printf("Node with value %d was inserted after the node %d.\n\r", newValue, targetValue);
}

void LinkedListInsertBeforeNode(int newValue, int targetValue)
{
    if (head == NULL)
    {
        printf("\r\nThe list is empty\r\n");
        return;
    }

    /* inserting before the head */
    if (head->value == targetValue)
    {
        LinkedListInsertAtBeginning(newValue);
        return;
    }

    node_t *previous = head;
    while (previous->next != NULL && previous->next->value != targetValue)
    {
        previous = previous->next;
    }
    if (previous->next == NULL)
    {
        printf("\r\nTarget value %d was not found\r\n", targetValue);
        return;
    }
    node_t *newNode = (node_t *)malloc(sizeof(node_t));
    if (newNode == NULL)
    {
        printf("OVERFLOW: no free memory available.\n");
        return;
    }
    newNode->value = newValue;
    newNode->next = previous->next;
    previous->next = newNode;

    printf("Node with value %d was inserted before the node %d.\n\r", newValue, targetValue);
}

/* to delete a node from a linked list:
 * case 1: the first node is deleted
 * case 2: the last node is deleted
 * case 3: the node after a given node is deleted
 */
void LinkedListDeleteAtBeginning(void)
{
    /* the underflow condition */
    if (head == NULL)
    {
        printf("UNDERFLOW: no node to be deleted\r\n");
        return;
    }

    node_t *current = head;
    head = head->next;
    free(current);

    printf("First node deleted successfully. \r\n");
}

void LinkedListDeleteAtEnd(void)
{
    /* the underflow condition */
    if (head == NULL)
    {
        printf("UNDERFLOW: no node to be deleted\r\n");
        return;
    }

    node_t *current = head;
    node_t *previous = head;

    while (current->next != NULL)
    {
        previous = current;
        current = current->next;  //advance current one more node
    }

    previous->next = NULL;
    free(current);

    printf("Last node deleted successfully. \r\n");
}

void LinkedListDeleteAfterNode(int value)
{
    /* the underflow condition */
    if(head == NULL)
    {
        printf("UNDERFLOW: no node to be deleted\r\n");
        return;
    }

    node_t *target = head;

    while (target != NULL && target->value != value)
    {
        target = target->next; //advance target one more node
    }

    if (target == NULL)
    {
        printf("Node with value %d not found.\r\n", value);
        return;
    }

    node_t *toDelete = target->next;
    target->next = toDelete->next;
    free(toDelete);

    printf("Node %d deleted successfully\r\n", value);
}

void LinkedListDeleteList(void)
{
    node_t *current = head;
    node_t *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    head = NULL;

    printf("\r\nList deleted successfully\r\n");
}