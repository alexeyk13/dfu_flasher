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

#include "usbd.h"
#include "comm.h"
#include "board.h"
#include "usb_desc.h"
#if (DFU_DEBUG)
#include "dbg.h"
#endif
#include <string.h>

#define USB_EP0_SIZE                                 64

#pragma pack(push, 1)

typedef struct {
    uint8_t idx;
    uint16_t lang;
    USB_STRING_DESCRIPTOR_TYPE str;
}STRING_DESCRIPTOR_ITEM;

#pragma pack(pop)

int usbd_tx(COMM* comm, const void *buf, unsigned int size)
{
    if (size > USBD_BUF_SIZE)
        size = USBD_BUF_SIZE;
    memcpy(comm->usbd.buf, buf, size);
    return size;
}

static inline int usbd_device_get_status(COMM* comm)
{
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
    printf("USB: get device status\n\r");
#endif
    uint16_t status = 0;
    return usbd_tx(comm, (const char*)&status, sizeof(uint16_t));
}

static inline int usbd_set_address(COMM* comm)
{
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
    printf("USB set ADDRESS %#X\n\r", comm->setup.wValue);
#endif
    board_usb_set_address(comm, comm->setup.wValue);
    switch (comm->usbd.state)
    {
    case USBD_STATE_DEFAULT:
        comm->usbd.state = USBD_STATE_ADDRESSED;
        break;
    case USBD_STATE_ADDRESSED:
        if (comm->setup.wValue == 0)
            comm->usbd.state = USBD_STATE_DEFAULT;
        break;
    default:
        break;
    }
    return 0;
}

static inline int usbd_get_string_descriptor(COMM* comm, int idx, int lang)
{
    int i;
    int offset = 0;
    const STRING_DESCRIPTOR_ITEM* item;
    for (i = 0; i < STRING_DESCRIPTORS_COUNT; ++i)
    {
        item = (const STRING_DESCRIPTOR_ITEM*)((const char*)&__STRING_DESCRIPTORS + offset);
        if (item->idx == idx && item->lang == lang)
            return usbd_tx(comm, &item->str, item->str.bLength);
        offset += item->str.bLength + sizeof(uint16_t) + sizeof(uint8_t);
    }
    return -1;
}

static inline int usbd_get_descriptor(COMM* comm)
{
    //can be called in any device state
    int res = -1;

    int index = comm->setup.wValue & 0xff;
    switch (comm->setup.wValue >> 8)
    {
    case USB_DEVICE_DESCRIPTOR_INDEX:
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
        printf("USB get DEVICE descriptor\n\r");
#endif
        res = usbd_tx(comm, &__DEVICE_DESCRIPTOR, __DEVICE_DESCRIPTOR.bLength);
        break;
    case USB_CONFIGURATION_DESCRIPTOR_INDEX:
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
        printf("USB get CONFIGURATION %d descriptor\n\r", index);
#endif
        res = usbd_tx(comm, &__CONFIGURATION_DESCRIPTOR, ((const USB_CONFIGURATION_DESCRIPTOR_TYPE*)&__CONFIGURATION_DESCRIPTOR)->wTotalLength);
        break;
    case USB_STRING_DESCRIPTOR_INDEX:
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
        printf("USB get STRING %d descriptor, LangID: %#X\n\r", index, comm->setup.wIndex);
#endif
        res = usbd_get_string_descriptor(comm, index, comm->setup.wIndex);
        break;
    }
    return res;
}

static inline int usbd_get_configuration(COMM* comm)
{
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
    printf("USB: get configuration\n\r");
#endif
    return usbd_tx(comm, &comm->usbd.configuration, 1);
}

static inline int usbd_set_configuration(COMM* comm)
{
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
    printf("USB: set configuration %d\n\r", comm->setup.wValue);
#endif
    //read USB 2.0 specification for more details
    if (comm->usbd.state == USBD_STATE_CONFIGURED)
    {
        comm->usbd.configuration = 0;
        comm->usbd.iface = 0;
        comm->usbd.iface_alt = 0;

        comm->usbd.state = USBD_STATE_ADDRESSED;

        class_reset(comm);
    }
    else if (comm->usbd.state == USBD_STATE_ADDRESSED && comm->setup.wValue)
    {
        comm->usbd.configuration = comm->setup.wValue;
        comm->usbd.iface = 0;
        comm->usbd.iface_alt = 0;
        comm->usbd.state = USBD_STATE_CONFIGURED;

        class_configured(comm);
    }
    return 0;
}

