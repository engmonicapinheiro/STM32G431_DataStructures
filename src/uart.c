#include "uart.h"
#include "stm32g431xx.h"
#include <stdint.h>
#include "commands.h"

/* variables for the ring buffer for the LinkedList menu */
static volatile uint8_t RxBuffer[UART_RX_BUFFER_SIZE];
static volatile uint8_t RxHead = 0;
static volatile uint8_t RxTail = 0;

static uint16_t ComputeUartBaudrate(uint32_t peripheralClk, uint32_t baudrate);
static void UartSetBaudrate(uint32_t peripheralClk, uint32_t baudrate);
static void UartWrite(int ch);

/*
 * PA2 -> TX
 * PA3 -> RX
 */
void UartInit(void)
{
    /* enable clock access to GPIOA */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    /* set the mode of PA2 to alternate function mode */
    GPIOA->MODER &= ~(GPIO_MODER_MODE2_0);
    GPIOA->MODER |= GPIO_MODER_MODE2_1;

    /* set alternate function type to AF7 (USART2 TX) */
    GPIOA->AFR[0] |= (7U << GPIO_AFRL_AFSEL2_Pos);

    /* set the mode of PA3 to alternate function mode */
    GPIOA->MODER &= ~(GPIO_MODER_MODE3_0);
    GPIOA->MODER |= GPIO_MODER_MODE3_1;

    /* set alternate function type to AF7 (USART2 RX) */
    GPIOA->AFR[0] |= (7U << GPIO_AFRL_AFSEL3_Pos);

    /* enable clock access to UART2 */
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    /* configure UART baudrate */
    UartSetBaudrate(APB1_CLOCK, UART_BAUDRATE_DEBUG);

    /* configure transfer direction (TX and RX) */
    USART2->CR1 |= USART_CR1_TE| USART_CR1_RE;

    /* enable RX interrupt and receive interrupt */
    USART2->CR1 |= USART_CR1_RXNEIE;

    /* enable interrupt in NVIC */
    NVIC_EnableIRQ(USART2_IRQn);

    /* enable the UART module */
    USART2->CR1 |= USART_CR1_UE;
}

static uint16_t ComputeUartBaudrate(uint32_t peripheralClk, uint32_t baudrate)
{
    return (peripheralClk + (baudrate/2U))/baudrate;
}

static void UartSetBaudrate(uint32_t peripheralClk, uint32_t baudrate)
{
    USART2->BRR = ComputeUartBaudrate(peripheralClk, baudrate);
}

static void UartWrite(int ch)
{
    /* make sure transmit data register is empty */
    while (!(USART2->ISR & USART_ISR_TXE)) {}
    /* write to transmit data register */
    USART2->TDR = (ch & 0xFF);
}

int _write(int fd, char *ptr, int len)
{
    for (int i = 0; i < len; i++)
    {
        UartWrite(ptr[i]);
    }
    return len;
}

int UartReadCharNonBlocking(uint8_t *ch)
{
    /* no data available */
    if (RxHead == RxTail)
    {
        return 0;
    }

    *ch = RxBuffer[RxTail];
    RxTail = (RxTail + 1U) & (UART_RX_BUFFER_SIZE - 1U);
    return 1;
}

uint8_t UartReadRingBuffer(void)
{
    uint8_t ch;
    while (!UartReadCharNonBlocking(&ch)) {}
    return ch;
}

int UartReadInt(void)
{
    uint8_t ch;
    int value = 0;
    int negative = 0;

    ch = UartReadRingBuffer();

    if (ch == '-')
    {
        negative = 1;
        ch = UartReadRingBuffer();
    }

    while (ch >= '0' && ch <= '9')
    {
        value = (value * 10) + (ch - '0');
        ch = UartReadRingBuffer();
    }

    return negative ? -value : value;
}


void USART2_IRQHandler(void)
{
    /* RXNEIE also enables the overrun interrupt. ORE is NOT cleared by
 * reading RDR, so if it is left set the IRQ re-enters forever and
 * main() never runs again. Clear it explicitly. */
    if (USART2->ISR & USART_ISR_ORE)
    {
        USART2->ICR = USART_ICR_ORECF;
    }

    if (USART2->ISR & USART_ISR_RXNE)
    {
        /* reading RDR also clears RXNE */
        uint8_t data = (uint8_t)(USART2->RDR & 0xFF);
        uint8_t nextHead = (RxHead + 1U) & (UART_RX_BUFFER_SIZE - 1U);

        /* only stores if the buffer is not full */
        if(nextHead != RxTail)
        {
            RxBuffer[RxHead] = data;
            RxHead = nextHead;
        }
    }
}

