/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2014, Alexey Kramarenko
    All rights reserved.
*/
/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#include "stm32l0_usb.h"
#include "stm32.h"
#include "stm32l0_gpio.h"
#include "../../board.h"
#include "../../comm_private.h"
#include "../../usb.h"
#include "../../usbd.h"
#include "config.h"
#if (DFU_DEBUG) && (USB_DEBUG_ERRORS)
#include "../../dbg.h"
#endif



#ifndef NULL
#define NULL                                        0
#endif

#define USB_DM                                      A11
#define USB_DP                                      A12

#define USB_EP_TOTAL_COUNT                          8

#pragma pack(push, 1)

typedef struct {
    uint16_t ADDR_TX;
    uint16_t COUNT_TX;
    uint16_t ADDR_RX;
    uint16_t COUNT_RX;
} USB_BUFFER_DESCRIPTOR;

#define USB_BUFFER_DESCRIPTORS                      ((USB_BUFFER_DESCRIPTOR*) USB_PMAADDR)

#pragma pack(pop)

static inline uint16_t* ep_reg_data(int num)
{
    return (uint16_t*)(USB_BASE + USB_EP_NUM(num) * 4);
}

static inline EP* ep_data(COMM* comm, int num)
{
    return (num & USB_EP_IN) ? &(comm->drv.in[USB_EP_NUM(num)]) : &(comm->drv.out[USB_EP_NUM(num)]);
}

static inline void ep_toggle_bits(int num, int mask, int value)
{
    *ep_reg_data(num) = ((*ep_reg_data(num)) & USB_EPREG_MASK) | (((*ep_reg_data(num)) & mask) ^ value);
}

static inline void memcpy16(void* dst, void* src, unsigned int size)
{
    int i;
    size = (size + 1) / 2;
    for (i = 0; i < size; ++i)
        ((uint16_t*)dst)[i] = ((uint16_t*)src)[i];
}

void board_usb_init(COMM* comm)
{
    comm->drv.addr = 0;
    comm->drv.suspend = false;

    //enable DM/DP
    gpio_enable_pin(USB_DM, GPIO_MODE_AF | GPIO_OT_PUSH_PULL | GPIO_SPEED_HIGH, AF0);
    gpio_enable_pin(USB_DP, GPIO_MODE_AF | GPIO_OT_PUSH_PULL | GPIO_SPEED_HIGH, AF0);

    //enable clock
    RCC->APB1ENR |= RCC_APB1ENR_USBEN;

    //close device, probably opened by main device
    board_usb_stop(comm);
}

void board_usb_flush_ep(COMM* comm, int num)
{
    EP* ep = ep_data(comm, num);
    ep->ptr = NULL;
    if (num & USB_EP_IN)
        ep_toggle_bits(num, USB_EPTX_STAT, USB_EP_TX_NAK);
    else
        ep_toggle_bits(num, USB_EPRX_STAT, USB_EP_RX_NAK);
}

void board_usb_close_ep(COMM* comm, int num)
{
    board_usb_flush_ep(comm, num);
    if (num & USB_EP_IN)
        ep_toggle_bits(num, USB_EPTX_STAT, USB_EP_TX_DIS);
    else
        ep_toggle_bits(num, USB_EPRX_STAT, USB_EP_RX_DIS);
    EP* ep = ep_data(comm, num);
    ep->ptr = NULL;
    ep->size = 0;
    ep->mps = 0;
}

void board_usb_open_ep(COMM* comm, int num, USB_EP_TYPE type, int size)
{
    //find free addr in FIFO
    unsigned int fifo, i;
    fifo = 0;
    for (i = 0; i < USB_EP_COUNT_MAX; ++i)
    {
        fifo += comm->drv.in[i].mps;
        fifo += comm->drv.out[i].mps;
    }
    fifo += sizeof(USB_BUFFER_DESCRIPTOR) * USB_EP_COUNT_MAX;

    EP* ep = ep_data(comm, num);

    uint16_t ctl = USB_EP_NUM(num);
    //setup ep type
    switch (type)
    {
    case USB_EP_CONTROL:
        ctl |= 1 << 9;
        break;
    case USB_EP_BULK:
        ctl |= 0 << 9;
        break;
    case USB_EP_INTERRUPT:
        ctl |= 3 << 9;
        break;
    case USB_EP_ISOCHRON:
        ctl |= 2 << 9;
        break;
    }

    //setup FIFO
    if (num & USB_EP_IN)
    {
        USB_BUFFER_DESCRIPTORS[USB_EP_NUM(num)].ADDR_TX = fifo;
        USB_BUFFER_DESCRIPTORS[USB_EP_NUM(num)].COUNT_TX = 0;
    }
    else
    {
        USB_BUFFER_DESCRIPTORS[USB_EP_NUM(num)].ADDR_RX = fifo;
        if (size <= 62)
            USB_BUFFER_DESCRIPTORS[USB_EP_NUM(num)].COUNT_RX = ((size + 1) >> 1) << 10;
        else
            USB_BUFFER_DESCRIPTORS[USB_EP_NUM(num)].COUNT_RX = ((((size + 3) >> 2) - 1) << 10) | (1 << 15);
    }
    ep->mps = size;

    *ep_reg_data(num) = ctl;
    //set NAK, clear DTOG
    if (num & USB_EP_IN)
        ep_toggle_bits(num, USB_EPTX_STAT | USB_EP_DTOG_TX, USB_EP_TX_NAK);
    else
        ep_toggle_bits(num, USB_EPRX_STAT | USB_EP_DTOG_RX, USB_EP_RX_NAK);
}

