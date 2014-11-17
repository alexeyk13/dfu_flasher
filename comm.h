/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef COMM_H
#define COMM_H

#include "config.h"
#include <stdbool.h>

typedef struct {
    DRV drv;
    bool stop;
}COMM;

void comm_start();

#endif // COMM_H
