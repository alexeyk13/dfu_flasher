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

#ifndef STM32L0_CONFIG_H
#define STM32L0_CONFIG_H

//------------------------------ POWER -----------------------------------------------
//0 meaning HSI. If not defined, 25MHz will be defined by default by ST lib
#define HSE_VALUE                               8000000
#define HSE_BYPASS                              1

#define PLL_MUL                                 12
#define PLL_DIV                                 3
//------------------------------ UART ------------------------------------------------
//Use UART as default stdio
#define UART_STDIO                              1
//PIN_DEFAULT and PIN_UNUSED can be also set.
#define UART                                    UART_1
#define UART_TX                                 A9
#define UART_BAUD                               115200
#define UART_DATA_BITS                          8
#define UART_PARITY                             'N'
#define UART_STOP_BITS                          1

#endif // STM32L0_CONFIG_H
