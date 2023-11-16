/***********************************************************************
 * @file        ble.h
 * @brief       This file contains all the ble declarations
 *              required for the project
 *
 * @date        Oct 3, 2023
 * @author      Aditi Nanaware
 * @Student     Aditi Nanaware, aditi.nanaware@colorado.edu
 *
 * @institution University of Colorado Boulder (UCB)
 * @course      ECEN 5823: IoT Embedded Firmware
 * @instructor  David Sluiter
 *
 * @assignment  Assignment 3 - Si7021 and Load Power Management Part 1
 * @due         Oct 3, 2023
 *
 * @resources   Lecture Slides
****************************************************************************/

#ifndef SRC_BLE_H_
#define SRC_BLE_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
// added
#include "i2c.h"

//Includes related to Bluetooth
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "sl_status.h"
#include "src/ble_device_type.h"

// This is the number of entries in the queue. Please leave
// this value set to 16.
#define QUEUE_DEPTH      (16)
// Student edit:
//   define this to 1 if your design uses all array entries
//   define this to 0 if your design leaves 1 array entry empty
#define USE_ALL_ENTRIES  (1)

#define UINT8_TO_BITSTREAM(p, n) { *(p)++ = (uint8_t)(n); }
#define UINT32_TO_BITSTREAM(p, n) { *(p)++ = (uint8_t)(n); *(p)++ = (uint8_t)((n) >> 8); \
                                    *(p)++ = (uint8_t)((n) >> 16); *(p)++ = (uint8_t)((n) >> 24); }

#define UINT32_TO_FLOAT(m, e) (((uint32_t)(m) & 0x00FFFFFFU) | (uint32_t)((int32_t)(e) << 24))

#define sl_bt_gatt_public_addr (0)

#define MIN_ADV_INTERVAL 250
#define MAX_ADV_INTERVAL 250
#define ADV_DURATION 0
#define MAX_ADV_EVENTS 0

#ifdef DEVICE_IS_BLE_SERVER
  #if (DEVICE_IS_BLE_SERVER == 0)
    #define SCAN_PASSIVE (0)
    #define CONNECTION_MIN_INTERVAL 75
    #define CONNECTION_MAX_INTERVAL 75
    #define SLAVE_LATENCY 300
    #define CONNECTION_TIME_INTERVAL 75
    //Setting the Supervision timeout to a value greater than (1 + slave latency) * (connection_interval * 2).
    // (1+4)*(75*2) =  750 ... so setting it to 760 (value greater than 750)
    #define SUPERVISOR_TIMEOUT 825
    #define MAX_CE_LEN 4
    #define MIN_CE_LEN 0

    #define SCAN_INTERVAL (80) //for 50 ms
    #define SCAN_WINDOW (40)  //for 25 ms
  #else
    #define CONNECTION_MIN_INTERVAL 75
    #define CONNECTION_MAX_INTERVAL 75
    #define SLAVE_LATENCY 300
    #define CONNECTION_TIME_INTERVAL 75
    //Setting the Supervision timeout to a value greater than (1 + slave latency) * (connection_interval * 2).
    // (1+4)*(75*2) =  750 ... so setting it to 760 (value greater than 750)
    #define SUPERVISOR_TIMEOUT 760
    #define MAX_CE_LEN 0
    #define MIN_CE_LEN 0
  #endif
#endif


#define SECOND_TIMER_FREQ (8192)
#define SECOND_TIMER_HANDLE (1)
// BLE Data Structure, save all of our private BT data in here.
// Modern C (circa 2021 does it this way)
// typedef ble_data_struct_t is referred to as an anonymous struct definition
typedef struct
{
// values that are common to servers and clients
bd_addr myAddress;
uint8_t myAddressType;

// values unique for server
// The advertising set handle allocated from Bluetooth stack.
uint8_t advertisingSetHandle;
bool connection_open; // true when in an open connection
bool ok_to_send_htm_indications; // true when client enabled indications
bool indication_in_flight; // true when an indication is in-flight
uint8_t connection_handle;
uint8_t security_mode;

bool passkey;
uint8_t bonding_handle;
bool button_pressed_flag;
bool button_indication;
//bool bonding;
bool bonded;
uint16_t reason;

// values unique for client
bool procedure_complete_flag;
uint32_t client_service_handle_htm;
uint32_t client_service_handle_button;
uint16_t characteristic_htm;
uint16_t characteristic_button;
uint32_t Temp_Value;

} ble_data_struct_t;

