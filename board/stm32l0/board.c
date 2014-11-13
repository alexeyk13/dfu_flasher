/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "../../board.h"
#include "stm32.h"
#include "gpio.h"
#include "power.h"

void board_init()
{
    __disable_irq();
    power_init();
    gpio_set_pin(A5, true);
}

void board_reset()
{
    NVIC_SystemReset();
}