static inline int usbd_device_request(COMM* comm)
{
    int res = -1;
    switch (comm->setup.bRequest)
    {
    case USB_REQUEST_GET_STATUS:
        res = usbd_device_get_status(comm);
        break;
    case USB_REQUEST_SET_ADDRESS:
        res = usbd_set_address(comm);
        break;
    case USB_REQUEST_GET_DESCRIPTOR:
        res = usbd_get_descriptor(comm);
        break;
    case USB_REQUEST_GET_CONFIGURATION:
        res = usbd_get_configuration(comm);
        break;
    case USB_REQUEST_SET_CONFIGURATION:
        res = usbd_set_configuration(comm);
        break;
    }

    return res;
}

static inline int usbd_interface_get_status(COMM* comm)
{
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
    printf("USB: get interface status\n\r");
#endif
    uint16_t status = 0;
    return usbd_tx(comm, &status, sizeof(uint16_t));
}

static inline int usbd_set_interface(COMM* comm)
{
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
    printf("USB: interface set\n\r");
#endif
    comm->usbd.iface = comm->setup.wIndex;
    comm->usbd.iface_alt = comm->setup.wValue;
    return 0;
}

static inline int usbd_get_interface(COMM* comm)
{
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
    printf("USB: interface get\n\r");
#endif
    return usbd_tx(comm, &comm->usbd.iface_alt, 1);
}

static inline int usbd_interface_request(COMM* comm)
{
    int res = -1;
    switch (comm->setup.bRequest)
    {
    case USB_REQUEST_GET_STATUS:
        res = usbd_interface_get_status(comm);
        break;
    case USB_REQUEST_SET_INTERFACE:
        res = usbd_set_interface(comm);
        break;
    case USB_REQUEST_GET_INTERFACE:
        res = usbd_get_interface(comm);
        break;
    }
    return res;
}

static inline int usbd_endpoint_get_status(COMM* comm)
{
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
    printf("USB: get endpoint status\n\r");
#endif
    uint16_t status = 0;
    if (board_usb_is_stall(comm, comm->setup.wIndex & 0xffff))
        status |= 1 << 0;
    return usbd_tx(comm, &status, sizeof(uint16_t));
}

static inline int usbd_endpoint_set_feature(COMM* comm)
{
    unsigned int res = -1;
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
    printf("USB: endpoint set feature\n\r");
#endif
    switch (comm->setup.wValue)
    {
    case USBD_FEATURE_ENDPOINT_HALT:
        board_usb_set_stall(comm, comm->setup.wIndex & 0xffff);
        res = 0;
        break;
    default:
        break;
    }
    return res;
}

static inline int usbd_endpoint_clear_feature(COMM* comm)
{
    unsigned int res = -1;
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
    printf("USB: endpoint clear feature\n\r");
#endif
    switch (comm->setup.wValue)
    {
    case USBD_FEATURE_ENDPOINT_HALT:
        board_usb_clear_stall(comm, comm->setup.wIndex & 0xffff);
        res = 0;
        break;
    default:
        break;
    }
    return res;
}

static inline int usbd_endpoint_request(COMM* comm)
{
    int res = -1;
    switch (comm->setup.bRequest)
    {
    case USB_REQUEST_GET_STATUS:
        res = usbd_endpoint_get_status(comm);
        break;
    case USB_REQUEST_SET_FEATURE:
        res = usbd_endpoint_set_feature(comm);
        break;
    case USB_REQUEST_CLEAR_FEATURE:
        res = usbd_endpoint_clear_feature(comm);
        break;
    }
    return res;
}

