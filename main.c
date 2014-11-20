/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "core.h"
#include "board.h"
#include "config.h"
#include "comm.h"
#include "usbd.h"
#include "dfud.h"
#include "proto.h"
#include "protod.h"
#if DFU_DEBUG
#include "dbg.h"
#endif

void main() __attribute__ ((section (".loader")));

void flasher()
{
    COMM comm;
    board_init();
#if DFU_DEBUG
    board_dbg_init();
    printf("DFU Flasher %d.%d started...\n\r", VERSION >> 8, VERSION & 0xff);
    printf("DFU protocol version: %d.%d\n\r", PROTO_VERSION >> 8, PROTO_VERSION & 0xff);
#endif

    comm.stop = false;
    board_usb_init(&comm);
    usbd_init(&comm);
    dfud_init(&comm);
    protod_init(&comm);

    if (board_usb_start(&comm))
    {
#if DFU_DEBUG
        printf("Comm: USB started\n\r");
#endif
        while (!comm.stop)
            board_usb_request(&comm);

        board_usb_stop(&comm);
#if DFU_DEBUG
        printf("Comm: USB stopped\n\r");
#endif
    }
#if DFU_DEBUG
    else
        printf("Comm error: USB start error\n\r");
#endif

#if DFU_DEBUG
    printf("DFU Flasher reset\n\r");
#endif
    board_reset();
}

void main()
{
    //because core_init can change stack, all local vars must be defined after this call
    core_init();
    flasher();
    for (;;) {}
}
