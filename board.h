/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef BOARD_H
#define BOARD_H

#include "defs.h"
#include "usb.h"
#include <stdbool.h>

//main
extern void board_init();
extern void board_reset();

//dbg
extern void board_dbg_init();
extern void board_dbg(const char *const buf, unsigned int size);

//delay
extern void delay_us(unsigned int us);
extern void delay_ms(unsigned int ms);

//USB device
extern void board_usb_init(COMM* comm);
extern bool board_usb_start(COMM* comm);
extern void board_usb_stop(COMM* comm);
extern void board_usb_request(COMM* comm);
extern void board_usb_set_address(COMM* comm, unsigned int address);

//USB endpoint
extern void board_usb_open_ep(COMM* comm, int num, USB_EP_TYPE type, int size);
extern void board_usb_close_ep(COMM* comm, int num);
extern void board_usb_flush_ep(COMM* comm, int num);
extern void board_usb_tx(COMM* comm, int num, char *data, int size);
extern void board_usb_rx(COMM* comm, int num, char* data, int size);
extern void board_usb_set_stall(COMM* comm, int num);
extern void board_usb_clear_stall(COMM* comm, int num);
extern bool board_usb_is_stall(COMM* comm, int num);

//flash
extern int board_flash_read(COMM* comm, unsigned int addr, char* buf, unsigned int size);
extern int board_flash_write(COMM* comm, unsigned int addr, const char* buf, unsigned int size);
extern int board_flash_erase(COMM* comm, unsigned int addr, unsigned int size);

#endif // BOARD_H
