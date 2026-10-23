#include "stm32g431xx.h"
#include <stdio.h>
#include "fpu.h"
#include "uart.h"
#include "timebase.h"
#include "gpio.h"
#include "adc.h"
#include "commands.h"
#include "rtc.h"
#include "eventManagement.h"
#include "singlyLinkedList.h"
#include "doublyLinkedList.h"
#include "circularLinkedList.h"
#include "stack.h"

#define VREF_PLUS_MV 3300U

bool buttonState;
uint32_t sensorValue;
float chipTemperature = 0.0f;

int main()
{
    /* enable the FPU */
    FpuEnable();
    /* initialise the UART */
    UartInit();
    /* RTC init */
    rtc_init();
    /* initialise timebase */
    TimebaseInit();
    /* initialise the LED */
    LedInit();
    /* initialise the button */
    ButtonInit();
    /* initialise the ADC */
   //  AdcInit();
    /* start conversion */
    //StartConversion();

    InternalTemperatureSensorInit();
    /* start conversion */
  //  StartConversion();

    //int option = 0;

    /* Test 1 */
#ifdef TEST1
    int capacity = 7;
    stack_t *temperatureValues = StackCreate(capacity);

    for (int i = 0; i <= 2*capacity; ++i)
    {
        StackPush(temperatureValues, (i*5));
    }

    printf("Top element is %d\r\n", StackPeek(temperatureValues));
    printf("Stack size is %d\r\n", StackGetSize(temperatureValues));
    printf("\r\n");

    for (int i = 0; i <= 2*capacity; ++i)
    {
        printf("Stack popped element is %d\r\n", StackPop(temperatureValues));
    }

    if (StackIsEmpty(temperatureValues))
    {
        printf("Stack is empty\r\n");
    }
    else
    {
        printf("Stack is not empty\r\n");
    }
#endif

    /* Test 2 */
    int capacity = 5;
    stack_t *temperatureValues = StackCreate(capacity);

    for (int i = 0; i <= capacity; ++i)
    {
        StackPush(temperatureValues, ReadInternalTemperatureCelcius(VREF_PLUS_MV, &chipTemperature));
    }
    printf("Top element is %d\r\n", StackPeek(temperatureValues));
    printf("Stack size is %d\r\n", StackGetSize(temperatureValues));
    printf("\r\n");

   // printf("Hello again from STM32G431xx...\n\r");

    while (1)
    {
        //keep this
      //  option = PrintLinkedListMenu();
      //  LinkedListMenu(option);

        // buttonState = GetButtonState();
        // printf("button = %d\r\n", buttonState);
        // delay(500);
        //
        // sensorValue = AdcRead();
        // printf("pot = %ld\r\n", sensorValue);
        // delay(500);

        /* task: insert 3 temperatures each time the button is pressed */


        // if (ReadInternalTemperatureCelcius(VREF_PLUS_MV, &chipTemperature))
        // {
        //     printf("temp = %f\r\n", chipTemperature);
        // }
        //
        // delay(1000);
    }
}
