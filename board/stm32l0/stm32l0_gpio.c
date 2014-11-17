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


#include "stm32l0_gpio.h"
#include "stm32.h"

#define GPIO_PORT(pin)                                          (pin / 16)
#define GPIO_PIN(pin)                                           (pin & 15)

typedef GPIO_TypeDef* GPIO_TypeDef_P;
const GPIO_TypeDef_P GPIO[];

const GPIO_TypeDef_P GPIO[8] =                                  {GPIOA, GPIOB, GPIOC, GPIOD, 0, 0, 0, GPIOH};

#define GPIO_SET_MODE(pin, mode)                                GPIO[GPIO_PORT(pin)]->MODER &= ~(3 << (GPIO_PIN(pin) * 2)); \
                                                                GPIO[GPIO_PORT(pin)]->MODER |= ((mode) << (GPIO_PIN(pin) * 2))

#define GPIO_SET_OT(pin, mode)                                  GPIO[GPIO_PORT(pin)]->OTYPER &= ~(1 << GPIO_PIN(pin)); \
                                                                GPIO[GPIO_PORT(pin)]->OTYPER |= ((mode) << GPIO_PIN(pin))

#define GPIO_SET_SPEED(pin, mode)                               GPIO[GPIO_PORT(pin)]->OSPEEDR &= ~(3 << (GPIO_PIN(pin) * 2)); \
                                                                GPIO[GPIO_PORT(pin)]->OSPEEDR |= ((mode) << (GPIO_PIN(pin) * 2))

#define GPIO_SET_PUPD(pin, mode)                                GPIO[GPIO_PORT(pin)]->PUPDR &= ~(3 << (GPIO_PIN(pin) * 2)); \
                                                                GPIO[GPIO_PORT(pin)]->PUPDR |= ((mode) << (GPIO_PIN(pin) * 2))

#define GPIO_AFR(pin)                                           (*((unsigned int*)((unsigned int)(GPIO[GPIO_PORT(pin)]) + 0x20 + 4 * ((GPIO_PIN(pin)) / 8))))
#define GPIO_AFR_SET(pin, mode)                                 GPIO_AFR(pin) &= ~(0xful << ((GPIO_PIN(pin) % 8) * 4ul)); \
                                                                GPIO_AFR(pin) |= ((unsigned int)(mode) << ((GPIO_PIN(pin) % 8) * 4ul))

void gpio_enable_pin(PIN pin, unsigned int mode, AF af)
{
    RCC->IOPENR |= 1 << GPIO_PORT(pin);
    GPIO_SET_MODE(pin, (mode >> 0) & 3);
    GPIO_SET_OT(pin, (mode >> 2) & 1);
    GPIO_SET_SPEED(pin, (mode >> 3) & 3);
    GPIO_SET_PUPD(pin, (mode >> 5) & 3);
    GPIO_AFR_SET(pin, af);
}

void gpio_set_pin(PIN pin, bool set)
{
	if (set)
        GPIO[GPIO_PORT(pin)]->BSRR = 1 << GPIO_PIN(pin);
	else
        GPIO[GPIO_PORT(pin)]->BRR = 1 << GPIO_PIN(pin);
}

bool gpio_get_pin(PIN pin)
{
    return (GPIO[GPIO_PORT(pin)]->IDR >> GPIO_PIN(pin)) & 1;
}
