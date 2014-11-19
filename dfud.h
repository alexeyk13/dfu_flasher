/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef DFUD_H
#define DFUD_H

#include "defs.h"
#include "config.h"

typedef struct {
    int status, state;
    char buf[USBD_BUF_SIZE];
    int size;
}DFUD;

void dfud_init(COMM* comm);

#endif // DFUD_H
