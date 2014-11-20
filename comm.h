/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef COMM_H
#define COMM_H

#include "config.h"
#include <stdbool.h>
#include "usbd.h"
#include "dfud.h"
#include "protod.h"
#include BOARD_USB

typedef struct _COMM {
    DRV drv;
    USBD usbd;
    DFUD dfud;
    PROTOD protod;
    SETUP setup;
    bool stop;
}COMM;

#endif // COMM_H
