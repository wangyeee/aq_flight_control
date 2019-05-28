/*
    This file is part of AutoQuad.

    AutoQuad is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AutoQuad is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with AutoQuad.  If not, see <http://www.gnu.org/licenses/>.

    Copyright (c) 2011-2014  Bill Nesbitt
 */

#include "aq.h"
#include "fpu.h"
#include "aq_init.h"
#include "rcc.h"
#include "serial.h"
#include "motors.h"
#include "util.h"
#include <CoOS.h>

volatile unsigned long counter;
volatile unsigned long minCycles = 0xFFFFFFFF;

int main(void) {
    fpuInit();      // setup FPU context switching

    rccConfiguration();

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    CoInitOS();

    aqInitStack = aqStackInit(AQINIT_STACK_SIZE, "INIT");

    CoCreateTask(aqInit, (void *)0, AQINIT_PRIORITY, &aqInitStack[AQINIT_STACK_SIZE-1], AQINIT_STACK_SIZE);
    CoStartOS();

    return 0;
}

// self calibrating idle timer loop
void CoIdleTask(void* pdata) {
    uint32_t thisCycles, lastCycles = 0;
    volatile uint32_t cycles;
    volatile uint32_t *DWT_CYCCNT = (uint32_t *)0xE0001004;
    volatile uint32_t *DWT_CONTROL = (uint32_t *)0xE0001000;
    volatile uint32_t *SCB_DEMCR = (uint32_t *)0xE000EDFC;

    *SCB_DEMCR = *SCB_DEMCR | 0x01000000;
    *DWT_CONTROL = *DWT_CONTROL | 1; // enable the counter

    while (1) {
#ifdef UTIL_STACK_CHECK
        utilStackCheck();
#endif
        counter++;

        thisCycles = *DWT_CYCCNT;
        cycles = thisCycles - lastCycles;
        lastCycles = thisCycles;

        // record shortest number of instructions for loop
        if (cycles < minCycles)
            minCycles = cycles;
    }
}

void CoStkOverflowHook(OS_TID taskID) {
    // Process stack overflow here
    while (1)
        ;
}

#ifdef DEBUG_ENABLED
#include <stdio.h>
void HardFault_RegDump(unsigned int* stack) {
    __ASM volatile("BKPT #01");
    printf("SCB->HFSR = 0x%08x\n", (unsigned int) SCB->HFSR);
    if ((SCB->HFSR & (1 << 30)) != 0) {
        unsigned int cfsr = SCB->CFSR;
        if((cfsr & 0xFFFF0000) != 0) {
            printf("Usage error: CFSR = 0x%08X\n", cfsr);
        }
        if((cfsr & 0xFF00) != 0) {
            printf("Bus fault error: CFSR = 0x%08X\n", cfsr);
        }
        if((cfsr & 0xFF) != 0) {
            printf("Memory management error: CFSR = 0x%08X\n", cfsr);
        }
        printf("SCB->CFSR = 0x%08X\n", cfsr);
    }
    printf("R0  = 0x%08X\n", stack[r0]);
    printf("R1  = 0x%08X\n", stack[r1]);
    printf("R2  = 0x%08X\n", stack[r2]);
    printf("R3  = 0x%08X\n", stack[r3]);
    printf("R12 = 0x%08X\n", stack[r12]);
    printf("LR  = 0x%08X\n", stack[lr]);
    printf("PC  = 0x%08X\n", stack[pc]);
    printf("PSR = 0x%08X\n", stack[psr]);
    __ASM volatile("BKPT #02");
    for (;;);
}

void HardFault_Handler(void) {
    __asm("TST lr, #4");
    __asm("ITE EQ \n"
          "MRSEQ r0, MSP \n"
          "MRSNE r0, PSP");
    __asm("B HardFault_RegDump");
}
#else
void HardFault_Handler(void) {
    // to avoid the unpredictable flight in case of problems
    motorsOff();
    // Go to infinite loop when Hard Fault exception occurs
    while (1);
}
#endif

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
    //    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
    while (1)
        ;
}
#endif
