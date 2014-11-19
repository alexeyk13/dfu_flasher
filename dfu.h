/*
    USB DFU Flasher
    Copyright (c) 2014, Alexey Kramarenko
    All rights reserved.
*/

#ifndef DFU_H
#define DFU_H

#include <stdint.h>

/* DFU class requests */

#define DFU_DETACH                                      0x0
#define DFU_DNLOAD                                      0x1
#define DFU_UPLOAD                                      0x2
#define DFU_GETSTATUS                                   0x3
#define DFU_CLRSTATUS                                   0x4
#define DFU_GETSTATE                                    0x5
#define DFU_ABORT                                       0x6

/* DFU status codes */

//No error condition is present.
#define DFU_STATUS_OK                                   0x0
//File is not targeted for use by this device.
#define DFU_STATUS_TARGET                               0x1
//File is for this device but fails some vendor-specific verification test.
#define DFU_STATUS_FILE                                 0x2
//Device is unable to write memory.
#define DFU_STATUS_WRITE                                0x3
//Memory erase function failed.
#define DFU_STATUS_ERASE                                0x4
//Memory erase check failed.
#define DFU_STATUS_CHECK_ERASED                         0x5
//Program memory function failed.
#define DFU_STATUS_PROG                                 0x6
//Programmed memory failed verification.
#define DFU_STATUS_VERIFY                               0x7
//Cannot program memory due to received address that is out of range.
#define DFU_STATUS_ADDRESS                              0x8
//Received DFU_DNLOAD with wLength = 0, but device does not think it has all of the data yet.
#define DFU_STATUS_NOTDONE                              0x9
//Device’s firmware is corrupt. It cannot return to run-time (non-DFU) operations.
#define DFU_STATUS_FIRMWARE                             0xa
//iString indicates a vendor-specific error.
#define DFU_STATUS_VENDOR                               0xb
//Device detected unexpected USB reset signaling.
#define DFU_STATUS_USBR                                 0xc
//Device detected unexpected power on reset.
#define DFU_STATUS_POR                                  0xd
//Something went wrong, but the device does not know what it was.
#define DFU_STATUS_UNKNOWN                              0xe
//Device stalled an unexpected request.
#define DFU_STATUS_STALLEDPKT                           0xf

/* DFU state machine */

//Device is running its normal application.
#define DFU_STATE_APP_IDLE                              0x0
//Device is running its normal application, has received the DFU_DETACH request, and is waiting for a USB reset.
#define DFU_STATE_APP_DETACH                            0x1
//Device is operating in the DFU mode and is waiting for requests.
#define DFU_STATE_IDLE                                  0x2
//Device has received a block and is waiting for the host to solicit the status via DFU_GETSTATUS.
#define DFU_STATE_DNLOAD_SYNC                           0x3
//Device is programming a control-write block into its nonvolatile memories.
#define DFU_STATE_DNBUSY                                0x4
//Device is processing a download operation. Expecting DFU_DNLOAD requests.
#define DFU_STATE_DNLOAD_IDLE                           0x5
//Device has received the final block of firmware from the host and is waiting for receipt of DFU_GETSTATUS to begin the
//Manifestation phase; or device has completed the Manifestation phase and is waiting for receipt of
//DFU_GETSTATUS. (Devices that can enter this state after the Manifestation phase set bmAttributes bit
//bitManifestationTolerant to 1.)
#define DFU_STATE_MANIFEST_SYNC                         0x6
//Device is in the Manifestation phase. (Not all devices will be able to respond to DFU_GETSTATUS when in this state.)
#define DFU_STATE_MANIFEST                              0x7
//Device has programmed its memories and is waiting for a RESET USB reset or a power on reset. (Devices that must enter this
//state clear bitManifestationTolerant to 0.)
#define DFU_STATE_MANIFEST_WAIT_RESET                   0x8
//The device is processing an upload operation. Expecting DFU_UPLOAD requests.
#define DFU_STATE_UPLOAD_IDLE                           0x9
//An error has occurred. Awaiting the DFU_CLRSTATUS request.
#define DFU_STATE_ERROR                                 0xa

/* DFU structures */

#pragma pack(push, 1)

#define DFU_FUNCTIONAL_DESCRIPTOR_INDEX                 0x21
#define DFU_FUNCTIONAL_DESCRIPTOR_SIZE                  7

#define DFU_MANIFEST_TOLERANT                           (1 << 2)
#define DFU_CAN_UPLOAD                                  (1 << 1)
#define DFU_CAN_DOWNLOAD                                (1 << 0)

typedef struct {
    uint8_t  bLength;                                   //Size of this descriptor in bytes
    uint8_t  bDescriptorType;                           //DFU FUNCTIONAL descriptor type
    uint8_t  bmAttributes;                              //DFU attributes
                                                        //Bit 7..3: reserved
                                                        //Bit 2: device is able to communicate
                                                        //via USB after Manifestation phase.
                                                        //(bitManifestationTolerant)
                                                        //0 = no, must see bus reset
                                                        //1 = yes
                                                        //Bit 1: upload capable (bitCanUpload)
                                                        //0 = no
                                                        //1 = yes
                                                        //Bit 0: download capable
                                                        //(bitCanDnload)
                                                        //0 = no
                                                        //1 = yes
    uint16_t  wDetachTimeOut;                           //Time, in milliseconds, that the device
                                                        //will wait after receipt of the
                                                        //DFU_DETACH request. If this time
                                                        //elapses without a USB reset, then the
                                                        //device will terminate the
                                                        //Reconfiguration phase and revert
                                                        //back to normal operation. This
                                                        //represents the maximum time that the
                                                        //device can wait (depending on its
                                                        //timers, etc.). The host may specify a
                                                        //shorter timeout in the DFU_DETACH
                                                        //request.
    uint16_t wTransferSize;                             //Maximum number of bytes that the
                                                        //device can accept per control-write
                                                        //transaction.
} DFU_FUNCTIONAL_DESCRIPTOR_TYPE, *P_DFU_FUNCTIONAL_DESCRIPTOR_TYPE;

typedef struct {
    uint8_t bStatus;                                    //An indication of the status resulting from the
                                                        //execution of the most recent request.
    uint8_t bwPollTime[3];                              //Minimum time, in milliseconds, that the host should
                                                        //wait before sending a subsequent
                                                        //DFU_GETSTATUS request.
    uint8_t bState;                                     //An indication of the state that the device is going to
                                                        //enter immediately following transmission of this
                                                        //response. (By the time the host receives this
                                                        //information, this is the current state of the device.)

    uint8_t iString;                                    //Index of status description in string table.
} DFU_STATUS_TYPE, *P_DFU_STATUS_TYPE;

#pragma pack(pop)


#endif // DFU_H
