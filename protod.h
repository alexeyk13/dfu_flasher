/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef PROTOD_H
#define PROTOD_H

#include "defs.h"

typedef struct {
    int cmd;
} PROTOD;

int protod_tx(COMM* comm, int* size);
int protod_rx(COMM* comm);

void protod_init(COMM* comm);

#endif // PROTOD_H