/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef STM32L0_USB_H
#define STM32L0_USB_H

#include <stdint.h>

typedef struct {
    void* ptr;
    unsigned int size, processed;
    uint16_t mps;
    uint8_t io_active;
} EP;

typedef struct {
  EP* out[USB_EP_COUNT_MAX];
  EP* in[USB_EP_COUNT_MAX];
  uint8_t addr;
} USB;


//void board_usb_init(SHARED_USB_DRV* drv);
//bool stm32_usb_request(SHARED_USB_DRV* drv, IPC* ipc);

#endif // STM32L0_USB_H
