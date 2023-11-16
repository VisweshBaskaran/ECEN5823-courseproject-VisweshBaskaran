/***********************************************************************
 * @file        timers.h
 * @brief       header/interface file for LETimer0
 *
 * @author      Aditi Nanaware, aditi.nanaware@colorado.edu
 * @date        Sep 8, 2023
 *
 * @institution University of Colorado Boulder (UCB)
 * @course      ECEN 5823-001: IoT Embedded Firmware
 * @instructor  David Sluiter
 *
 * @assignment  Assignment 3 - Si7021 and Load Power Management Part 1
 * @due         Sep 22, 2023 
 *
 * @resources   Lecture Slides
 *
 */


#ifndef SRC_TIMERS_H_
#define SRC_TIMERS_H_


/*
@author         Aditi Nanaware
@function name  initLETIMER0
@brief          functionality of initializating the TIMER0 depending upon the
                Energy Modes is implemented in this function.
@param          none
@return         void
@resources      Lecture Slides
*/
void initLETIMER0();

/*
@author         Aditi Nanaware
@function name  timerWaitUs_polled
@brief          functionality of providing required delay
                (less that the period of the LETIMER0)
                is implemented via polling in this function.
@param          uint32_t (required delay in us)
@return         void
@resources      Lecture Slides
*/
//void timerWaitUs_polled(uint32_t us_wait);

/*
@author         Aditi Nanaware
@function name  timerWaitUs_irq
@brief          functionality of providing required delay
                (less that the period of the LETIMER0)
                is implemented via interrupt in this function.
@param          uint32_t (required delay in us)
@return         void
@resources      Lecture Slides
*/
void timerWaitUs_irq(uint32_t us_wait);
/*
@author         Aditi Nanaware
@function name  calculate_count
@brief          functionality of calculate the count/ number of
                ticks to wait of LETIMER0 to which covers the
                required delay is implemented in this function.
@param          uint32_t (requireed delay in us)
@return         void
@resources      Lecture Slides
*/
uint32_t calculate_count(uint32_t cnt);

#endif /* SRC_TIMERS_H_ */
