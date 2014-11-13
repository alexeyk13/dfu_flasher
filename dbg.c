#include "dbg.h"
#include "board.h"
#include "printf.h"
#include <stdarg.h>

void write_handler(const char *const buf, unsigned int size, void* param)
{
    board_dbg(buf, size);
}

void printf(const char *const fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    __format(fmt, va, write_handler, 0);
    va_end(va);
}
