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
    comm.stop = false;
    board_usb_init(&comm);

    while (!comm.stop)
    {
        if (!board_usb_start(&comm))
        {
    #if DFU_DEBUG
            printf("Comm error: USB start error\n\r");
    #endif
            return;
        }
    #if DFU_DEBUG
        printf("Comm: USB started\n\r");
    #endif

        while (!comm.stop && board_usb_request(&comm)) {}

#if DFU_DEBUG
        if (!comm.stop)
            printf("Comm error: USB event failure, restarting\n\r");
#endif
        board_usb_stop(&comm);
#if DFU_DEBUG
        printf("Comm: USB stopped\n\r");
#endif
    }

}
