#include "gpio.h"
#include "stm32g4xx.h"

void LedInit(void)
{
    /* enable clock access to GPIOB */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    /* set PB8 mode to output mode */
    GPIOB->MODER |= GPIO_MODER_MODE8_0;
    GPIOB->MODER &= ~(GPIO_MODER_MODE8_1);
}

void LedOn(void)
{
    /* set PB8 high */
    GPIOB->ODR |= GPIO_ODR_OD8;
}

void LedOff(void)
{
    /* set PB8 low */
    GPIOB->ODR &= ~(GPIO_ODR_OD8);
}

void ButtonInit(void)
{
    /* enable clock access to PORTA */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    /* set PA9 as input */
    GPIOA->MODER &= ~(GPIO_MODER_MODE9_Msk);
}

bool GetButtonState(void)
{
    /* check if button is pressed
     * button is active low */
    if(GPIOA->IDR & GPIO_IDR_ID9)
    {
        return false;
    }
        return true;
}