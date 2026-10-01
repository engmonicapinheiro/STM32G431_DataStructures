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
    /* initialise the ADC */
     AdcInit();
    /* start conversion */
    StartConversion();

    int option = 0;

    printf("Hello again from STM32G431xx...\n\r");

    while (1)
    {
        option = PrintLinkedListMenu();
        LinkedListMenu(option);

    }
}