void usb_fifo_tx(COMM* comm, int num)
{
    EP* ep = ep_data(comm, num);
    int size = ep->size - ep->processed;
    if (size > ep->mps)
        size = ep->mps;
    USB_BUFFER_DESCRIPTORS[USB_EP_NUM(num)].COUNT_TX = size;
    memcpy16((void*)(USB_BUFFER_DESCRIPTORS[USB_EP_NUM(num)].ADDR_TX + USB_PMAADDR), ep->ptr +  ep->processed, size);
    ep_toggle_bits(num, USB_EPTX_STAT, USB_EP_TX_VALID);
    ep->processed += size;
}

void board_usb_tx(COMM* comm, int num, char* data, int size)
{
    EP* ep = ep_data(comm, num);
    ep->size = size;
    ep->ptr = data;
    ep->processed = 0;

    usb_fifo_tx(comm, num);
}

void board_usb_rx(COMM* comm, int num, char* data, int size)
{
    EP* ep = ep_data(comm, num);
    //no blocks for ZLP
    ep->size = size;
    ep->ptr = data;
    ep->processed = 0;

    ep_toggle_bits(num, USB_EPRX_STAT, USB_EP_RX_VALID);
}

void board_usb_set_stall(COMM* comm, int num)
{
    board_usb_flush_ep(comm, num);
    if (USB_EP_IN & num)
        ep_toggle_bits(num, USB_EPTX_STAT, USB_EP_TX_STALL);
    else
        ep_toggle_bits(num, USB_EPRX_STAT, USB_EP_RX_STALL);
}

void board_usb_clear_stall(COMM* comm, int num)
{
    board_usb_flush_ep(comm, num);
    if (USB_EP_IN & num)
        ep_toggle_bits(num, USB_EPTX_STAT, USB_EP_TX_NAK);
    else
        ep_toggle_bits(num, USB_EPRX_STAT, USB_EP_RX_NAK);
}

bool board_usb_is_stall(COMM* comm, int num)
{
    if (USB_EP_IN & num)
        return ((*ep_reg_data(num)) & USB_EPTX_STAT) == USB_EP_TX_STALL;
    else
        return ((*ep_reg_data(num)) & USB_EPRX_STAT) == USB_EP_RX_STALL;
}

bool board_usb_start(COMM* comm)
{
    //power up and wait tStartup
    USB->CNTR &= ~USB_CNTR_PDWN;
    delay_us(1);
    USB->CNTR &= ~USB_CNTR_FRES;

    USB->CNTR |= USB_CNTR_CTRM;

    //clear any spurious pending interrupts
    USB->ISTR = 0;

    //buffer descriptor table at top
    USB->BTABLE = 0;

    //pullup device
    USB->BCDR |= USB_BCDR_DPPU;
    return true;
}

void board_usb_stop(COMM* comm)
{
    //disable pullup
    USB->BCDR &= ~USB_BCDR_DPPU;

    int i;
    //close all endpoints
    for (i = 0; i < USB_EP_COUNT_MAX; ++i)
    {
        board_usb_close_ep(comm, i);
        board_usb_close_ep(comm, USB_EP_IN | i);
    }

    for (i = USB_EP_COUNT_MAX; i < USB_EP_TOTAL_COUNT; ++i)
    {
        ep_toggle_bits(i, USB_EPTX_STAT | USB_EP_DTOG_TX, USB_EP_TX_DIS);
        ep_toggle_bits(i, USB_EPRX_STAT | USB_EP_DTOG_TX, USB_EP_RX_DIS);
        *ep_reg_data(i) = 0;
    }

    //power down, disable all interrupts
    USB->DADDR = 0;
    USB->CNTR = USB_CNTR_PDWN | USB_CNTR_FRES;
}

