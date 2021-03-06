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

#include "defs.h"
#include "usb.h"
#include "config.h"
#include "proto.h"
#include <stdbool.h>

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
    // SETUP state machine
    USB_SETUP_STATE setup_state;
    // USBD state machine
    USBD_STATE state;
    int configuration, iface, iface_alt;
    char buf[PAGE_BUF_SIZE + sizeof(PROTO_REQ)];
    bool pend_reset;
} USBD;

typedef enum {
    USBD_FEATURE_ENDPOINT_HALT = 0,
    USBD_FEATURE_DEVICE_REMOTE_WAKEUP,
    USBD_FEATURE_TEST_MODE,
    USBD_FEATURE_SELF_POWERED
} USBD_FEATURES;

//device functions for board
void usbd_init(COMM* comm);
void usbd_reset(COMM* comm);
void usbd_suspend(COMM* comm);
void usbd_wakeup(COMM* comm);
void usbd_setup(COMM* comm);
//endpoint functions for board
void usbd_tx_complete(COMM* comm);
void usbd_rx_complete(COMM* comm);

//functions for class driver
int usbd_tx(COMM* comm, const void* buf, unsigned int size);

//callbacks for class driver
extern void class_reset(COMM* comm);
extern void class_suspend(COMM* comm);
extern void class_wakeup(COMM* comm);
extern void class_configured(COMM* comm);
extern int class_setup(COMM* comm);

#endif // USBD_H
