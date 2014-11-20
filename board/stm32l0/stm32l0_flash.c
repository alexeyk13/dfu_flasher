/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "stm32l0_flash.h"
#include "../../board.h"
#include "../../dfu.h"
#include "config.h"
#include <string.h>

#if (PROTO_ENABLE_UPLOAD)
int board_flash_read(COMM* comm, unsigned int addr, char* buf, unsigned int size)
{
    memcpy(buf, (void*)addr, size);
    return DFU_STATUS_OK;
}
#endif