static inline void usb_ctr(COMM* comm)
{
    uint8_t num;
    num = USB->ISTR & USB_ISTR_EP_ID;
    uint16_t size;

    //got SETUP
    if (num == 0 && (*ep_reg_data(num)) & USB_EP_SETUP)
    {
        memcpy16(&comm->setup, (void*)(USB_BUFFER_DESCRIPTORS[0].ADDR_RX + USB_PMAADDR), sizeof(SETUP));
        *ep_reg_data(num) = (*ep_reg_data(num)) & USB_EPREG_MASK & ~USB_EP_CTR_RX;
        usbd_setup(comm);
        return;
    }

    if ((*ep_reg_data(num)) & USB_EP_CTR_RX)
    {
        size = USB_BUFFER_DESCRIPTORS[num].COUNT_RX & 0x3ff;
        memcpy16(comm->drv.out[num].ptr + comm->drv.out[num].processed, (void*)(USB_BUFFER_DESCRIPTORS[num].ADDR_RX + USB_PMAADDR), size);
        *ep_reg_data(num) = (*ep_reg_data(num)) & USB_EPREG_MASK & ~USB_EP_CTR_RX;
        comm->drv.out[num].processed += size;

        if (comm->drv.out[num].processed >= comm->drv.out[num].size)
        {
            if (num == 0)
                usbd_rx_complete(comm);
            else
            {
//                printf("RX class complete\n\r");
            }
        }
        else
            ep_toggle_bits(num, USB_EPRX_STAT, USB_EP_RX_VALID);
        return;
    }

    if ((*ep_reg_data(num)) & USB_EP_CTR_TX)
    {
        *ep_reg_data(num) = (*ep_reg_data(num)) & USB_EPREG_MASK & ~USB_EP_CTR_TX;
        //handle STATUS in for set address
        if (comm->drv.addr && USB_BUFFER_DESCRIPTORS[num].COUNT_TX == 0)
        {
            USB->DADDR = USB_DADDR_EF | comm->drv.addr;
            comm->drv.addr = 0;
        }

        if (comm->drv.in[num].processed >= comm->drv.in[num].size)
        {
            if (num == 0)
                usbd_tx_complete(comm);
            else
            {
//                printf("TX class complete\n\r");
            }
        }
        else
            usb_fifo_tx(comm, USB_EP_IN | num);
    }
}

static inline void usb_reset(COMM* comm)
{
    comm->drv.suspend = false;
    //enable function
    USB->DADDR = USB_DADDR_EF;
    usbd_reset(comm);
}

static inline void usb_suspend(COMM* comm)
{
    comm->drv.suspend = true;
    usbd_suspend(comm);
}

static inline void usb_wakeup(COMM* comm)
{
    comm->drv.suspend = false;
    usbd_wakeup(comm);
}

void board_usb_request(COMM* comm)
{
    //TODO: wdt

    uint16_t sta = USB->ISTR;

    //transfer complete. Most often called
    if (sta & USB_ISTR_CTR)
    {
        usb_ctr(comm);
        return;
    }
    //rarely called
    if (sta & USB_ISTR_RESET)
    {
        usb_reset(comm);
        USB->ISTR &= ~USB_ISTR_RESET;
        return;
    }
    if ((sta & USB_ISTR_SUSP) && (!comm->drv.suspend))
    {
        usb_suspend(comm);
        USB->ISTR &= ~USB_ISTR_SUSP;
        return;
    }
    USB->ISTR &= ~USB_ISTR_SUSP;
    if (sta & USB_ISTR_WKUP)
    {
        usb_wakeup(comm);
        USB->ISTR &= ~USB_ISTR_WKUP;
        return;
    }
#if (USB_DEBUG_ERRORS)
    if (sta & USB_ISTR_PMAOVR)
    {
#if (DFU_DEBUG)
        printf("USB overflow\n\r");
#endif
        USB->ISTR &= ~USB_ISTR_PMAOVR;
        return;
    }
    if (sta & USB_ISTR_ERR)
    {
#if (DFU_DEBUG)
        printf("USB hardware error\n\r");
#endif
        USB->ISTR &= ~USB_ISTR_ERR;
        return;
    }
#endif
}

void board_usb_set_address(COMM* comm, unsigned int address)
{
    //address will be set after STATUS IN packet
    if (address)
        comm->drv.addr = address;
    else
        USB->DADDR = USB_DADDR_EF;
}
