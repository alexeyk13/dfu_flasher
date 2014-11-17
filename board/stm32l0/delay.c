/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "delay.h"
#include "stm32.h"

#define COEF_MS                 3800
#define COEF_US                 4

void delay_us(unsigned int us)
{
    unsigned int i;
    for (i = 0; i < COEF_US * us; ++i)
        __NOP();
}

void delay_ms(unsigned int ms)
{
    unsigned int i;
    for (i = 0; i < COEF_MS * ms; ++i)
        __NOP();
}
