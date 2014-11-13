#include "wdt.h"
#include "stm32.h"

#define KICK_KEY                                        0xaaaa
#define WRITE_ENABLE_KEY                                0x5555
#define START_KEY                                       0xcccc

void wdt_init()
{
    //check/turn on LSI
    RCC->CSR |= RCC_CSR_LSION;
    while ((RCC->CSR & RCC_CSR_LSIRDY) == 0) {}
    //set prescaller to max. Just in case of flash timeouts
    IWDG->KR = WRITE_ENABLE_KEY;
    IWDG->PR = 7;
    IWDG->KR = WRITE_ENABLE_KEY;
    IWDG->RLR = 0xfff;

    //start WDT
    IWDG->KR = START_KEY;
    IWDG->KR = KICK_KEY;
}

void wdt_kick()
{
    IWDG->KR = KICK_KEY;
}
