#include "../../../3rdparty/CMSIS/Device/ST/STM32L0xx/Include/stm32l053xx.h"

void  main() __attribute__ ((section (".loader")));

void main()
{
    __disable_irq();
    __enable_irq();
}
