/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "dfud.h"
#include "dfu.h"
#include "comm.h"
#include "board.h"
#include "config.h"
#if (DFU_DEBUG)
#include "dbg.h"
#endif

void class_reset(COMM* comm)
{

}

void class_suspend(COMM* comm)
{

}

void class_wakeup(COMM* comm)
{

}

void class_configured(COMM* comm)
{

}

static inline int dfud_get_status(COMM* comm)
{
#if (DFU_DEBUG) && (USB_DEBUG_CLASS_REQUESTS)
    printf("DFU: get status\n\r");
#endif
    DFU_STATUS_TYPE status;
    status.bStatus = comm->dfud.status;
    status.bwPollTime[0] = status.bwPollTime[1] = status.bwPollTime[2] = 0;
    status.bState = comm->dfud.state;
    status.iString = 0;
    return usbd_tx(comm, &status, sizeof(DFU_STATUS_TYPE));
}

int class_setup(COMM* comm)
{
    int res = -1;
    switch (comm->setup.bRequest)
    {
//    case DFU_DNLOAD:
//        break;
    case DFU_GETSTATUS:
        res = dfud_get_status(comm);
        break;
/*    case DFU_CLRSTATUS:
        break;
    case DFU_GETSTATE:
        break;
    case DFU_ABORT:
        break;*/
    }
    return res;
}

void dfud_init(COMM* comm)
{
    comm->dfud.status = DFU_STATUS_OK;
    comm->dfud.state = DFU_STATE_IDLE;
}
