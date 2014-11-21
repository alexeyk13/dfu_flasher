/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "stm32l0_flash.h"
#include "stm32.h"
#include "../../board.h"
#include "../../dfu.h"
#include "config.h"
#include <string.h>

#define PEKEY1                                  0x89ABCDEF
#define PEKEY2                                  0x02030405

#define PRGKEY1                                 0x8C9DAEBF
#define PRGKEY2                                 0x13141516

#define FLASH_PAGE_SIZE                         128

#define FLASH_SR_ERROR                          (FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_SIZERR)

void flash_init()
{
    //unlock PELOCK bit
    if (FLASH->PECR & FLASH_PECR_PELOCK)
    {
        FLASH->PEKEYR = PEKEY1;
        FLASH->PEKEYR = PEKEY2;
    }
    //unlock PRGLOCK bit
    if (FLASH->PECR & FLASH_PECR_PRGLOCK)
    {
        FLASH->PRGKEYR = PRGKEY1;
        FLASH->PRGKEYR = PRGKEY2;
    }
}

#if (PROTO_ENABLE_UPLOAD)
int board_flash_read(COMM* comm, unsigned int addr, char* buf, unsigned int size)
{
    memcpy(buf, (void*)addr, size);
    return DFU_STATUS_OK;
}
#endif

int board_flash_erase(COMM* comm, unsigned int addr, unsigned int size)
{
    if ((size != FLASH_PAGE_SIZE) || (addr % FLASH_PAGE_SIZE))
        return DFU_STATUS_CHECK_ERASED;
    while (FLASH->SR & FLASH_SR_BSY) {}
    FLASH->PECR = FLASH_PECR_ERASE | FLASH_PECR_PROG;
    *((uint32_t*)addr) = 0;
    while (FLASH->SR & FLASH_SR_BSY) {}

    if (FLASH->SR & FLASH_SR_ERROR)
    {
        FLASH->SR |= FLASH_SR_ERROR;
        return DFU_STATUS_ERASE;
    }
    return DFU_STATUS_OK;
}

int board_flash_write(COMM* comm, unsigned int addr, const char* buf, unsigned int size)
{
    int i;
    if ((size != FLASH_PAGE_SIZE) || (addr % FLASH_PAGE_SIZE))
        return DFU_STATUS_PROG;

    while (FLASH->SR & FLASH_SR_BSY) {}
    FLASH->PECR = FLASH_PECR_PROG;

    for (i = 0; i < FLASH_PAGE_SIZE / sizeof(uint32_t); ++i)
    {
        ((uint32_t*)addr)[i] = ((const uint32_t*)buf)[i];
        while (FLASH->SR & FLASH_SR_BSY) {}
        if (FLASH->SR & FLASH_SR_ERROR)
        {
            FLASH->SR |= FLASH_SR_ERROR;
            return DFU_STATUS_PROG;
        }
    }
    return DFU_STATUS_OK;
}
