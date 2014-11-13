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

#include "power.h"
#include "stm32.h"
#include "stm32l0_config.h"

#define HSI_VALUE                           16000000
#define HSE_STARTUP_TIMEOUT                 50000

//exactly 96 MHz is required for USB
#define PLL_MUL                             (96000000 / (HSE_VALUE))
#define PLL_DIV                             3

static inline void setup_pll()
{
#if (HSE_VALUE)
    if (RCC->CR & RCC_CR_HSEON)
    {
        unsigned int pow, mul;
        mul = PLL_MUL;
        for (pow = 0; mul > 4; mul /= 2, pow++) {}
        RCC->CFGR |= (((PLL_DIV - 1) << 22) | ((pow << 1) | (mul - 3)) << 18) | (1 << 16);
    }
    else
#endif
        RCC->CFGR |= (((PLL_DIV - 1) << 22) | 2 << 18);
}

void power_init()
{
    RCC->APB1ENR = 0;
    RCC->APB2ENR = 0;
    RCC->AHBENR = 0;

    //make sure MSI is on before switch
    RCC->CR |= RCC_CR_MSION;
    while ((RCC->CR & RCC_CR_MSIRDY) == 0) {}

    //1. switch to internal RC(MSI)
    RCC->CFGR = 0;
    while (RCC->CFGR & (3 << 2)) {}

    //2. try to turn HSE on (if not already, and if HSE_VALUE is set)
#if (HSE_VALUE)
    if ((RCC->CR & RCC_CR_HSEON) == 0)
    {

#if (HSE_BYPASS)
        RCC->CR |= RCC_CR_HSEON | RCC_CR_HSEBYP;
#else
        RCC->CR |= RCC_CR_HSEON;
#endif
        int i;
        for (i = 0; i < HSE_STARTUP_TIMEOUT; ++i)
            if (RCC->CR & RCC_CR_HSERDY)
                break;
    }
#endif
    //on HSE failure or not set - switch to HSI
    if ((RCC->CR & (RCC_CR_HSION | RCC_CR_HSERDY)) == 0)
    {
        RCC->CR |= RCC_CR_HSION;
        while ((RCC->CR & RCC_CR_HSIRDY) == 0) {}
    }

    //3. setup pll
    RCC->CR &= ~RCC_CR_PLLON;

    setup_pll();
    //turn PLL on
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}

    //4. setup bases - on STM32L0 APB1/APB2 can operates at maximum speed

    //5. tune flash latency
    FLASH->ACR = FLASH_ACR_PRE_READ | 1;
    //6. switch to PLL
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & (3 << 2)) != RCC_CFGR_SWS_PLL) {}
}
