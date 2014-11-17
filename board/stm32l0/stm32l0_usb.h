/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef STM32L0_USB_H
#define STM32L0_USB_H

#include <stdint.h>
#include <stdbool.h>

//0 control, 1 for DFU class
#define USB_EP_COUNT_MAX            2

typedef struct {
    void* ptr;
    unsigned int size, processed;
    uint16_t mps;
    uint8_t io_active;
} EP;

typedef struct _USB {
  EP out[USB_EP_COUNT_MAX];
  EP in[USB_EP_COUNT_MAX];
  uint8_t addr;
  bool suspend;
} DRV;

#endif // STM32L0_USB_H
