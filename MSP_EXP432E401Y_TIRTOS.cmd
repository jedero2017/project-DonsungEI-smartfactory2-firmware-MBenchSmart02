/*
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== MSP_EXP432E401Y.cmd ========
 *  Define the memory block start/length for the MSP_EXP432E401Y M4
 */
--stack_size=1024   /* C stack is also used for ISR stack */

HEAPSIZE = 0x20000;  /* Size of heap buffer used by HeapMem */

MEMORY
{
    FLASH (RX) : origin = 0x00004000, length = 0x000FC000
    SRAM (RWX) : origin = 0x20000000, length = 0x00040000
}

/* Section allocation in memory */

SECTIONS
{
    .text   :   > FLASH
    .const  :   > FLASH
    .cinit  :   > FLASH
    .pinit  :   > FLASH
    .init_array : > FLASH

    .TI.ramfunc : {} load=FLASH, run=SRAM, table(BINIT)
    .data   :   > SRAM
    .bss    :   > SRAM
    .sysmem :   > SRAM

    /* Heap buffer used by HeapMem */
    .priheap   : {
        __primary_heap_start__ = .;
        . += HEAPSIZE;
        __primary_heap_end__ = .;
    } > SRAM align 8

    .stack  :   > SRAM (HIGH)
}
