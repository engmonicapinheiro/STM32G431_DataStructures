#include "stm32g431xx.h"
#include <stdio.h>
#include <stdint.h>
#include "fpu.h"
#include "uart.h"
#include "timebase.h"
#include "gpio.h"
#include "adc.h"



/* bit masks for GPIO configuration */
#define GPIO_MODE_MASK  0x03  //2 bits for GPIO mode (0-3)
#define GPIO_STATE_MASK 0x04  //1 bit for GPIO state (ON/OFF)
#define GPIO_SPEED_MASK 0x38  //3 bits for GPIO speed (0-7)

/* GPIO modes */
#define GPIO_MODE_INPUT  0
#define GPIO_MODE_OUTPUT 1

/* GPIO state */
#define GPIO_OFF  0
#define GPIO_ON   1

/* GPIO speed */
#define GPIO_SPEED_LOW      0
#define GPIO_SPEED_MEDIUM   1
#define GPIO_SPEED_HIGH     2

/* LED configuration (PB8) */
#define LED_PORT  GPIOB
#define LED_PIN       8

/**
 * @brief packs GPIO configuration settings into a single byte.
 * @param mode: GPIO mode (0-3)
 * @param state: GPIO state (ON=1, OFF=0)
 * @param speed: GPIO speed (0-7)
 * @retval Packed 8-bit data representing the GPIO settings
 */
uint8_t PackGPIOSettings(uint8_t mode, uint8_t state, uint8_t speed)
{
    return(mode & 0x03) | ((state & 0x01) << 2) | ((speed & 0x07) << 3);
}

/**
 * @brief unpacks GPIO settings from a packed byte.
 * @param data: packed GPIO configuration byte
 */
void UnpackGPIOSettings(uint8_t data, uint8_t *mode, uint8_t *state, uint8_t *speed)
{
    *mode = data & GPIO_MODE_MASK;
    *state = (data & GPIO_STATE_MASK) >> 2;
    *speed = (data & GPIO_SPEED_MASK) >> 3;
}

/**
 * @brief configures the LED bassed on bit-packed GPIO settings.
 * @param packedData: packed GPIO settings
 */
void ConfigureLed(uint8_t packedData)
{
    uint8_t mode, state, speed;
    UnpackGPIOSettings(packedData, &mode, &state, &speed);

    /* enable GPIO clock for PORTB */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

    /* configure GPIO mode */
    if (mode == GPIO_MODE_OUTPUT)
    {
        GPIOB->MODER &= ~(3 << (LED_PIN * 2));   //clear mode bits
        GPIOB->MODER |= (1 << (LED_PIN * 2)); //set as output
        printf("LED configured as OUTPUT.\n\r");
    }
    else
    {
        GPIOB->MODER &= ~(3 << (LED_PIN * 2)); //set as input (default)
        printf("LED configured as INPUT.\n\r");
    }

    /* configure GPIO speed */
    GPIOB->OSPEEDR &= ~(3 << (LED_PIN * 2));  //clear speed bits
    GPIOB->OSPEEDR |= (speed << (LED_PIN * 2)); //set speed
    printf("LED speed set to level %u.\n\r", speed);

    /* set LED state */
    if (state == GPIO_ON)
    {
        GPIOB->ODR |= (1 << LED_PIN);
        printf("LED turned ON.\n\r");
    }
    else
    {
        GPIOB->ODR &= ~(1 << LED_PIN);
        printf("LED turned OFF>\n\r");
    }
}

/**
 * @brief toggles the LED state directly without reconfiguring other GPIO settings.
 * @param packedData: packed GPIO settings
 * @retval updated packed data with toggled LED state
 */
uint8_t ToggleLED(uint8_t packedData)
{
    uint8_t state = (packedData &GPIO_STATE_MASK) >> 2;  //extract current state
    state = !state;  //toggle LED state

    //update the packed data with the toggle state
    packedData = (packedData & ~GPIO_STATE_MASK) | (state << 2);

    //directly toggle the LED without reconfiguring mode or speed
    if (state == GPIO_ON)
    {
        GPIOB->ODR |= (1 << LED_PIN); //turn on the led
        printf("Led turned on.\n\r");
    }
    else
    {
        GPIOB->ODR &= ~(1 << LED_PIN);  //turn off the led
        printf("Led turned off.\n\r");
    }
    return packedData;
}


int main()
{
    /* enable the FPU */
    FpuEnable();
    /* initialise the UART */
    UartInit();
    /* initialise timebase */
    TimebaseInit();
    /* initialise the LED */
   // LedInit();
    /* initialise the button */
  //  ButtonInit();
    /* initialise the ADC */
   // AdcInit();
    /* start conversion */
 //   StartConversion();

    /* pack GPIO settings for LED (Mode: output = 1, State: ON = 1, Speed: Medium=1 */
    uint8_t ledSettings = PackGPIOSettings(GPIO_MODE_OUTPUT, GPIO_ON, GPIO_SPEED_HIGH);

    /* apply led configurations */
    ConfigureLed(ledSettings);

    /* unpack and display the settings */
    uint8_t mode, state, speed;
    UnpackGPIOSettings(ledSettings, &mode, &state, &speed);
    printf("Current LED config - Mode: %u, State: %s, Speed: %u\n", mode, state ? "ON" : "OFF", speed);


    while (1)
    {
        //printf("Hello again from STM32G431xx...\n\r");
        delay(1000);
        ledSettings = ToggleLED(ledSettings);
    }
}
