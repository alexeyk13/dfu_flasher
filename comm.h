/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef COMM_H
#define COMM_H

#include "config.h"

typedef struct {
    USB usb;
}COMM;

void comm_start();

#endif // COMM_H
