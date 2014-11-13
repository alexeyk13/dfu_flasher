/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "../../board.h"
#include "stm32.h"
#include "gpio.h"

void board_init()
{
    __disable_irq();
    gpio_set_pin(A5, true);
}

void board_reset()
{
    __enable_irq();
}
