/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "board.h"
#include "config.h"
#if DFU_DEBUG
#include "dbg.h"

#define VERSION                     "0.1"
#endif

void  main() __attribute__ ((section (".loader")));

void main()
{
    board_init();
#if DFU_DEBUG
    board_dbg_init();
    printf("DFU Flasher V%s started...\n\r", VERSION);
#endif

#if DFU_DEBUG
    printf("DFU Flasher reset\n\r");
#endif
    board_reset();
}
