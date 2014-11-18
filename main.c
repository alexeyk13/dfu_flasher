/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "core.h"
#include "board.h"
#include "config.h"
#include "comm.h"
#if DFU_DEBUG
#include "dbg.h"

#define VERSION                     "0.1"
#endif

void main() __attribute__ ((section (".loader")));

void main()
{
    core_init();
    board_init();
#if DFU_DEBUG
    board_dbg_init();
    printf("DFU Flasher V%s started...\n\r", VERSION);
#endif

    comm_start();

#if DFU_DEBUG
    printf("DFU Flasher reset\n\r");
#endif
    board_reset();
    for (;;) {}
}
