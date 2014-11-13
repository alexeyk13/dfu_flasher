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

#include "stm32.h"
#include "stm32l0_config.h"

typedef USART_TypeDef* USART_TypeDef_P;

//static const unsigned int UART_POWER_PINS[UARTS_COUNT] =    {14, 17};
//static const PIN UART_TX_PINS[UARTS_COUNT] =                {B6, PIN_UNUSED};
//static const PIN UART_RX_PINS[UARTS_COUNT] =                {B7, PIN_UNUSED};


void board_dbg_init()
{

}

void board_dbg(const char *const buf, unsigned int size)
{
    int i;
    UART->CR1 |= USART_CR1_TE;
    for(i = 0; i < size; ++i)
    {
        while ((UART->ISR & USART_ISR_TXE) == 0) {}
        UART->TDR = buf[i];
    }
}

/*
void stm32_uart_set_baudrate_internal(SHARED_UART_DRV* drv, UART_PORT port, const BAUD* config)
{
    if (port >= UARTS_COUNT)
    {
        error(ERROR_INVALID_PARAMS);
        return;
    }
    if (drv->uart.uarts[port] == NULL)
    {
        error(ERROR_NOT_ACTIVE);
        return;
    }
    unsigned int clock;
    UART_REGS[port]->CR1 &= ~USART_CR1_UE;

    if (config->data_bits == 8 && config->parity != 'N')
#if defined(STM32L0)
        UART_REGS[port]->CR1 |= USART_CR1_M_0;
#else
        UART_REGS[port]->CR1 |= USART_CR1_M;
#endif
    else
#if defined(STM32L0)
        UART_REGS[port]->CR1 &= ~USART_CR1_M_0;
#else
        UART_REGS[port]->CR1 &= ~USART_CR1_M;
#endif

    if (config->parity != 'N')
    {
        UART_REGS[port]->CR1 |= USART_CR1_PCE;
        if (config->parity == 'O')
            UART_REGS[port]->CR1 |= USART_CR1_PS;
        else
            UART_REGS[port]->CR1 &= ~USART_CR1_PS;
    }
    else
        UART_REGS[port]->CR1 &= ~USART_CR1_PCE;

    UART_REGS[port]->CR2 = (config->stop_bits == 1 ? 0 : 2) << 12;
    UART_REGS[port]->CR3 = 0;

    if (port == UART_1 || port == UART_6)
        clock = get_clock(drv, STM32_CLOCK_APB2);
    else
        clock = get_clock(drv, STM32_CLOCK_APB1);
    unsigned int mantissa, fraction;
    mantissa = (25 * clock) / (4 * (config->baud));
    fraction = ((mantissa % 100) * 8 + 25)  / 50;
    mantissa = mantissa / 100;
    UART_REGS[port]->BRR = (mantissa << 4) | fraction;

    UART_REGS[port]->CR1 |= USART_CR1_UE | USART_CR1_PEIE;
    UART_REGS[port]->CR3 |= USART_CR3_EIE;
}
*/

