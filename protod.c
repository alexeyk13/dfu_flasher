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
#include "board.h"
#include "areas.h"
#if (DFU_DEBUG) && (PROTO_DEBUG)
#include "dbg.h"
#endif

#if (PROTO_ENABLE_UPLOAD)
static inline int protod_cmd_read(COMM* comm, unsigned int addr, unsigned int size, int* tx_size)
{
    int i;
    for (i = 0; i < READ_AREAS_COUNT; ++i)
    {
        if (__READ_AREAS[i].addr <= addr && __READ_AREAS[i].addr + __READ_AREAS[i].size >= addr + size)
        {
            int res = board_flash_read(comm, addr, comm->dfud.buf, size);
            if (res == DFU_STATUS_OK)
                *tx_size = usbd_tx(comm, comm->dfud.buf, size);
            return res;
        }
    }
    return DFU_STATUS_ADDRESS;
}

#endif

static inline int protod_cmd_erase(COMM* comm, unsigned int addr, unsigned int size)
{
    int i;
    for (i = 0; i < WRITE_AREAS_COUNT; ++i)
    {
        if (__WRITE_AREAS[i].addr <= addr && __WRITE_AREAS[i].addr + __WRITE_AREAS[i].size >= addr + size)
            return board_flash_erase(comm, addr, size);
    }
    return DFU_STATUS_ADDRESS;
}

static inline int protod_cmd_write(COMM* comm, unsigned int addr, const char* buf, unsigned int size)
{
    int i;
    for (i = 0; i < WRITE_AREAS_COUNT; ++i)
    {
        if (__WRITE_AREAS[i].addr <= addr && __WRITE_AREAS[i].addr + __WRITE_AREAS[i].size >= addr + size)
            return board_flash_write(comm, addr, buf, size);
    }
    return DFU_STATUS_ADDRESS;
}

static inline int protod_cmd_version(COMM* comm, int* tx_size)
{
    PROTO_VERSION_RESP version;
    version.loader = VERSION;
    version.protocol = PROTO_VERSION;
    *tx_size = usbd_tx(comm, &version, sizeof(PROTO_VERSION_RESP));
    return DFU_STATUS_OK;
}

int protod_tx(COMM* comm, int* tx_size)
{
    int res = DFU_STATUS_NOTDONE;
    switch (comm->protod.cmd)
    {
    case PROTO_CMD_VERSION:
        res = protod_cmd_version(comm, tx_size);
        break;
#if (PROTO_ENABLE_UPLOAD)
    case PROTO_CMD_READ:
        res = protod_cmd_read(comm, comm->protod.param1, comm->protod.param2, tx_size);
        break;
#endif
    default:
        return DFU_STATUS_NOTDONE;
    }
    comm->protod.cmd = 0;
    return res;
}

int protod_rx(COMM* comm)
{
    int res = DFU_STATUS_TARGET;
    if (comm->dfud.size < sizeof(PROTO_REQ))
        return DFU_STATUS_NOTDONE;
    PROTO_REQ* req = (PROTO_REQ*)comm->dfud.buf;
    if (req->data_size + sizeof(PROTO_REQ) < comm->dfud.size)
        return DFU_STATUS_NOTDONE;
    comm->protod.cmd = req->cmd;
    comm->protod.param1 = req->param1;
    comm->protod.param2 = req->param2;
    switch (req->cmd)
    {
#if (PROTO_ENABLE_UPLOAD)
    case PROTO_CMD_READ:
#if (DFU_DEBUG) && (PROTO_DEBUG)
        printf("cmd read %#X-%#X\n\r", req->param1, req->param1 + req->param2);
#endif
        res = DFU_STATUS_OK;
        break;
#endif
    case PROTO_CMD_VERSION:
#if (DFU_DEBUG) && (PROTO_DEBUG)
        printf("cmd version\n\r");
#endif
        res = DFU_STATUS_OK;
        break;
    case PROTO_CMD_WRITE:
#if (DFU_DEBUG) && (PROTO_DEBUG)
        printf("cmd write %#X-%#X\n\r", req->param1, req->param1 + req->param2);
#endif
        res = protod_cmd_write(comm, req->param1, comm->dfud.buf + sizeof(PROTO_REQ), req->param2);
        break;
    case PROTO_CMD_ERASE:
#if (DFU_DEBUG) && (PROTO_DEBUG)
        printf("cmd erase %#X-%#X\n\r", req->param1, req->param1 + req->param2);
#endif
        res = protod_cmd_erase(comm, req->param1, req->param2);
        break;
    case PROTO_CMD_LEAVE:
#if (DFU_DEBUG) && (PROTO_DEBUG)
        printf("cmd leave\n\r");
#endif
        comm->usbd.pend_reset = true;
        res = DFU_STATUS_OK;
        break;
    }
    return res;
}

void protod_init(COMM* comm)
{
    comm->protod.cmd = 0;
}
