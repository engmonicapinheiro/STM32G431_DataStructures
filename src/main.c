#include "stm32g431xx.h"
#include <stdio.h>
#include "fpu.h"
#include "uart.h"
#include "timebase.h"
#include "gpio.h"
#include "adc.h"
#include "rtc.h"
#include "stackLinkedList.h"
#include "queue.h"


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

    queue_t *temperatureValues = QueueCreate(5);
    QueueEnqueue(temperatureValues, 7);
    QueueEnqueue(temperatureValues, 14);
    QueueEnqueue(temperatureValues, 21);
    QueueEnqueue(temperatureValues, 28);

    printf("Queue size is %d\r\n", QueueGetSize(temperatureValues));
    printf("Front element: %d\r\n", QueueGetFrontElement(temperatureValues));
    printf("Dequeued element: %d\r\n", QueueDequeue(temperatureValues));
    printf("Queue new size is %d\r\n", QueueGetSize(temperatureValues));



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
