/***********************************************************************
 * @file         scheduler.h
 * @brief        header/interface file for schedular
 *
 * @author       Aditi Nanaware, aditi.nanaware@colorado.edu
 * @date         Sep 19, 2023
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
 *************************************************************************/

#ifndef SRC_SCHEDULER_H_
#define SRC_SCHEDULER_H_

#include "ble.h"

#include <stdint.h>

/*
 Enum is defined which has list of events
 Only event here for our functionality is
 to get the temperature reading
 */
typedef enum
{
  evtNoevt = 0,
  evtgetTemperature = 1,
  evtCOMP1interrupt = 2,
  evtI2CTransferDone = 4,
  evtPB0Push = 8,
  evtPB0Release = 16,
  evtPB1Push = 32,
  evtPB1Release = 64,
  evtPB1PB0Pushed = 128,
  evtGesture = 256
}event_list;


typedef enum
{
  State_WaitforUF,
  State_I2CWrite,
  State_WaitforI2CComplete,
  State_I2CRead,
  State_ReaDTeMP,
}State;

typedef enum
{
 State_CheckGesture,
 State_Return_to_Check,
}GESTURE_STATE;

/*
@author         Aditi Nanaware
@function name  schedularPushButtonEvent
@brief          functionality of setting an event implemented in this function
                based upon the state of the push button.
@param          none
@return         void
@resources      Lecture Slides
*/
void schedularPushButtonEvent();

void schedularPushButton1Event();

/*
@author         Aditi Nanaware
@function name  schedularSetTempEvent
@brief          functionality of setting an event implemented in this function.
                Event to get Temperature Reading is set in this function.
@param          none
@return         void
@resources      Lecture Slides
*/
void schedularSetTempEvent();

/*
@author         Aditi Nanaware
@function name  schedularSetCOMP1Event
@brief          functionality of setting an event implemented in this function.
                Event to set COMP1 interrupt event is implemenmted in this function.
@param          none
@return         void
@resources      Lecture Slides
*/
void schedularSetCOMP1Event();

/*
@author         Aditi Nanaware
@function name  schedularSetI2CEvent
@brief          functionality of setting an event implemented in this function.
                Event to set I2C event is implemenmted in this function.
@param          none
@return         void
@resources      Lecture Slides
*/
void schedularSetI2CEvent();

/*
@author         Aditi Nanaware
@function name  getNextEvent
@brief          This function returns next event scheduled.
                Fucntion checks if the getTemperature is set
                and returns the event.
@param          none
@return         void
@resources      Lecture Slides
*/
//uint32_t getNextEvent();

/*
@author         Aditi Nanaware
@function name  Temperature_State_Machine
@brief          This function implements state machine to read
                the temperature sensor value periodically
@param          none
@return         void
@resources      Lecture Slides
*/
void schedulerSetGestureEvent();
void Temperature_State_Machine();
void Gesture_State_Machine();
void Gesture_Handler();

typedef enum
{
  Open = 0,
  Services_Discovered = 1,
  Characteristics_Discovered = 2,
  Enable_Notification = 3,
  Idle = 4,
  Discover_Service_for_Button= 8,
  Discover_Char_For_Button= 16,
  SET_Notification_For_Button = 32,
  EN_Notification_For_Button = 164
}Discovery_STATE;

typedef struct
{
   uint8_t uuid_data[2];

}uuid;

/*
@author         Aditi Nanaware
@function name  Discovery_State_Machine
@brief          This function implements state machine to for Client
@param          none
@return         void
@resources      Lecture Slides
*/
void Discovery_State_Machine();



#endif /* SRC_SCHEDULER_H_ */