/*
void stm32_uart_open_internal(SHARED_UART_DRV* drv, UART_PORT port, UART_ENABLE* ue)
{
    if (port >= UARTS_COUNT)
    {
        error(ERROR_INVALID_PARAMS);
        return;
    }
    if (drv->uart.uarts[port] != NULL)
    {
        error(ERROR_ALREADY_CONFIGURED);
        return;
    }
    drv->uart.uarts[port] = malloc(sizeof(UART));
    if (drv->uart.uarts[port] == NULL)
    {
        error(ERROR_OUT_OF_MEMORY);
        return;
    }
    drv->uart.uarts[port]->tx_pin = ue->tx;
    drv->uart.uarts[port]->rx_pin = ue->rx;
    drv->uart.uarts[port]->error = ERROR_OK;
    drv->uart.uarts[port]->tx_stream = INVALID_HANDLE;
    drv->uart.uarts[port]->tx_handle = INVALID_HANDLE;
    drv->uart.uarts[port]->rx_stream = INVALID_HANDLE;
    drv->uart.uarts[port]->rx_handle = INVALID_HANDLE;
    drv->uart.uarts[port]->tx_total = 0;
    drv->uart.uarts[port]->tx_chunk_pos = drv->uart.uarts[port]->tx_chunk_size = 0;
    if (drv->uart.uarts[port]->tx_pin == PIN_DEFAULT)
        drv->uart.uarts[port]->tx_pin = UART_TX_PINS[port];
    if (drv->uart.uarts[port]->rx_pin == PIN_DEFAULT)
        drv->uart.uarts[port]->rx_pin = UART_RX_PINS[port];

    if (drv->uart.uarts[port]->tx_pin != PIN_UNUSED)
    {
        drv->uart.uarts[port]->tx_stream = stream_create(ue->stream_size);
        if (drv->uart.uarts[port]->tx_stream == INVALID_HANDLE)
        {
            free(drv->uart.uarts[port]);
            drv->uart.uarts[port] = NULL;
            return;
        }
        drv->uart.uarts[port]->tx_handle = stream_open(drv->uart.uarts[port]->tx_stream);
        if (drv->uart.uarts[port]->tx_handle == INVALID_HANDLE)
        {
            stream_destroy(drv->uart.uarts[port]->tx_stream);
            free(drv->uart.uarts[port]);
            drv->uart.uarts[port] = NULL;
            return;
        }
        stream_listen(drv->uart.uarts[port]->tx_stream, (void*)HAL_HANDLE(HAL_UART, port));
    }
    if (drv->uart.uarts[port]->rx_pin != PIN_UNUSED)
    {
        drv->uart.uarts[port]->rx_stream = stream_create(ue->stream_size);
        if (drv->uart.uarts[port]->rx_stream == INVALID_HANDLE)
        {
            stream_close(drv->uart.uarts[port]->tx_handle);
            stream_destroy(drv->uart.uarts[port]->tx_stream);
            free(drv->uart.uarts[port]);
            drv->uart.uarts[port] = NULL;
            return;
        }
        drv->uart.uarts[port]->rx_handle = stream_open(drv->uart.uarts[port]->rx_stream);
        if (drv->uart.uarts[port]->rx_handle == INVALID_HANDLE)
        {
            stream_destroy(drv->uart.uarts[port]->rx_stream);
            stream_close(drv->uart.uarts[port]->tx_handle);
            stream_destroy(drv->uart.uarts[port]->tx_stream);
            free(drv->uart.uarts[port]);
            drv->uart.uarts[port] = NULL;
            return;
        }
        drv->uart.uarts[port]->rx_free = stream_get_free(drv->uart.uarts[port]->rx_stream);
    }

    //setup pins
#if defined(STM32F1)
    //turn on remapping
    if (((drv->uart.uarts[port]->tx_pin != UART_TX_PINS[port]) && (drv->uart.uarts[port]->tx_pin != PIN_UNUSED)) ||
        ((drv->uart.uarts[port]->rx_pin != UART_RX_PINS[port]) && (drv->uart.uarts[port]->rx_pin != PIN_UNUSED)))
    {
        ack_gpio(drv, STM32_GPIO_ENABLE_AFIO, 0, 0, 0);
        switch (drv->uart.uarts[port]->tx_pin)
        {
        case B6:
            AFIO->MAPR |= AFIO_MAPR_USART1_REMAP;
            break;
        case D5:
            AFIO->MAPR |= AFIO_MAPR_USART2_REMAP;
            break;
        case C10:
            AFIO->MAPR |= AFIO_MAPR_USART3_REMAP_PARTIALREMAP;
            break;
        case D8:
            AFIO->MAPR |= AFIO_MAPR_USART3_REMAP_FULLREMAP;
            break;
        default:
            error(ERROR_NOT_SUPPORTED);
            free(drv->uart.uarts[port]);
            drv->uart.uarts[port] = NULL;
            return;
        }
    }
#endif
    if (drv->uart.uarts[port]->tx_pin != PIN_UNUSED)
    {
#if defined(STM32F1)
        ack_gpio(drv, STM32_GPIO_ENABLE_PIN_SYSTEM, drv->uart.uarts[port]->tx_pin, GPIO_MODE_OUTPUT_AF_PUSH_PULL_50MHZ, false);
#elif defined(STM32F2) || defined(STM32F4)
        ack_gpio(drv, STM32_GPIO_ENABLE_PIN_SYSTEM, drv->uart.uarts[port]->tx_pin, GPIO_MODE_AF | GPIO_OT_PUSH_PULL |  GPIO_SPEED_HIGH, drv->uart.uarts[port]->port < UART_4 ? AF7 : AF8);
#elif defined(STM32L0)
        ack_gpio(drv, STM32_GPIO_ENABLE_PIN_SYSTEM, drv->uart.uarts[port]->tx_pin, GPIO_MODE_AF | GPIO_OT_PUSH_PULL |  GPIO_SPEED_HIGH, drv->uart.uarts[port]->tx_pin == UART_TX_PINS[port] ? AF0 : AF4);
#endif
    }

    if (drv->uart.uarts[port]->rx_pin != PIN_UNUSED)
    {
#if defined(STM32F1)
        ack_gpio(drv, STM32_GPIO_ENABLE_PIN_SYSTEM, drv->uart.uarts[port]->rx_pin, GPIO_MODE_INPUT_FLOAT, false);
#elif defined(STM32F2) || defined(STM32F4)
        ack_gpio(drv, STM32_GPIO_ENABLE_PIN_SYSTEM, drv->uart.uarts[port]->rx_pin, , GPIO_MODE_AF | GPIO_SPEED_HIGH, drv->uart.uarts[port]->port < UART_4 ? AF7 : AF8);
#elif defined(STM32L0)
        ack_gpio(drv, STM32_GPIO_ENABLE_PIN_SYSTEM, drv->uart.uarts[port]->rx_pin, GPIO_MODE_AF | GPIO_SPEED_HIGH, drv->uart.uarts[port]->rx_pin == UART_RX_PINS[port] ? AF0 : AF4);
#endif
    }
    //power up
    if (port == UART_1 || port == UART_6)
        RCC->APB2ENR |= 1 << UART_POWER_PINS[port];
    else
        RCC->APB1ENR |= 1 << UART_POWER_PINS[port];

    //enable core
    UART_REGS[port]->CR1 |= USART_CR1_UE;
    //enable receiver
    if (drv->uart.uarts[port]->rx_pin != PIN_UNUSED)
        UART_REGS[port]->CR1 |= USART_CR1_RE | USART_CR1_RXNEIE;

    stm32_uart_set_baudrate_internal(drv, port, &ue->baud);
    //enable interrupts
    irq_register(UART_VECTORS[port], stm32_uart_on_isr, (void*)drv);
    NVIC_EnableIRQ(UART_VECTORS[port]);
    NVIC_SetPriority(UART_VECTORS[port], 13);
}
*/
