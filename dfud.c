/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "dfud.h"
#include "dfu.h"
#include "comm.h"
#include "board.h"
#include <string.h>
#if (DFU_DEBUG)
#include "dbg.h"
#endif

void class_reset(COMM* comm)
{
    comm->dfud.status = DFU_STATUS_OK;
    comm->dfud.state = DFU_STATE_IDLE;
    comm->dfud.size = 0;
}

void class_suspend(COMM* comm)
{

}

void class_wakeup(COMM* comm)
{

}

void class_configured(COMM* comm)
{
    class_reset(comm);
}

static inline void dfu_set_error(COMM* comm, int code)
{
    comm->dfud.state = DFU_STATE_ERROR;
    comm->dfud.status = code;
    if (code == DFU_STATUS_STALLEDPKT)
    {
        board_usb_set_stall(comm, 0);
        board_usb_set_stall(comm, USB_EP_IN | 0);
    }
}

static inline int dfud_dnload(COMM* comm)
{
#if (DFU_DEBUG) && (USB_DEBUG_CLASS_REQUESTS)
    printf("DFU DNLOAD: %d\n\r", comm->setup.wLength);
#endif
    switch (comm->dfud.state)
    {
    case DFU_STATE_IDLE:
        comm->dfud.size = comm->setup.wLength;
        if (comm->dfud.size > 0)
        {
            memcpy(comm->dfud.buf, comm->usbd.buf, comm->dfud.size);
            comm->dfud.state = DFU_STATE_DNLOAD_SYNC;
        }
        else
            dfu_set_error(comm, DFU_STATUS_NOTDONE);
        break;
    case DFU_STATE_DNLOAD_IDLE:
        if (comm->setup.wLength == 0)
        {
            comm->dfud.state = DFU_STATE_MANIFEST_SYNC;
            printf("TODO: process packet %d\n\r", comm->dfud.size);
            break;
        }
    default:
        dfu_set_error(comm, DFU_STATUS_STALLEDPKT);
    }
    return 0;
}

static inline int dfud_get_status(COMM* comm)
{
    switch (comm->dfud.state)
    {
    case DFU_STATE_DNLOAD_SYNC:
        comm->dfud.state = DFU_STATE_DNLOAD_IDLE;
        break;
    case DFU_STATE_MANIFEST_SYNC:
        //TODO: inform USB stop here
        comm->dfud.state = DFU_STATE_IDLE;
        break;
    }

#if (DFU_DEBUG) && (USB_DEBUG_CLASS_REQUESTS)
    printf("DFU get STATUS: %d\n\r", comm->dfud.state);
#endif

    DFU_STATUS_TYPE status;
    status.bStatus = comm->dfud.status;
    status.bwPollTime[0] = status.bwPollTime[1] = status.bwPollTime[2] = 0;
    status.bState = comm->dfud.state;
    status.iString = 0;

    return usbd_tx(comm, &status, sizeof(DFU_STATUS_TYPE));
}

static inline int dfud_get_state(COMM* comm)
{
#if (DFU_DEBUG) && (USB_DEBUG_CLASS_REQUESTS)
    printf("DFU get STATE: %d\n\r", comm->usbd.state);
#endif
    char state = (char)comm->usbd.state;
    return usbd_tx(comm, &state, sizeof(char));
}

int class_setup(COMM* comm)
{
    int res = -1;
    switch (comm->setup.bRequest)
    {
    case DFU_DNLOAD:
        res = dfud_dnload(comm);
        break;
    case DFU_GETSTATUS:
        res = dfud_get_status(comm);
        break;
    case DFU_GETSTATE:
        res = dfud_get_state(comm);
        break;
    case DFU_CLRSTATUS:
    case DFU_ABORT:
        class_reset(comm);
        break;
    }
    return res;
}

void dfud_init(COMM* comm)
{
    class_reset(comm);
}
