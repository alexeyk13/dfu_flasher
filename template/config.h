/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef CONFIG_H
#define CONFIG_H


//------------------------------ board ------------------------------------------------
#define BOARD_USB                                  "board/stm32l0/stm32l0_usb.h"
#define CORTEX_M0
#define SP_PTR                                      0x20002000
#define DELAY_COEF_MS                               10000
#define DELAY_COEF_US                               10

//------------------------------ debug ------------------------------------------------
//enable debug (generally, by UART)
#define DFU_DEBUG                                   0

//------------------------------- USB -------------------------------------------------
#define USB_DEBUG_ERRORS                            0
#define USB_DEBUG_REQUESTS                          0
#define USB_DEBUG_CLASS_REQUESTS                    0
//at least 1 page size
#define PAGE_BUF_SIZE                               128

//---------------------------- protocol -----------------------------------------------
#define PROTO_DEBUG                                 0
//enable flash memory reading from device
#define PROTO_ENABLE_UPLOAD                         0

#endif // STM32L0_CONFIG_H
