#include "eventManagement.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stm32g431xx.h"
#include "rtc.h"

uint8_t received_data = 0;
static uint8_t commandBuffer[MAX_COMMAND_LEN];
static uint8_t buffer_index = 0;

static EventNode_t *eventListHead = NULL;

void AddEvent(const char *description)
{
    /* allocate memory for the new node */
    EventNode_t *newEvent = (EventNode_t *)malloc(sizeof(EventNode_t));

    if (!newEvent)
    {
        printf("Memory allocation failed!\n\r");
        return;
    }

    /* populate the new event node */
    newEvent->timestamp = get_current_timestamp();
    snprintf(newEvent->description, MAX_EVENT_DESC_LEN, "%s", description);
    newEvent->next = NULL;

    /* insert the new event at the end of the list */
    if (!eventListHead)
    {
        eventListHead = newEvent;
    }
    else
    {
        EventNode_t *current = eventListHead;
        while (current->next)
        {
            current = current->next;
        }
        current->next = newEvent;
    }

    printf("Event added: %s at timestamp %lu\n\r", description, newEvent->timestamp);
}

void PrintEventList(void)
{
    if (!eventListHead)
    {
        printf("No events to display!\n\r");
        return;
    }

    EventNode_t *current = eventListHead;
    printf("Event list: \n\r");

    /* traverse the list */
    while (current)
    {
        printf("Timestamp> %lu, Description: %s\r\n", current->timestamp, current->description);
        current = current->next;
    }
}

/**
 * @brief Remove an event by its timestamp.
 * @param timestamp: timestamp of the event to be removed.
 */
void RemoveEventByTimestamp(uint32_t timestamp)
{
    EventNode_t *current = eventListHead;
    EventNode_t *previous = NULL;

    while (current)
    {
        if (current->timestamp == timestamp)
        {
            if (previous)
            {
                previous->next = current->next;
            }
            else
            {
                eventListHead = current->next;
            }

            free(current);
            printf("Event with timestamp %lu removed.\n\r", timestamp);
            return;
        }

        previous = current;
        current = current->next;
    }

    printf("Event with timestamp %lu not found.\n\r", timestamp);
}


void HandleUartCommand(const char *command)
{
    if (strstr(command, "add_event") == command)
    {
        char description[MAX_EVENT_DESC_LEN];
        sscanf(command, "add_event %s", description);
        AddEvent(description);
    }
    else if (strstr(command, "print_event") == command)
    {
        PrintEventList();
    }
    else if (strstr(command, "remove_event") == command)
    {
        uint32_t timestamp;
        sscanf(command, "remove_event %lu", &timestamp);
        RemoveEventByTimestamp(timestamp);
    }
    else
    {
        printf("Unknown command %s\n\r", command);
    }
}

/* example:
 * add_event ButtonPressed*
 */

void USART2_IRQHandler(void)
{
    if (USART2->ISR & USART_ISR_RXNE)
    {
        received_data = (uint8_t)USART2->RDR; //reading RDR clears RXNE automatically

        if(received_data == '*')   //command termination
        {
            commandBuffer[buffer_index] = '\0';  //null - terminate the string
            //process the command
            HandleUartCommand((const char *)commandBuffer);
            buffer_index = 0;
        }
        else
        {
            if(buffer_index < MAX_COMMAND_LEN - 1)
            {
                commandBuffer[buffer_index++] = received_data;
            }
        }
    }
}