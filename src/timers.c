/***********************************************************************
 * @file        timers.c
 * @brief       This file contains all the initializations required for
                configuring the LETIMER0 for our design.
 *
 * @date        Sep 8, 2023
 * @author      Aditi Nanaware
 * @Student     Aditi Nanaware, aditi.nanaware@colorado.edu
 *
 * @institution University of Colorado Boulder (UCB)
 * @course      ECEN 5823: IoT Embedded Firmware
 * @instructor  David Sluiter
 *
 * @assignment  Assignment 3 - Si7021 and Load Power Management Part 1
 * @due         Sep 22, 2023 
 *
 * @resources   Lecture Slides
****************************************************************************/

#include "em_letimer.h"
#include "em_cmu.h"
#include "em_assert.h"

//Include logging for this file
//#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"
#include "app.h"

void initLETIMER0()
{
  uint32_t temp;
  /*
   Data structure to be passed to LETIMER_Init ()
   @function used to set LETIMER0_CTRL reg bits and other values
   */
  const LETIMER_Init_TypeDef letimerInitData =
  {
    false,              // enable; don't enable when init completes, we'll enable last
    false,               // debugRun; useful to have the timer running when single-stepping in the debugger
    true,               // comp0Top; load COMP0 into CNT on underflow
    false,              // bufTop; don't load COMP1 into COMP0 when REP0==0
    0,                  // out0Pol; 0 default output pin value
    0,                  // out1Pol; 0 default output pin value
    letimerUFOANone,    // ufoa0; no underflow output action
    letimerUFOANone,    // ufoa1; no underflow output action
    letimerRepeatFree,  // repMode; free running mode i.e. load & go forever
    0                   // COMP0(top) Value, I calculate this below
  };

  // Initialize the LETIMER0
  LETIMER_Init (LETIMER0, &letimerInitData);

  // Load COMP0 (top) and COMP1
  #ifdef LOWEST_ENERGY_MODE
      #if LOWEST_ENERGY_MODE==3
        LETIMER_CompareSet (LETIMER0,   0, VALUE_TO_LOAD_ULF);
      #else
        LETIMER_CompareSet (LETIMER0,   0, VALUE_TO_LOAD_LF);
      #endif
  #endif

  //LETIMER_CompareSet (LETIMER0,   1, VALUE_FOR_COMP1);

  // Clear all IRQ flags in the LETIMER0 IF status register
  // punch them all down
  LETIMER_IntClear (LETIMER0, 0xFFFFFFFF);

  // Set UF and COMP1 in LETIMER0_IEN
  //so that the timer will generate IRQs to the NVIC.
  temp = LETIMER_IEN_UF;
  LETIMER_IntEnable (LETIMER0, temp);

  // Enable the timer to starting counting down
  LETIMER_Enable (LETIMER0, true);

  /*
  // Test code:
  // read it a few times to make sure it's running within the range of values we expect
  temp = LETIMER_CounterGet (LETIMER0);
  temp = LETIMER_CounterGet (LETIMER0);
  temp = LETIMER_CounterGet (LETIMER0);
 */
}

uint32_t calculate_count(uint32_t cnt)
{
  int count = LETIMER_CounterGet(LETIMER0);
  //Calculate the count to wait till, taking into account
  //LETIMER0 is down counter and reloads after underflow
  if((count - (int)cnt) < 0)
    return (uint32_t)(TIMER_PERIOD_CNT - (count - cnt));
  else
    return (uint32_t)(count-cnt);
}

//void timerWaitUs_polled(uint32_t us_wait)
//{
//  //Convert the time from us in ms
//  uint32_t ms_wait = us_wait/1000;
//
//  uint32_t count = 0;
//
//  //Range check (high bound) and clamp it to TimePeriod -1
//  if(ms_wait > LETIMER_PERIOD_MS)
//    {
//       ms_wait = LETIMER_PERIOD_MS;
//       count = TIMER_PERIOD_CNT -1;
//    }
//
//  //Range check (Low bound)
//  else if( ms_wait < MIN_TIMER_PERIOD_MS)
//    {
//      return;
//    }
//  //If the requested delay is in the range then calculate the
//  //Required count and wait fpr that much period
//  else if(ms_wait <= LETIMER_PERIOD_MS)
//    {
//        count = (ms_wait* ACTUAL_CLK_FREQ)/1000;
//    }
//
//   //Get the number of ticks to wait
//   uint32_t count_needed = calculate_count(count);
//
//   //Logic to wait for the required number of ticks
//   uint32_t current_cnt  = LETIMER_CounterGet(LETIMER0);
//   while(current_cnt != count_needed)
//     {
//       current_cnt  = LETIMER_CounterGet(LETIMER0);
//     }
//}

void timerWaitUs_irq(uint32_t us_wait)
{

  //Convert the time from us in ms
  uint32_t ms_wait = us_wait /1000;

  uint32_t count = 0;
  //Range check (high bound) and clamp it to TimePeriod -1
    if(ms_wait > LETIMER_PERIOD_MS)
      {
         ms_wait = LETIMER_PERIOD_MS;
         count = TIMER_PERIOD_CNT -1;
      }

    //Range check (Low bound)
//    else if( ms_wait < MIN_TIMER_PERIOD_MS)
//      {
//        return;
//      }
    //If the requested delay is in the range then calculate the
    //Required count and wait fpr that much period
    else if(ms_wait <= LETIMER_PERIOD_MS)
      {
          count = (ms_wait* ACTUAL_CLK_FREQ)/1000;
      }

     //Get the number of ticks to wait
     uint32_t count_needed = calculate_count(count);

     LETIMER_IntClear(LETIMER0, LETIMER_IFC_COMP1);
     LETIMER_CompareSet(LETIMER0,1,count_needed);
     LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP1);

}

