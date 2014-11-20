/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "protod.h"
#include "proto.h"
#include "comm.h"
#include "usbd.h"
#include "dfu.h"
#include "config.h"
#if (DFU_DEBUG) && (PROTO_DEBUG)
#include "dbg.h"
#endif

static inline int protod_cmd_version(COMM* comm)
{
    PROTO_VERSION_RESP version;
    version.loader = VERSION;
    version.protocol = PROTO_VERSION;
    return usbd_tx(comm, &version, sizeof(PROTO_VERSION_RESP));
}

int protod_tx(COMM* comm, int* size)
{
    switch (comm->protod.cmd)
    {
    case PROTO_CMD_VERSION:
        *size = protod_cmd_version(comm);
        break;
    default:
        return DFU_STATUS_NOTDONE;
    }
    comm->protod.cmd = 0;
    return DFU_STATUS_OK;
}

int protod_rx(COMM* comm)
{
    if (comm->dfud.size < sizeof(PROTO_REQ))
        return DFU_STATUS_NOTDONE;
    PROTO_REQ* req = (PROTO_REQ*)comm->dfud.buf;
    if (req->data_size + sizeof(PROTO_REQ) < comm->dfud.size)
        return DFU_STATUS_NOTDONE;
    switch (req->cmd)
    {
    case PROTO_CMD_WRITE:
        printf("cmd write\n\r");
        break;
    case PROTO_CMD_ERASE:
        printf("cmd erase\n\r");
        break;
    case PROTO_CMD_ERASE_STATUS:
        printf("cmd erase status\n\r");
        break;
    case PROTO_CMD_VERSION:
#if (DFU_DEBUG) && (PROTO_DEBUG)
        printf("cmd version\n\r");
#endif
        comm->protod.cmd = PROTO_CMD_VERSION;
        break;
    case PROTO_CMD_LEAVE:
#if (DFU_DEBUG) && (PROTO_DEBUG)
        printf("cmd leave\n\r");
#endif
        comm->usbd.pend_reset = true;
        break;
    default:
        return DFU_STATUS_TARGET;
        break;
    }
    return DFU_STATUS_OK;
}

void protod_init(COMM* comm)
{
    comm->protod.cmd = 0;
}