/*
@author         Aditi Nanaware
@function name  handle_ble_event
@brief          functionality of ble state machine is implemented in this
                function i.e advertise, connection open, etc
@param          sl_bt_msg_t
@return         void
@resources      Lecture Slides
*/
void handle_ble_event(sl_bt_msg_t *evt);

/*
@author         Aditi Nanaware
@function name  TemperatuetoFloat
@brief          functionality of converting temperature in c to bitstream
                to send to the client
@param          int32_t
@return         void
@resources      Lecture Slides
*/
void TemperatuetoFloat(uint32_t temperature_in_c);

/*
@author         Aditi Nanaware
@function name  getBleDataPtr
@brief          functionality of returning a pointer to the BLE private data
@param          none
@return         ble_data_struct_t
@resources      Lecture Slides
*/
ble_data_struct_t* getBleDataPtr();

/*
@author         Aditi Nanaware
@function name  FLOAT_TO_INT32
@brief          functionality of converting temperature value from uint32_t to IEEE-11073 format
@param          none
@return         int32_t
@resources      Lecture Slides
*/
int32_t FLOAT_TO_INT32(const uint8_t *value_start_little_endian);

// Modern C (circa 2021 does it this way)
// This is referred to as an anonymous struct definition.
// This is the structure of 1 queue/buffer/FIFO entry.
// When you write an entry to this queue, you write 2
// user values to the queue entry, a and b.
// The rationale for declaring 1 entry in the queue in this
// manner will become clear later in Assignment 8.
// *** Please do not change this definition!
//   The struct name type has to be: queue_struct_t
//   The first parameter has to be:  uint8_t       a;
//   The second parameter has to be: uint16_t      b;
// *** The autograder (i.e. the testbench) uses this struct!

/*
Queue is made of these struct elements
*/
typedef struct
{
  uint16_t charHandle; // Char handle from gatt_db.h
  size_t bufferLength; // Length of buffer in bytes to send
  uint8_t buffer[5]; // The actual data buffer for the indication.
  // Need space for HTM (5 bytes) and button_state (2 bytes)
  // indications, array [0] holds the flags byte.

//  uint8_t       a; // An 8-bit user value in 1 queue entry
//  uint16_t      b; // A 16-bit user value in 1 queue entry

} queue_struct_t;


// Function prototypes. The autograder (i.e. the testbench) only uses these
// functions to test your design. Please do not change these definitions.

/*
@author�Aditi Nanaware
@function name write_queue
@brief  functionality of writing to the circular buffer is
        implemented in this function
@param   uint8_t a, uint16_t b,  input to the circular buffer
@return  true when error occurred while writing to the circular buffer
         false when write to the circular buffer is successful
@resources none
*/
// DOS bool write_queue (uint16_t charHandle, size_t bufferLength, uint8_t buffer[5]);
bool write_queue (uint16_t charHandle, size_t bufferLength, uint8_t *buffer);

/*
@author�Aditi Nanaware
@function name read_queue
@brief  functionality of reading from the circular buffer is
        implemented in this function
@param   uint16_t *charHandle, size_t *bufferLength, uint8_t *buffer
         pointers to store the returned element from the circular buffer
@return  true when error occurred while reading from the circular buffer
         false when read from the circular buffer is successful
@resources none
*/
bool read_queue (uint16_t *charHandle, size_t *bufferLength, uint8_t *buffer);

/*
@author�Aditi Nanaware
@function name get_queue_status
@brief  functionality of getting current status of the circular buffer i.e.
        wptr, rptr, full and empty; is implemented in this function
@param   uint32_t *_wptr, uint32_t *_rptr, bool *_full, bool *_empty,
         pointers to store the returned status of the circular buffer
@return  void
@resources none
*/
void     get_queue_status (uint32_t *_wptr, uint32_t *_rptr, bool *_full, bool *_empty);

/*
@author�Aditi Nanaware
@function name get_queue_depth
@brief  functionality of getting current length of the circular buffer
        is implemented in this function
@param   void
@return  uint32_t, current length of the circular buffer
@resources none
*/
uint32_t get_queue_depth (void);

void handleSystemSsofttimer(ble_data_struct_t *BleData);

#endif /* SRC_BLE_H_ */
