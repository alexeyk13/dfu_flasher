/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef BOARD_H
#define BOARD_H

#include "comm.h"
#include "usb.h"
#include <stdbool.h>

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
extern void board_usb_request(COMM* comm);
void board_usb_open_ep(COMM* comm, int num, USB_EP_TYPE type, int size);
void board_usb_close_ep(COMM* comm, int num);
void board_usb_flush_ep(COMM* comm, int num);

#endif // BOARD_H
