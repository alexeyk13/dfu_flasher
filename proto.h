/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef PROTO_H
#define PROTO_H

#include <stdint.h>

#pragma pack(push, 1)

#define PROTO_VERSION                       0x101

#define PROTO_CMD_READ                      'r'
#define PROTO_CMD_WRITE                     'w'
#define PROTO_CMD_ERASE                     'e'
#define PROTO_CMD_STATUS                    's'
#define PROTO_CMD_VERSION                   'v'
#define PROTO_CMD_LEAVE                     'l'

typedef struct {
    uint8_t cmd;
    uint8_t align;
    uint16_t data_size;
    uint32_t param1;
    uint32_t param2;
} PROTO_REQ;

typedef struct {
    uint16_t loader, protocol;
} PROTO_VERSION_RESP;


#pragma pack(pop)


#endif // PROTO_H
