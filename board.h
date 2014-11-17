/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef BOARD_H
#define BOARD_H

#include "comm.h"

//main
extern void board_init();
extern void board_reset();

//dbg
extern void board_dbg_init();
extern void board_dbg(const char *const buf, unsigned int size);

//USB
extern void board_usb_init(COMM* comm);
extern bool board_usb_start(COMM* comm);
extern void board_usb_stop(COMM* comm);
extern bool board_usb_request(COMM* comm);

#endif // BOARD_H
