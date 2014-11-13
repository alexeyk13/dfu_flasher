/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2014, Alexey Kramarenko
    All rights reserved.
*/
/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "uart.h"
#include "stm32.h"
#include "gpio.h"
#include "power.h"
#include "stm32l0_config.h"

typedef USART_TypeDef* USART_TypeDef_P;

static const USART_TypeDef_P UART_REGS[UARTS_COUNT]=            {USART1, USART2};
static const PIN UART_TX_PINS[UARTS_COUNT] =                    {B6, PIN_UNUSED};
static const unsigned int UART_POWER_PINS[UARTS_COUNT] =        {14, 17};

void board_dbg_init()
{
    gpio_enable_pin(UART_TX, GPIO_MODE_AF | GPIO_OT_PUSH_PULL | GPIO_SPEED_HIGH, UART_TX == UART_TX_PINS[UART] ? AF0 : AF4);

    //power up
    if (UART == UART_1)
        RCC->APB2ENR |= 1 << UART_POWER_PINS[UART];
    else
        RCC->APB1ENR |= 1 << UART_POWER_PINS[UART];

    //disable core, if was enabled before, set 8N1
    UART_REGS[UART]->CR1 = 0;
    UART_REGS[UART]->CR2 = 0;
    UART_REGS[UART]->CR3 = 0;

    unsigned int mantissa, fraction;
    mantissa = (25 * BUS_CLOCK) / (4 * (UART_BAUD));
    fraction = ((mantissa % 100) * 8 + 25)  / 50;
    mantissa = mantissa / 100;
    UART_REGS[UART]->BRR = (mantissa << 4) | fraction;

    //enable core and transmitter
    UART_REGS[UART]->CR1 |= USART_CR1_UE | USART_CR1_TE;
}

void board_dbg(const char *const buf, unsigned int size)
{
    int i;
    for(i = 0; i < size; ++i)
    {
        while ((UART_REGS[UART]->ISR & USART_ISR_TXE) == 0) {}
        UART_REGS[UART]->TDR = buf[i];
    }
    while ((UART_REGS[UART]->ISR & USART_ISR_TC) == 0) {}
}
