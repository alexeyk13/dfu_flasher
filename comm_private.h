/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef COMM_PRIVATE_H
#define COMM_PRIVATE_H

#include "config.h"
#include <stdbool.h>
#include "usbd.h"
#include BOARD_USB

typedef struct _COMM {
    DRV drv;
    USBD usbd;
    SETUP setup;
    bool stop;
}COMM;

#endif // COMM_PRIVATE_H
