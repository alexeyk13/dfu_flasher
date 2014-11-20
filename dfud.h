/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef DFUD_H
#define DFUD_H

#include "defs.h"
#include "config.h"
#include "proto.h"

typedef struct {
    int status, state;
    char buf[PAGE_BUF_SIZE + sizeof(PROTO_REQ)];
    int size;
}DFUD;

void dfud_init(COMM* comm);

#endif // DFUD_H
