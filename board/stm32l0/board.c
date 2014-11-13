/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "../../board.h"
#include "stm32.h"

void board_init()
{
    __disable_irq();
}

void board_reset()
{
    __enable_irq();
}