void usbd_setup_process(COMM* comm)
{
    int res = -1;
    switch (comm->setup.bmRequestType & BM_REQUEST_TYPE)
    {
    case BM_REQUEST_TYPE_STANDART:
        switch (comm->setup.bmRequestType & BM_REQUEST_RECIPIENT)
        {
        case BM_REQUEST_RECIPIENT_DEVICE:
            res = usbd_device_request(comm);
            break;
        case BM_REQUEST_RECIPIENT_INTERFACE:
            res = usbd_interface_request(comm);
            break;
        case BM_REQUEST_RECIPIENT_ENDPOINT:
            res = usbd_endpoint_request(comm);
            break;
        }
        break;
    case BM_REQUEST_TYPE_CLASS:
    case BM_REQUEST_TYPE_VENDOR:
        res = class_setup(comm);
        break;
    }

    if (res > comm->setup.wLength)
        res = comm->setup.wLength;
    //success. start transfers
    if (res >= 0)
    {
        if ((comm->setup.bmRequestType & BM_REQUEST_DIRECTION) == BM_REQUEST_DIRECTION_HOST_TO_DEVICE)
        {
            //data already received, sending status
            comm->usbd.setup_state = USB_SETUP_STATE_STATUS_IN;
            board_usb_tx(comm, USB_EP_IN | 0, NULL, 0);
        }
        else
        {
            //response less, than required and multiples of EP0SIZE - we need to send ZLP on end of transfers
            if (res < comm->setup.wLength && ((res % USB_EP0_SIZE) == 0))
            {
                if (res)
                {
                    comm->usbd.setup_state = USB_SETUP_STATE_DATA_IN_ZLP;
                    board_usb_tx(comm, USB_EP_IN | 0, comm->usbd.buf, res);
                }
                //if no data at all, but request success, we will send ZLP right now
                else
                {
                    comm->usbd.setup_state = USB_SETUP_STATE_DATA_IN;
                    board_usb_tx(comm, USB_EP_IN | 0, NULL, 0);
                }
            }
            else if (res)
            {
                comm->usbd.setup_state = USB_SETUP_STATE_DATA_IN;
                board_usb_tx(comm, USB_EP_IN | 0, comm->usbd.buf, res);
            }
            //data stage is optional
            else
            {
                comm->usbd.setup_state = USB_SETUP_STATE_STATUS_OUT;
                board_usb_tx(comm, USB_EP_IN | 0, NULL, 0);
            }
        }
    }
    else
    {
        if ((comm->setup.bmRequestType & BM_REQUEST_RECIPIENT) == BM_REQUEST_RECIPIENT_ENDPOINT)
            board_usb_set_stall(comm, comm->setup.wIndex);
        else
        {
            board_usb_set_stall(comm, 0);
            board_usb_set_stall(comm, USB_EP_IN | 0);
        }
        comm->usbd.setup_state = USB_SETUP_STATE_REQUEST;
#if (DFU_DEBUG) && (USB_DEBUG_ERRORS)
        printf("Unhandled ");
        switch (comm->setup.bmRequestType & BM_REQUEST_TYPE)
        {
        case BM_REQUEST_TYPE_STANDART:
            printf("STANDART");
            break;
        case BM_REQUEST_TYPE_CLASS:
            printf("CLASS");
            break;
        case BM_REQUEST_TYPE_VENDOR:
            printf("VENDOR");
            break;
        }
        printf(" request\n\r");

        printf("bmRequestType: %X\n\r", comm->setup.bmRequestType);
        printf("bRequest: %X\n\r", comm->setup.bRequest);
        printf("wValue: %X\n\r", comm->setup.wValue);
        printf("wIndex: %X\n\r", comm->setup.wIndex);
        printf("wLength: %X\n\r", comm->setup.wLength);
#endif
    }
}

