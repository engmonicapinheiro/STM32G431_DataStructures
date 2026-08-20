#include "stm32g431xx.h"
#include <stdio.h>
#include "fpu.h"
#include "uart.h"
#include "timebase.h"
#include "gpio.h"
#include "adc.h"

uint32_t sensorValue;

int main()
{
    /* enable the FPU */
    FpuEnable();
    /* initialise the UART */
    UartInit();
    /* initialise timebase */
    TimebaseInit();
    /* initialise the LED */
    LedInit();
    /* initialise the button */
    ButtonInit();
    /* initialise the ADC */
    AdcInit();
    /* start conversion */
    StartConversion();


    while (1)
    {
        printf("Hello from STM32G431xx...\n\r");
        delay(1);
        sensorValue = AdcRead();
        printf("%lu\r\n", (unsigned long)sensorValue);

    }

}
