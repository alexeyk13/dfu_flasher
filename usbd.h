/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2014, Alexey Kramarenko
    All rights reserved.
*/
/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef USBD_H
#define USBD_H

#include "comm.h"
#include "usb.h"

typedef enum {
    USBD_STATE_DEFAULT = 0,
    USBD_STATE_ADDRESSED,
    USBD_STATE_CONFIGURED
} USBD_STATE;

typedef enum {
    USB_SETUP_STATE_REQUEST = 0,
    USB_SETUP_STATE_DATA_IN,
    //in case response is less, than request
    USB_SETUP_STATE_DATA_IN_ZLP,
    USB_SETUP_STATE_DATA_OUT,
    USB_SETUP_STATE_STATUS_IN,
    USB_SETUP_STATE_STATUS_OUT
} USB_SETUP_STATE;

typedef struct {
    //SETUP state machine
    SETUP setup;
    USB_SETUP_STATE setup_state;
    // USBD state machine
    USBD_STATE state;
} USBD;

void usbd_init(COMM* comm);
void usbd_reset(COMM* comm);
void usbd_suspend(COMM* comm);
void usbd_wakeup(COMM* comm);

#endif // USBD_H
