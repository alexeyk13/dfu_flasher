/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef DFUD_H
#define DFUD_H

#include "defs.h"

typedef struct {
    int status, state;
}DFUD;

void dfud_init(COMM* comm);

#endif // DFUD_H
