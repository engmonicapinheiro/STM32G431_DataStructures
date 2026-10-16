#include "adc.h"
#include "stm32g431xx.h"
#include "timebase.h"
#include <stdio.h>
#include <stdbool.h>

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

    /* Exit deep power down, enable regulator */
    ADC2->CR &= ~ADC_CR_DEEPPWD;
    ADC2->CR |= ADC_CR_ADVREGEN;
    delay(10);

    /* Calibrate (single-ended) */
    ADC2->CR &= ~ADC_CR_ADCALDIF;
    ADC2->CR |= ADC_CR_ADCAL;
    while (ADC2->CR & ADC_CR_ADCAL) {}

    /* Sample time ch2: 47.5 cycles (0b100) */
    ADC2->SMPR1 = (ADC2->SMPR1 & ~ADC_SMPR1_SMP2_Msk) | (4U << ADC_SMPR1_SMP2_Pos);

    /* Select channel 2 as first conversion in regular sequence */
    ADC2->SQR1 |= (2U << ADC_SQR1_SQ1_Pos);

    /*set conversion sequence length */
    ADC2->SQR1 &= ~ADC_SQR1_L_Msk;

    /* enable the ADC */
    ADC2->ISR |= ADC_ISR_ADRDY;
    ADC2->CR |= ADC_CR_ADEN;

    /* Wait until ADC is ready */
    while (!(ADC2->ISR & ADC_ISR_ADRDY)) {}
}

void StartConversion(void)
{
    /* CFGR must only be written while ADSTART = 0 */
    ADC2->CFGR |= ADC_CFGR_CONT | ADC_CFGR_OVRMOD;  /* continuous + overwrite on overrun */
    ADC2->CR   |= ADC_CR_ADSTART;

}

uint32_t AdcRead(void)
{
    /* conversions run freely; wait for the first one, then DR always holds the latest */
    while (!(ADC2->ISR & ADC_ISR_EOC)) {}
    return ADC2->DR;   /* reading DR clears EOC; the next conversion sets it again */
}


/* configure timer2 to generate a 1Hz output
 * to trigger the temperature sensor ADC conversion.
 * The internal temperature sensor is in channel17.
 * Sysclock = TIM2 clock source = 16MHz = 16 000 000 cycles/second
 * 16 000 000 / 1600 = 10 000
 */
void InternalTemperatureSensorInit(void)
{
    /*** ADC1 clock + GPIO not needed (internal channel) ***/
    RCC->AHB2ENR |= RCC_AHB2ENR_ADC12EN;

    /* synchronous clock HCLK/1 (shared by ADC1/ADC2) */
    ADC12_COMMON->CCR = (ADC12_COMMON->CCR & ~ADC_CCR_CKMODE) | ADC_CCR_CKMODE;

    /* exit deep power-down, enable regulator */
    ADC1->CR &= ~ADC_CR_DEEPPWD;
    ADC1->CR |= ADC_CR_ADVREGEN;
    delay(10);

    /* calibrate (single-ended) */
    ADC1->CR &= ~ADC_CR_ADCALDIF;
    ADC1->CR |= ADC_CR_ADCAL;
    while ((ADC1->CR & ADC_CR_ADCAL) != 0U) {}

    /*
     * CFGR = 0:
     *   12-bit, right aligned
     *   software trigger
     *   single conversion, no continuous/discontinuous operation
     *   no DMA or automatic wait
     *
     * CFGR2 = 0: oversampling disabled.
     */
    ADC1->IER   = 0U;
    ADC1->CFGR  = 0U;
    ADC1->CFGR2 = 0U;

    /* sensor startup */
    /*
         * Channel 16 sampling time = 640.5 ADC clock cycles.
         * SMP16 = 0b111.
         *
         * This exceeds the temperature sensor's minimum sampling time
         * with the selected HCLK/4 clock, including at HCLK = 170 MHz.
         */
    ADC1->SMPR2 = (ADC1->SMPR2 & ~ADC_SMPR2_SMP16) | (7UL << ADC_SMPR2_SMP16_Pos);

    /*
         * Regular sequence:
         *   L = 0: one conversion
         *   SQ1 = 16: internal temperature sensor
         */
    ADC1->SQR1 = 16UL << ADC_SQR1_SQ1_Pos;

    /* enable temperature sensor, disable VBAT */
    ADC12_COMMON->CCR |= ADC_CCR_VSENSESEL;
    ADC12_COMMON->CCR &= ~ADC_CCR_VBATSEL;
    delay(1);

    /*
     * trigger: SOFTWARE (EXTEN = 0), single conversion, overwrite on overrun.
     *
     * temperature_read_c() starts each conversion itself and returns false
     * if ADSTART is already set. With a hardware trigger, ADSTART stays set
     * permanently once armed, so temperature_read_c() always returned false
     * and nothing was ever printed in the main loop.
     */
    ADC1->CFGR &= ~(ADC_CFGR_CONT | ADC_CFGR_EXTSEL_Msk | ADC_CFGR_EXTEN_Msk);
    ADC1->CFGR |= ADC_CFGR_OVRMOD;

    /* enable ADC */
    ADC1->ISR = ADC_ISR_ADRDY;
    ADC1->CR |= ADC_CR_ADEN;
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0U) {}

    /* do NOT set ADSTART here: temperature_read_c() starts each conversion */

    /*** TIM2: 16 MHz / 1600 = 10 kHz, / 10000 = 1 Hz ***/
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    TIM2->PSC  = 1600 - 1;
    TIM2->ARR  = 10000 - 1;
    TIM2->CCR2 = 50 - 1;
    /* PWM mode 1 (OC2M=110) + CCR2 preload */
    TIM2->CCMR1 = (6U << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE;
    TIM2->EGR   = TIM_EGR_UG;                   /* load PSC/ARR, reset CNT */
    TIM2->CR1   = TIM_CR1_CEN;                  /* start last */
}


