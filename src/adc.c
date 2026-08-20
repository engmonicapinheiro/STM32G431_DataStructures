#include "adc.h"
#include "stm32g431xx.h"

/* Using PA1 for ADC channel 2 */

void AdcInit(void)
{
    /*** configure the ADC GPIO pin ***/

    /* enable clock access to GPIOA */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    /* set PA1 mode to analog mode */
    GPIOA->MODER |= GPIO_MODER_MODE1_0;
    GPIOA->MODER |= GPIO_MODER_MODE1_1;

    /*** configure the ADC module ***/

    /* enable clock access to the ADC module */
    RCC->AHB2ENR |= RCC_AHB2ENR_ADC12EN;

    /*  set ADC clock mode. snchronous PCLK/1 or PCLK/2 */
    ADC12_COMMON->CCR &= ~ADC_CCR_CKMODE_Msk;
    ADC12_COMMON->CCR |= ADC_CCR_CKMODE_0;

    /* Select channel 2 as first conversion in regular sequence */
    ADC2->SQR1 |= (2U << ADC_SQR1_SQ1_Pos);

    /*set conversion sequence length */
    ADC2->SQR1 &= ~ADC_SQR1_L_Msk;

    /* enable the ADC */
    ADC2->CR |= ADC_CR_ADEN;

    /* Wait until ADC is ready */
    while (!(ADC2->ISR & ADC_ISR_ADRDY)) {}
}

void StartConversion(void)
{
    /* enable continuous conversion */
    ADC2->CFGR |= ADC_CFGR_CONT;
    /* start ADC conversion */
    ADC2->CR |= ADC_CR_ADSTART;

}

uint32_t AdcRead(void)
{
    /* wait for conversion to be complete */
    while (!(ADC2->ISR & ADC_ISR_EOC)){}
    /* read converted value */
    return ADC2->DR;
}