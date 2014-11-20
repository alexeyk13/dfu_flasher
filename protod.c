/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "protod.h"
#include "comm.h"
#include "usbd.h"
#include "dfu.h"
#include "dbg.h"

int protod_tx(COMM* comm, int* size)
{
    *size = usbd_tx(comm, "test", 4);
    printf("protocol tx\n\r");
    return DFU_STATUS_OK;
}

int protod_rx(COMM* comm)
{
    printf("protocol rx: %d\n\r", comm->dfud.size);
    return DFU_STATUS_OK;
}
