#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <stdint.h>
#include <stdbool.h>


/* maximum number of commands that can be stored */
#define COMMAND_QUEUE_SIZE  32

/* enumeration for the different kinds of commands */
typedef enum
{
    COMMAND_LED_ON = 1,  //turn the led on
    COMMAND_LED_OFF,    //turn the led off
    COMMAND_READ_ADC    //read the adc value
} CommandType_t;

/* struct representing the command */
typedef struct
{
    CommandType_t commandType;  //type of command
    uint32_t data;    //command data
} Command_t;

/* struct to represent a node in the linked list */
typedef struct Node
{
    Command_t command;   //command to be performed
    struct Node *next;   //pointer to the next node in the list
} Node_t;

/* structure representing the linked list of commands */
typedef struct
{
    Node_t *head;
} LinkedList_t;

bool InsertAtTail(LinkedList_t *list, Command_t command);
bool RemoveAtHead(LinkedList_t *list, Command_t *command);
void ProcessCommands(LinkedList_t *list);
void ProcessAdcCommand(Command_t *command);


#endif //COMMANDS_H