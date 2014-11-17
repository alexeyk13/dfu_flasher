/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "comm.h"
#include "board.h"
#include "config.h"
#if DFU_DEBUG
#include "dbg.h"
#endif

void comm_start()
{
    COMM comm;
    board_usb_init(&comm);

    if (!board_usb_start(&comm))
    {
#if DFU_DEBUG
        printf("Comm error: USB start error");
#endif
        return;
    }
}
