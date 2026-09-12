#include "commands.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "gpio.h"
#include "uart.h"

/**
 * @brief Inserts a command at the tail of the linked list (command queue).
 * @param list: pointer to the linked list (command queue)
 * @param command: the command to be added
 * @return true if successful, false if not
 */
bool InsertAtTail(LinkedList_t *list, Command_t command)
{
    Node_t *newNode = (Node_t *)malloc(sizeof(Node_t)); //allocate a new node

    //allocation failed
    if (newNode == NULL)
    {
        return false;
    }

    newNode->command = command;  //set the command
    newNode->next = NULL;   //set the next to the tail of the list

    //if the list is empty, make the new node the head
    if (list->head == NULL)
    {
        list->head = newNode;
    }
    else
    {
        //if the list is not empty, traverse to the last node
        Node_t *current = list->head;
        while (current->next != NULL)
        {
            //move to the next node until the last node is reached
            current = current->next;
        }
        //link the new node to the current last node
        current->next = newNode;   //add to tail
    }
    return true;
}

/**
 * @brief Removes a command from the head of the command queue.
 * @param list: pointer to the linked list (command queue)
 * @param command: pointer to store the command in
 * @return true if successful, false if not
 */
bool RemoveAtHead(LinkedList_t *list, Command_t *command)
{
    if (list->head == NULL)
    {
        return false;  //list is empty, nothing to remove
    }

    Node_t *temp = list->head;   //get the first node
    *command = temp->command;   //copy the command
    list->head = temp->next;   //update head
    free(temp);  //free node memory
    return true;
}

void ProcessCommands(LinkedList_t *list)
{
    Command_t command;

    while (RemoveAtHead(list, &command))
    {
        switch (command.commandType)
        {
        case COMMAND_LED_ON:
            LedOn();
            printf("LED turned on. \n\r");
            break;

        case COMMAND_LED_OFF:
            LedOff();
            printf("LED turned off. \n\r");
            break;

        case COMMAND_READ_ADC:
            ProcessAdcCommand(&command);
            break;
        }

    }
}

void ProcessAdcCommand(Command_t *command)
{
    printf("ADC value: %lu\n\r", command->data);
}