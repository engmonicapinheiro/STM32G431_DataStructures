#include "stm32g431xx.h"
#include <stdio.h>
#include "fpu.h"
#include "uart.h"
#include "timebase.h"
#include "gpio.h"


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


    while (1)
    {
        //printf("Hello from STM32G431xx...\n\r");
       // delay(1);
        if (GetButtonState())
        {
            LedOn();
        }
        else
        {
            LedOff();
        }

    }

}
