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

    Copyright (c) 2011-2014 Bill Nesbitt
*/

// NOTE: These parameters must be passed to GCC:
//
//  -fsingle-precision-constant
//

#ifndef _aq_h
#define _aq_h

#define USE_MAVLINK
#define USE_PRES_ALT        // uncomment to use pressure altitude instead of GPS
#define USE_SIGNALING       // uncomment to use external signaling events and ports
//#define HAS_QUATOS        // build including Quatos library
//#define HAS_AQ_TELEMETRY  // uncomment to include AQ native binary telemetry and command interface
//#define DIMU_VERSION  11  // uncomment to build for AQ6 hardware with DIMU add-on

#ifndef BOARD_VERSION
    #define BOARD_VERSION 9
#endif
#ifndef BOARD_REVISION
    #define BOARD_REVISION 1
#endif

#include "stm32f4xx.h"

#if BOARD_VERSION == 6
    #if BOARD_REVISION == 0
        #include "board_6_r0.h"
    #elif BOARD_REVISION == 1
        #include "board_6_r1.h"
    #endif

#elif BOARD_VERSION == 7
    #if BOARD_REVISION == 0
        #include "board_7_0.h"
    #endif

#elif BOARD_VERSION == 8
    #if BOARD_REVISION == 1 || BOARD_REVISION == 2
        #include "board_m4.h"
    #elif BOARD_REVISION == 3
        #include "board_m4_r3.h"
    #elif BOARD_REVISION == 4 || BOARD_REVISION == 5
        #include "board_m4_r5.h"
    #elif BOARD_REVISION == 6
        #include "board_m4_r6.h"
    #endif
#elif BOARD_VERSION == 9
    #if BOARD_REVISION == 1
        #include "board_sfc.h"
    #endif
#endif

#define CCM_RAM __attribute__((section(".ccm")))

#ifdef DEBUG_ENABLED
#define DBG_NO_OPT __attribute__((optimize("O0")))
#else
#define DBG_NO_OPT
#endif

#ifndef M_PI
#define M_PI   3.14159265f
#define M_PI_2   (M_PI / 2.0f)
#endif

#define RAD_TO_DEG  (180.0f / M_PI)
#define DEG_TO_RAD  (M_PI / 180.0f)

#define GRAVITY   9.80665f // m/s^2

#define AQ_US_PER_SEC  1000000

#ifndef __CROSSWORKS_ARM
#define __sqrtf sqrtf
#define CC_ALIGNED __attribute__ ((aligned (8)))
#ifndef NAN
#define NAN (__builtin_nanf(""))
#endif
#else
#define CC_ALIGNED
#endif

#ifndef bool
#define bool _Bool
#ifndef true
#define true 1
#define false 0
#endif // true,false
#endif // bool

//#define CAN_CALIB // transmit IMU data over CAN

enum dataTypesEnum {
    AQ_TYPE_DBL = 0,
    AQ_TYPE_FLT,
    AQ_TYPE_U32,
    AQ_TYPE_S32,
    AQ_TYPE_U16,
    AQ_TYPE_S16,
    AQ_TYPE_U8,
    AQ_TYPE_S8
};

extern volatile unsigned long counter;
extern volatile unsigned long minCycles;

#endif
