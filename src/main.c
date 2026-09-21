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

    printf("Hello again from STM32G431xx...\n\r");

    printf("***** Add Event test *****\n\r");

    /* simulate 'add_event' */
    AddEvent("PowerOn");
    delay(1000);
    AddEvent("SensorInit");
    delay(1000);

    printf("***** Print Event test *****\n\r");
    PrintEventList();
    delay(1000);

    /* simulate event handling */
    printf("***** Handle Event test *****\n\r");
    HandleUartCommand("add_event ButtonPressed");
    delay(1000);
    HandleUartCommand("print_event");
    delay(1000);
    HandleUartCommand("remove_event 3678");
    delay(1000);


    while (1)
    {

    }
}
