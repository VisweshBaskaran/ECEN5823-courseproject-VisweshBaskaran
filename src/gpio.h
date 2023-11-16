/*
   gpio.h
  
    Created on: Dec 12, 2018
        Author: Dan Walkes

    Updated by Dave Sluiter Sept 7, 2020. moved #defines from .c to .h file.
    Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.

    Editor: Feb 26, 2022, Dave Sluiter
    Change: Added comment about use of .h files.

 *
 * Student edit: Add your name and email address here:
 * @student    Aditi Nanaware, aditi.nanaware@Colorado.edu
 *
 
 */


// Students: Remember, a header file (a .h file) generally defines an interface
//           for functions defined within an implementation file (a .c file).
//           The .h file defines what a caller (a user) of a .c file requires.
//           At a minimum, the .h file should define the publicly callable
//           functions, i.e. define the function prototypes. #define and type
//           definitions can be added if the caller requires theses.

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "em_gpio.h"

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_

//Defining SI702 Sensor Port and Pin
#define Sensor_Enable_Port gpioPortD
#define Sensor_Enable 15

#define EXTCOMIN gpioPortD
#define EXTCOMINPin 13

#define PB0_PORT gpioPortF
#define PB0_PIN 6

#define PB1_PORT  gpioPortF
#define PB1_PIN  7

// Function prototypes
/*
@function name gpioInit
@brief  functionality to set DriveStrength and PinMode is
        implemented in this function
@param   none
@return  void
*/
void gpioInit();

/*
@function name gpioLed0SetOn
@brief  functionality to turn LED0 On is
        implemented in this function
@param   none
@return  void
*/
void gpioLed0SetOn();

/*
@function name gpioLed0SetOff
@brief  functionality to turn LED0 Off is
        implemented in this function
@param   none
@return  void
*/
void gpioLed0SetOff();

/*
@function name gpioLed1SetOn
@brief  functionality to turn LED1 On is
        implemented in this function
@param   none
@return  void
*/
void gpioLed1SetOn();

/*
@function name gpioLed1SetOff
@brief  functionality to turn LED1 Off is
        implemented in this function
@param   none
@return  void
*/
void gpioLed1SetOff();

void gpioSetDisplayExtcomin(bool value);

#endif /* SRC_GPIO_H_ */
