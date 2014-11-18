/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "../../board.h"
#include "stm32.h"
#include "stm32l0_config.h"
#include "stm32l0_gpio.h"
#include "stm32l0_power.h"
#if (ENABLE_WDT)
#include "stm32l0_wdt.h"
#endif

void board_init()
{
#if (ENABLE_WDT)
    wdt_init();
#endif
    power_init();
    gpio_set_pin(A5, true);
}

void board_reset()
{
    NVIC_SystemReset();
}
