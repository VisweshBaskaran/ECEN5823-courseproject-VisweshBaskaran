/***********************************************************************
 * @file        oscillators.c
 * @brief       This file contains all the initializations required for
                configuring the oscillators for our design.
 *
 * @date        Sep 8, 2023
 * @author      Aditi Nanaware
 * @Student     Aditi Nanaware, aditi.nanaware@colorado.edu
 *
 * @institution University of Colorado Boulder (UCB)
 * @course      ECEN 5823: IoT Embedded Firmware
 * @instructor  David Sluiter
 *
 * @assignment  Assignment 2 - Managing Energy Modes
 * @due         Sep 15, 2023
 *
 * @resources   Lecture Slides
****************************************************************************/

#include "oscillators.h"
#include "em_cmu.h"
#include "app.h"

void oscillator_init()
{
  /*
   *
  //Section intentionally kept for debug purpose

  //Temporary variables for testing purpose

  uint32_t temp1;
  uint32_t temp2;
  uint32_t temp3;
  uint32_t temp4;
  */
/*
   Using ULFRCO for EM3
   Using LFXO for EM0, EM1, EM2
 */
#ifdef LOWEST_ENERGY_MODE
    //For EM3
    #if LOWEST_ENERGY_MODE==3
        CMU_OscillatorEnable(cmuOsc_ULFRCO,true,true);
        CMU_ClockSelectSet (cmuClock_LFA, cmuSelect_ULFRCO);
        CMU_ClockDivSet(cmuClock_LETIMER0, cmuClkDiv_1);
    #else
    //For EM0, EM1. EM2
        CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
        CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
        CMU_ClockDivSet(cmuClock_LETIMER0, cmuClkDiv_4);
    #endif
#endif

//Enabling clock for LFA and LETIMER0
CMU_ClockEnable(cmuClock_LFA,true);
CMU_ClockEnable(cmuClock_LETIMER0,true);

/*
// Testing to see if we get the values that we configured for our oscillators
//in the above code
temp1 =CMU_ClockSelectGet(cmuClock_LFA);
temp2 = CMU_ClockDivGet(cmuClock_LETIMER0);
temp3 = CMU_ClockFreqGet(cmuClock_LFA);
temp4 = CMU_ClockFreqGet(cmuClock_LETIMER0);
*/
}


