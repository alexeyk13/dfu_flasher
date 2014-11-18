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

//if you've got error in this line, than this file is compiled wrong

#include "config.h"

/* Define constants used in low-level initialization.  */

  .equ   ICER0,                 0xe000e180
  .equ   ICPR0,                 0xe000e280

/* exported global constant and functions */
	.global core_init

/*********************** reset vector handler *********************/
	.section  .text, "ax"
	.syntax unified
#if defined(CORTEX_M3)
	.cpu cortex-m3
#elif defined(CORTEX_M4)
	.cpu cortex-m4
#else
	.cpu cortex-m0
#endif
	.thumb

	.thumb_func
core_init:
	cpsid i
	mov	r3, lr

	@disable all interrupts
	ldr	r0, =0xffffffff
	ldr	r1, =ICER0
	str	r0, [r1]
#if defined(CORTEX_M3) || defined(CORTEX_M4)
	str	r0, [r1, #0x4]
	str	r0, [r1, #0x8]
	str	r0, [r1, #0xc]
	str	r0, [r1, #0x10]
	str	r0, [r1, #0x14]
	str	r0, [r1, #0x18]
	str	r0, [r1, #0x1c]
#endif

	@clear all pending interrupts
	ldr	r1, =ICPR0
	str	r0, [r1]
#if defined(CORTEX_M3) || defined(CORTEX_M4)
	str	r0, [r1, #0x4]
	str	r0, [r1, #0x8]
	str	r0, [r1, #0xc]
	str	r0, [r1, #0x10]
	str	r0, [r1, #0x14]
	str	r0, [r1, #0x18]
	str	r0, [r1, #0x1c]
#endif

	@switch to MSP
	ldr	r0, =0x0;
	mrs	r0, control
	ldr	r0, =SP_PTR
	mov	sp, r0

	bx		r3