/*
 * Read die temperature in degrees Celsius.
 *
 * vref_plus_mv:
 *   Actual ADC reference voltage at VREF+, in millivolts.
 *   Pass VDDA only if VREF+ is connected to VDDA.
 *
 * temperature_c:
 *   Output pointer. Unchanged if the function returns false.
 *
 * Returns:
 *   true  - conversion completed and temperature was calculated
 *   false - invalid argument, ADC not ready, ADC busy, or invalid
 *           factory calibration values
 *
 * Blocking: hardware polling loops have no timeout.
 */
bool ReadInternalTemperatureCelcius(uint32_t vref_plus_mv, float *temperature_c)
{
    if ((temperature_c == NULL) || (vref_plus_mv == 0U)) {
        return false;
    }

    if (((ADC1->CR & ADC_CR_ADEN) == 0U) ||
        ((ADC1->ISR & ADC_ISR_ADRDY) == 0U) ||
        ((ADC1->CR & ADC_CR_ADSTART) != 0U)) {
        return false;
        }

    const uint16_t cal30  = TEMP_CAL_30C;
    const uint16_t cal130 = TEMP_CAL_130C;

    if ((cal30 == 0U) || (cal130 == 0U) ||
        (cal30 > 4095U) || (cal130 > 4095U) ||
        (cal30 == cal130)) {
        return false;
        }

    /* Clear stale conversion flags before starting a new conversion. */
    ADC1->ISR = ADC_ISR_EOC | ADC_ISR_EOS | ADC_ISR_OVR;

    ADC1->CR |= ADC_CR_ADSTART;

    while ((ADC1->ISR & ADC_ISR_EOC) == 0U) {
    }

    /* Reading DR clears EOC. */
    const uint16_t raw = (uint16_t)ADC1->DR;

    /* Ensure the single-conversion sequence has fully stopped. */
    while ((ADC1->CR & ADC_CR_ADSTART) != 0U) {
    }

    /*
     * Scale the measurement to the factory calibration reference
     * voltage, then interpolate between the calibration points.
     *
     * Cast before subtraction to avoid unsigned underflow.
     */
    const float raw_at_3v = (float)raw * ((float)vref_plus_mv / 3000.0f);

    *temperature_c = 30.0f + (raw_at_3v - (float)cal30) * (130.0f - 30.0f) / ((float)cal130 - (float)cal30);

    return true;
}