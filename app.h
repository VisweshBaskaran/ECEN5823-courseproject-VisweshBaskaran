/***************************************************************************//**
 * @file
 * @brief Application interface provided to main().
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * Editor: Feb 26, 2022, Dave Sluiter
 * Change: Added comment about use of .h files.
 *
 *
 *
 * Student edit: Add your name and email address here:
 * @student    Aditi Nanaware, aditi.nanaware@Colorado.edu
 *
 *
 ******************************************************************************/

// Students: Remember, a header file (a .h file) defines an interface
//           for functions defined within an implementation file (a .c file).
//           The .h file defines what a caller (a user) of a .c file requires.
//           At a minimum, the .h file should define the publicly callable
//           functions, i.e. define the function prototypes. #define and type
//           definitions can be added if the caller requires theses.

#ifndef APP_H
#define APP_H

// Define Energy Mode here
#define LOWEST_ENERGY_MODE 2

/*******************************************************
LFX0 == EM0, EM1, EM2 = 32768 Hz

Timer duration is 2250 ms
LFXO freq = 32,768 Hz
Prescaler value = 4
Actual timer clk freq = 32,768 / 4 = 8,192 Hz
Actual timer clk period in sec = 0.000122070 sec
Value to load into the timer:
= duration_in_ms / 1000 / clk_period_in_secs
= 2250 / 1000 / 0.000122070
= 18,432
sanity check: 18,432 * 0.000122070 sec = 2.24999424
********************************************************/
/********************************************************
ULFRCO == EM3 = 1000 Hz

Timer duration is 2250 ms
LFXO freq = 1000 Hz
Prescaler value = 1
Actual timer clk freq = 1000 / 1 = 1000 Hz
Actual timer clk period in sec = 0.001 sec
Value to load into the timer:
= duration_in_ms / 1000 / clk_period_in_secs
= 2250 / 1000 / 0.001
= 2250
sanity check: 2250 * 0.0001 sec = 2.25
********************************************************/

#ifdef LOWEST_ENERGY_MODE
    #if LOWEST_ENERGY_MODE==3
        #define PRESCALER_VALUE 1 //design decision
        #define ACTUAL_CLK_FREQ (1000/PRESCALER_VALUE) // = 8192 Hz
        //#define ACTUAL_CLK_PERIOD (float)(1/ACTUAL_CLK_FREQ) // = is it a floating point number or 0?
        #define LETIMER_PERIOD_MS 3000
        #define MIN_TIMER_PERIOD_MS 1
        #define LETIMER_ON_TIME_MS 175
        #define VALUE_TO_LOAD_ULF (LETIMER_PERIOD_MS*ACTUAL_CLK_FREQ)/1000
        #define TIMER_PERIOD_CNT VALUE_TO_LOAD_ULF
        #define MIN_TIMER_TICK_CNT (MIN_TIMER_PERIOD_MS*ACTUAL_CLK_FREQ)/1000
        //COMP1 = 175
        #define VALUE_FOR_COMP1 ((LETIMER_ON_TIME_MS*ACTUAL_CLK_FREQ)/1000)
    #else
        #define PRESCALER_VALUE 4 //design decision
        #define ACTUAL_CLK_FREQ (32768/PRESCALER_VALUE) // = 8192 Hz
        //#define ACTUAL_CLK_PERIOD (float)(1/ACTUAL_CLK_FREQ) // = is it a floating point number or 0?
        #define LETIMER_PERIOD_MS 3000
        #define MIN_TIMER_PERIOD_MS ((1000*PRESCALER_VALUE) /(32768))
        //24576 is value to load for 3 sec
        #define LETIMER_ON_TIME_MS 175
        #define VALUE_TO_LOAD_LF (LETIMER_PERIOD_MS*ACTUAL_CLK_FREQ)/1000
        #define TIMER_PERIOD_CNT VALUE_TO_LOAD_LF
        #define MIN_TIMER_TICK_CNT (MIN_TIMER_PERIOD_MS*ACTUAL_CLK_FREQ)/1000
        //COMP1 = 1434
        #define VALUE_FOR_COMP1 ((LETIMER_ON_TIME_MS*ACTUAL_CLK_FREQ)/1000)
    #endif
#endif

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
void app_init(void);

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
void app_process_action(void);

#endif // APP_H