void usbd_setup(COMM* comm)
{
    //Back2Back setup received
    if (comm->usbd.setup_state != USB_SETUP_STATE_REQUEST)
    {
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
        printf("USB B2B SETUP received, state: %d\n\r", comm->usbd.setup_state);
#endif
        //reset control EP if transaction in progress
        switch (comm->usbd.setup_state)
        {
        case USB_SETUP_STATE_DATA_IN:
        case USB_SETUP_STATE_DATA_IN_ZLP:
        case USB_SETUP_STATE_STATUS_IN:
            board_usb_flush_ep(comm, USB_EP_IN | 0);
            break;
        case USB_SETUP_STATE_DATA_OUT:
        case USB_SETUP_STATE_STATUS_OUT:
            board_usb_flush_ep(comm, 0);
            break;
        default:
            break;
        }

        comm->usbd.setup_state = USB_SETUP_STATE_REQUEST;
    }

    //if data from host - read it first before processing
    if ((comm->setup.bmRequestType & BM_REQUEST_DIRECTION) == BM_REQUEST_DIRECTION_HOST_TO_DEVICE)
    {
        if (comm->setup.wLength)
        {
            comm->usbd.setup_state = USB_SETUP_STATE_DATA_OUT;
            board_usb_rx(comm, 0, comm->usbd.buf, comm->setup.wLength);
        }
        //data stage is optional
        else
            usbd_setup_process(comm);
    }
    else
        usbd_setup_process(comm);
}

void usbd_tx_complete(COMM* comm)
{
    switch (comm->usbd.setup_state)
    {
    case USB_SETUP_STATE_DATA_IN_ZLP:
        //TX ZLP and switch to normal state
        comm->usbd.setup_state = USB_SETUP_STATE_DATA_IN;
        board_usb_tx(comm, USB_EP_IN | 0, NULL, 0);
        break;
    case USB_SETUP_STATE_DATA_IN:
        comm->usbd.setup_state = USB_SETUP_STATE_STATUS_OUT;
        board_usb_rx(comm, 0, NULL, 0);
        break;
    case USB_SETUP_STATE_STATUS_IN:
        comm->usbd.setup_state = USB_SETUP_STATE_REQUEST;
        break;
    default:
#if (DFU_DEBUG) && (USB_DEBUG_ERRORS)
        printf("USBD invalid state on write: %s\n\r", comm->usbd.setup_state);
#endif
        comm->usbd.setup_state = USB_SETUP_STATE_REQUEST;
        break;
    }
}

void usbd_rx_complete(COMM* comm)
{
    switch (comm->usbd.setup_state)
    {
    case USB_SETUP_STATE_DATA_OUT:
        usbd_setup_process(comm);
        break;
    case USB_SETUP_STATE_STATUS_OUT:
        comm->usbd.setup_state = USB_SETUP_STATE_REQUEST;
        break;
    default:
#if (DFU_DEBUG) && (USB_DEBUG_ERRORS)
        printf("USBD invalid setup state on read: %d\n\r", comm->usbd.setup_state);
#endif
        comm->usbd.setup_state = USB_SETUP_STATE_REQUEST;
        break;
    }
}

void usbd_init(COMM* comm)
{
    comm->usbd.setup_state = USB_SETUP_STATE_REQUEST;
    comm->usbd.state = USBD_STATE_DEFAULT;
}

void usbd_reset(COMM* comm)
{
    comm->usbd.state = USBD_STATE_DEFAULT;
    comm->usbd.setup_state = USB_SETUP_STATE_REQUEST;
    comm->usbd.configuration = 0;
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
    printf("USB device reset\n\r");
#endif
    board_usb_close_ep(comm, 0);
    board_usb_close_ep(comm, USB_EP_IN | 0);

    board_usb_open_ep(comm, 0, USB_EP_CONTROL, USB_EP0_SIZE);
    board_usb_open_ep(comm, USB_EP_IN | 0, USB_EP_CONTROL, USB_EP0_SIZE);

    class_reset(comm);
}

void usbd_suspend(COMM* comm)
{
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
    printf("USB device suspend\n\r");
#endif
    board_usb_flush_ep(comm, 0);
    board_usb_flush_ep(comm, USB_EP_IN | 0);
    comm->usbd.setup_state = USB_SETUP_STATE_REQUEST;
    if (comm->usbd.state == USBD_STATE_CONFIGURED)
        class_suspend(comm);
}

void usbd_wakeup(COMM* comm)
{
#if (DFU_DEBUG) && (USB_DEBUG_REQUESTS)
    printf("USB device wakeup\n\r");
#endif

    if (comm->usbd.state == USBD_STATE_CONFIGURED)
        class_wakeup(comm);
}
