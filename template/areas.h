/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef AREAS_H
#define AREAS_H

typedef struct {
    unsigned int addr;
    unsigned int size;
}AREA;

#define READ_AREAS_COUNT                1
const AREA __READ_AREAS[READ_AREAS_COUNT] = {
    {
        0x08000000,
        0x10000,
    }
};

#define WRITE_AREAS_COUNT                1
const AREA __WRITE_AREAS[WRITE_AREAS_COUNT] = {
    {
        0x08000000,
        0x0DF00,
    }
};

#endif // AREAS_H
