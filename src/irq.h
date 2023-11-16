/***********************************************************************
 * @file         irq.h
 * @brief        header/interface file for irq (Interrupts handling)
 *
 * @author       Aditi Nanaware, aditi.nanaware@colorado.edu
 * @date         Sep 8, 2023
 *
 * @institution University of Colorado Boulder (UCB)
 * @course      ECEN 5823-001: IoT Embedded Firmware
 * @instructor  David Sluiter
 *
 * @assignment  Assignment 2 - Managing Energy Modes
 * @due         Sep 15, 2023 
 *
 * @resources   Lecture Slides
 *
 ************************************************************************/


#ifndef SRC_IRQ_H_
#define SRC_IRQ_H_

#include "em_i2c.h"
#include "em_core.h"

/*
  Hash defines required for making a section critical
 */

//Defining a variable to store a copy of all the flags in interrupt
#define CORE_DECLARE_IRQ_STATE CORE_irqState_t irqState

//This should be called before entering the critical section
//(Disables the interrupts)
#define CORE_ENTER_CRITICAL() irqState = CORE_EnterCritical()

//This should be called before leaving the critical section
//(Re-enables the interrupts)
#define CORE_EXIT_CRITICAL() CORE_ExitCritical(irqState)

/*
@author         Aditi Nanaware
@function name  gettransferStatus
@brief          functionality of getting the transferStatus of I2C implemented
                command is implemented in this function
@param          none
@return         I2C_TransferReturn_TypeDef
@resources      Lecture Slides
*/
I2C_TransferReturn_TypeDef gettransferStatus();

/*
@author         Aditi Nanaware
@function name  letimerMilliseconds
@brief          functionality of getting the milliseconds since power up
                is implemented in this function
@param          none
@return         uint32_t
@resources      Lecture Slides
*/
uint32_t letimerMilliseconds();

void GPIO_EVEN_IRQHandler(void);

#endif /* SRC_IRQ_H_ */
