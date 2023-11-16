/***********************************************************************
 * @file        irq.c
 * @brief       This file contains all the interrupt service routines
 *              required for the project
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

#include "irq.h"
#include "em_letimer.h"
#include "em_core.h"
#include "em_i2c.h"
#include <stdint.h> // For data type definitions like uint32_t
#include <stdbool.h> // For Boolean data type (true/false)
#include "gpio.h"
#include "scheduler.h"
#include "app.h"

#define INCLUDE_LOG_DEBUG (1)
//#ifdef INCLUDE_LOG_DEBUG
#include "src/log.h"
//#endif

//Declare variable which stores the current Interrupt flag status and
//turns off all the Interrupts
CORE_DECLARE_IRQ_STATE;

uint32_t rollover = 0;

I2C_TransferReturn_TypeDef transferStatus;

/*
@author         Aditi Nanaware
@function name  LETIMER0_IRQHandler
@brief          functionality of handling the Underflow and COMP1 interrupts
                is implemented in this function. LED os turns ON or OFF
                depending upon the interrupt occured.
@param          void
@return         void
@resources      Lecture Slides
*/
void LETIMER0_IRQHandler(void)
{
  CORE_ENTER_CRITICAL();

  //First: determine source of IRQ
  uint32_t flags = LETIMER_IntGetEnabled(LETIMER0);

  //Second: Clear source of IRQ set in step 3
  LETIMER_IntClear(LETIMER0, flags);

  //Third: Perform whatever processing is required
  //Implemented below logic to schedule events based on interrupts
  if(flags & LETIMER_IF_UF)
    {
      schedularSetTempEvent();
      rollover++;
    }

  if(flags & LETIMER_IF_COMP1)
    {
      LETIMER_IntDisable(LETIMER0, LETIMER_IEN_COMP1);
      schedularSetCOMP1Event();
    }

  CORE_EXIT_CRITICAL();
}

/*
  CREDIT: Fixed millisecond return value with help of Visweshwaran Baskaran
          Borrowed whole "return....." line
 **/
uint32_t letimerMilliseconds()
{
  //return a count of milliseconds since power up
  //return ((rollover*TIMER_PERIOD_CNT)+(TIMER_PERIOD_CNT-LETIMER0_count));
  return (rollover + ((LETIMER_CompareGet(LETIMER0, 0) - LETIMER_CounterGet(LETIMER0))/LETIMER_CompareGet(LETIMER0, 0)))*LETIMER_PERIOD_MS;
}

void I2C0_IRQHandler(void)
{

  transferStatus = I2C_Transfer(I2C0);

  if (transferStatus == i2cTransferDone) {
      schedularSetI2CEvent();
    }

    if (transferStatus < i2cTransferDone)
    {
        LOG_ERROR("I2C_Transfer Failed: %d",transferStatus);
    }
}

void GPIO_EVEN_IRQHandler()
{
  CORE_ENTER_CRITICAL();
  // Check if the interrupt was triggered by the button
  uint32_t flags = GPIO_IntGetEnabled() & 0x55555555;
  GPIO_IntClear(flags);
  // Handle PB0 interrupt
  if (flags & (1 << PB0_PIN))
     {
       schedularPushButtonEvent();
     }

  CORE_EXIT_CRITICAL();
}

void GPIO_ODD_IRQHandler()
{
  CORE_ENTER_CRITICAL();

  // Check if the interrupt was triggered by the button
  uint32_t flags = GPIO_IntGetEnabled() & 0xAAAAAAAA;
  GPIO_IntClear(flags);
  // Handle PB0 interrupt
  if (flags & (1 << PB1_PIN))
     {
      schedularPushButton1Event();
     }
  CORE_EXIT_CRITICAL();
}


I2C_TransferReturn_TypeDef gettransferStatus()
{
  return transferStatus;
}
