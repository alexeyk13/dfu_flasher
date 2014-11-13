/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "board.h"

void  main() __attribute__ ((section (".loader")));

void main()
{
    board_init();

    board_reset();
}
