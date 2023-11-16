/***********************************************************************
 * @file        ble.c
 * @brief       This file contains all the ble routines
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

#define INCLUDE_LOG_DEBUG 1
#include "log.h"

#include <math.h> // need function prototype for pow()
#include "ble.h"
#include "scheduler.h"
#include "lcd.h"
#include "gpio.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Global variables for this assignment
// Declare memory for the queue/buffer/fifo,
// and the write and read pointers
queue_struct_t   my_queue[QUEUE_DEPTH]; // This is the storage for your queue


uint32_t         wptr = 0;              // write pointer
uint32_t         rptr = 0;              // read pointer

// Decide how will you handle the full condition.
// Boolean for full and empty queue indication
bool full = false;
bool empty = true;

//gattdb_button_state, sizeof(button_buffer), button_buffer
uint16_t charHandle;
size_t bufferLength;
uint8_t buffer[5]; // Assuming a maximum buffer size of 5
uint8_t button_buffer[2];

// BLE private data
ble_data_struct_t ble_data;
bd_addr serverAddress = SERVER_BT_ADDRESS;
sl_status_t sc;
uint8_t flag = 0x00;


uint8_t CHARACTERISTIC_UUID[2] = {0x1c,0x2a};
uint8_t BUTTON_CHARACTERISTIC_UUID[16] = {0x89, 0x62, 0x13, 0x2d, 0x2a, 0x65, 0xec, 0x87, 0x3e, 0x43, 0xc8, 0x38, 0x02, 0x00, 0x00, 0x00};

uint8_t SERVICE_UUID[2] = {0x09,0x18};
uint8_t BUTTON_SERVICE_UUID[16] = {0x89, 0x62, 0x13, 0x2d, 0x2a, 0x65, 0xec, 0x87, 0x3e, 0x43, 0xc8, 0x38, 0x01, 0x00, 0x00, 0x00};


// function that returns a pointer to the
// BLE private data
ble_data_struct_t* getBleDataPtr()
{
  return (&ble_data);
} // getBleDataPtr()

/*********************************************************/

// ---------------------------------------------------------------------
// Private function used only by this .c file.
// Compute the next ptr value. Given a valid ptr value, compute the next valid
// value of the ptr and return it.
// Isolation of functionality: This defines "how" a pointer advances.
// ---------------------------------------------------------------------

/*
@author�Aditi Nanaware
@function name nextPtr
@brief  functionality of how next ptr i.e. (read and write pointer s, generalized)
        should be updated is implemented in this function
@param   uint32_t, current ptr value
@return  uint32_t, nextPtr value
@resources none
 */
static uint32_t nextPtr(uint32_t ptr)
{

  // Student edit:
  // Create this function

  return ((ptr + 1) & (QUEUE_DEPTH - 1));

} // nextPtr()


// ---------------------------------------------------------------------
// Public function
// This function writes an entry to the queue if the the queue is not full.
// Input parameter "a" should be written to queue_struct_t element "a"
// Input parameter "b" should be written to queue_struct_t element "b"
// Returns bool false if successful or true if writing to a full fifo.
// i.e. false means no error, true means an error occurred.
// ---------------------------------------------------------------------

//DOS bool write_queue (uint16_t charHandle, size_t bufferLength, uint8_t buffer[5])
bool write_queue (uint16_t charHandle, size_t bufferLength, uint8_t *buffer)
{

  // Student edit:
  // Create this function
  // Decide how you want to handle the "full" condition.

  // Isolation of functionality:
  //     Create the logic for "when" a pointer advances

  // return true if writing to a full fifo.
  if(full == true)
    return true;

  //Queue is not full and we write into the queue
  my_queue[wptr].charHandle = charHandle;
  my_queue[wptr].bufferLength = bufferLength;
  // DOS  memcpy(my_queue[wptr].buffer, buffer, bufferLength);
  memcpy(&my_queue[wptr].buffer, buffer, bufferLength);

  //After writing we update the wptr
  wptr = nextPtr(wptr);

  //check for the full condition
  if(rptr == wptr)
    full = true;

  // After a write is successfull queue is no more empty if it was already empty
  empty = false;

  //Returns bool false for successful write
  return false;

} // write_queue()


// ---------------------------------------------------------------------
// Public function
// This function reads an entry from the queue, and returns values to the
// caller. The values from the queue entry are returned by writing
// the values to variables declared by the caller, where the caller is passing
// in pointers to a and b. The caller's code will look like this:
//
//   uint8_t      my8;
//   uint16_t     my16;
//   bool         status;
//
//   status = read_queue (&my8, &my16);
//
// *** If the code above doesn't make sense to you, you probably lack the
// necessary prerequisite knowledge to be successful in this course.
//
// Write the values of a and b from my_queue[rptr] to
// the memory addresses pointed at by a and b, like this :
//      *a = <something>;
//      *b = <something_else>;
// In this implementation, we do it this way because
// standard C does not provide a mechanism for a C function to return multiple
// values, as is common in perl or python.
// Returns bool false if successful or true if reading from an empty fifo.
// i.e. false means no error, true means an error occurred.
// ---------------------------------------------------------------------
bool read_queue (uint16_t *charHandle, size_t *bufferLength, uint8_t *buffer)
{

  // Student edit:
  // Create this function

  // Isolation of functionality:
  //     Create the logic for "when" a pointer advances

  //Check for empty condition i.e. nothing to read
  if(empty == true)
    return true;

  //Queue is not empty and we read from the queue
  *charHandle = my_queue[rptr].charHandle;
  *bufferLength = my_queue[rptr].bufferLength;
  // DOS memcpy(buffer, my_queue[rptr].buffer, my_queue[rptr].bufferLength);
  memcpy(buffer, &my_queue[rptr].buffer, my_queue[rptr].bufferLength);
  //Update the rptr
  rptr = nextPtr(rptr);
  //Check if the queue is now empty after the read
  if(rptr == wptr)
    empty = true;

  //After a read is successfull queue cannot be full
  full = false;

  //Return false as read was successfull
  return false;
} // read_queue()



// ---------------------------------------------------------------------
// Public function
// This function returns the wptr, rptr, full and empty values, writing
// to memory using the pointer values passed in, same rationale as read_queue()
// ---------------------------------------------------------------------
void get_queue_status (uint32_t *_wptr, uint32_t *_rptr, bool *_full, bool *_empty) {

  // Student edit:
  // Create this function

  *_wptr = wptr;
  *_rptr = rptr;
  *_full = full;
  *_empty = empty;

} // get_queue_status()



// ---------------------------------------------------------------------
// Public function
// Function that computes the number of written entries currently in the queue. If there
// are 3 entries in the queue, it should return 3. If the queue is empty it should
// return 0. If the queue is full it should return either QUEUE_DEPTH if
// USE_ALL_ENTRIES==1 otherwise returns QUEUE_DEPTH-1.
// ---------------------------------------------------------------------
uint32_t get_queue_depth() {

  // Student edit:
  // Create this function

  //If the queue is empty it should return 0
  if(empty == true)
    return 0;

  //If the queue is full it should return QUEUE_DEPTH
  if(full == true)
    return QUEUE_DEPTH;

  // computes the number of written entries currently in the queue
  return ((wptr-rptr)& (QUEUE_DEPTH - 1));

} // get_queue_depth()


/*********************************************************/
void handle_ble_event(sl_bt_msg_t *evt)
{

#ifdef DEVICE_IS_BLE_SERVER
#if (DEVICE_IS_BLE_SERVER == 1)
  switch (SL_BT_MSG_ID(evt->header))
  {
    // ******************************************************
    // Events common to both Servers and Clients
    // ******************************************************
    // --------------------------------------------------------
    // This event indicates the device has started and the radio is ready.
    // --------------------------------------------------------
    case sl_bt_evt_system_boot_id:
      //LOG_INFO("\r\n sl_bt_evt_system_boot_id");
      displayInit();
      displayPrintf(DISPLAY_ROW_NAME, "Server");
      // handle boot event

      /***************************************************************************//**
       *
       * Read the Bluetooth identity address used by the device, which can be a public
       * or random static device address.
       *
       * @param[out] address Bluetooth public address in little endian format
       * @param[out] type Address type
       *     - <b>0:</b> Public device address
       *     - <b>1:</b> Static random address
       *
       * @return SL_STATUS_OK if successful. Error code otherwise.
       *
       ******************************************************************************/
      sc = sl_bt_system_get_identity_address(&ble_data.myAddress, &ble_data.myAddressType);
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_system_get_identity_address() returned != 0 status=0x%04x", (unsigned int) sc);
        }

      /***************************************************************************//**
       *
       * Create an advertising set. The handle of the created advertising set is
       * returned in response.
       *
       * @param[out] handle Advertising set handle
       *
       * @return SL_STATUS_OK if successful. Error code otherwise.
       *
       ******************************************************************************/
      sc = sl_bt_advertiser_create_set(&ble_data.advertisingSetHandle);
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_advertiser_create_set() returned != 0 status=0x%04x", (unsigned int) sc);
        }
      displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
      /*
       *    Set the advertising timing parameters of the given advertising set. This
               setting will take effect next time that advertising is enabled.
           sl_bt_advertiser_set_timing parameters:
           - advertising set handle
           - min. adv. interval (milliseconds * 1.6)
           - max. adv. interval (milliseconds * 1.6)
           - advertisement duration
           - max. number advertisement events
       */
      sc = sl_bt_advertiser_set_timing(ble_data.advertisingSetHandle, (MIN_ADV_INTERVAL*(1.6)), (MAX_ADV_INTERVAL*(1.6)), ADV_DURATION, MAX_ADV_EVENTS);
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_advertiser_set_timing() returned != 0 status=0x%04x", (unsigned int) sc);
        }

      /******************************************************************************
           Start advertising of a given advertising set with specified discoverable and
           connectable modes
       *****************************************************************************/
      sc = sl_bt_advertiser_start(ble_data.advertisingSetHandle, sl_bt_advertiser_general_discoverable, sl_bt_advertiser_connectable_scannable);
      displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_advertiser_start() returned != 0 status=0x%04x", (unsigned int) sc);
        }
      displayPrintf(DISPLAY_ROW_BTADDR, "%x:%x:%x:%x:%x:%x ",ble_data.myAddress.addr[0],ble_data.myAddress.addr[1],ble_data.myAddress.addr[2],ble_data.myAddress.addr[3],ble_data.myAddress.addr[4],ble_data.myAddress.addr[5] );
      displayPrintf(DISPLAY_ROW_ASSIGNMENT,"A9");

      sc = sl_bt_sm_configure(0x0F, sm_io_capability_displayyesno);
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_sm_configure() returned != 0 status=0x%04x", (unsigned int) sc);
        }
      ble_data.connection_open = false;
      ble_data.ok_to_send_htm_indications = false;
      ble_data.indication_in_flight = false;
      sl_bt_sm_delete_bondings();
      break;
    case sl_bt_evt_connection_opened_id:
      //LOG_INFO("\r\n sl_bt_evt_connection_opened_id");
      // handle open event
      ble_data.connection_open = true;
      ble_data.connection_handle = evt->data.evt_connection_opened.connection;

      displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");

      /*
          Request a change in the connection parameters of a Bluetooth connection.

          sl_bt_connection_set_parameters parameters:
          (uint8_t connection,
          uint16_t min_interval,  //divide 1.25
          uint16_t max_interval,  //divide 1.25
          uint16_t latency,       // by time interval
          uint16_t timeout,       // divide 10
          uint16_t min_ce_length,
          uint16_t max_ce_length);
       */
      sc = sl_bt_connection_set_parameters(ble_data.connection_handle, (CONNECTION_MIN_INTERVAL/1.25), (CONNECTION_MAX_INTERVAL/1.25), (SLAVE_LATENCY/CONNECTION_TIME_INTERVAL), (SUPERVISOR_TIMEOUT/10), MIN_CE_LEN, MAX_CE_LEN);
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_connection_set_parameters() returned != 0 status=0x%04x", (unsigned int) sc);
        }

      /***************************************************************************//**
       *
       * Stop the advertising of the given advertising set. Counterpart with @ref
       * sl_bt_advertiser_start.
       *
       * This command does not affect the enable state of the periodic advertising
       * set, i.e., periodic advertising is not stopped.
       *
       * @param[in] handle Advertising set handle
       *
       * @return SL_STATUS_OK if successful. Error code otherwise.
       *
       ******************************************************************************/
      sc = sl_bt_advertiser_stop(ble_data.advertisingSetHandle);
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_advertiser_stop() returned != 0 status=0x%04x", (unsigned int) sc);
        }

      sc = sl_bt_system_set_soft_timer(SECOND_TIMER_FREQ, SECOND_TIMER_HANDLE ,false);
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_system_set_soft_timer()  returned != 0 status=0x%04x", (unsigned int) sc);
        }

      break;

    case sl_bt_evt_connection_closed_id:
      //LOG_INFO("\r\n sl_bt_evt_connection_closed_id");
      // handle close event
      ble_data.connection_open = false;

      sc = sl_bt_advertiser_start(ble_data.advertisingSetHandle, sl_bt_advertiser_general_discoverable, sl_bt_advertiser_connectable_scannable);

      displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
      displayPrintf(DISPLAY_ROW_TEMPVALUE, " "," " );
      displayPrintf(DISPLAY_ROW_PASSKEY, " " );
      displayPrintf(DISPLAY_ROW_ACTION, " " );
      displayPrintf(DISPLAY_ROW_9, " " );
      gpioLed1SetOff();
      gpioLed0SetOff();
      displayPrintf(DISPLAY_ROW_TEMPVALUE, ""," " );
      displayPrintf(DISPLAY_ROW_9, " " );
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_advertiser_start() returned != 0 status=0x%04x", (unsigned int) sc);
        }
      sl_bt_sm_delete_bondings();
      memset(&ble_data, 0, sizeof(ble_data_struct_t));
      break;

    case sl_bt_evt_sm_confirm_passkey_id:
      //LOG_INFO("\r\n sl_bt_evt_sm_confirm_passkey_id");
      ble_data.connection_handle = evt->data.evt_sm_confirm_passkey.connection;
     // ble_data.passkey = evt->data.evt_sm_confirm_passkey.passkey;
      ble_data.passkey = true;
      displayPrintf(DISPLAY_ROW_PASSKEY, "Passkey:%4lu", evt->data.evt_sm_confirm_passkey.passkey );
      displayPrintf(DISPLAY_ROW_ACTION, "Confirm with PB0" );

      break;

    case sl_bt_evt_sm_confirm_bonding_id:
      //LOG_INFO("\r\n sl_bt_evt_sm_confirm_bonding_id");
      ble_data.connection_handle = evt->data.evt_sm_confirm_bonding.connection;
      ble_data.bonding_handle = evt->data.evt_sm_confirm_bonding.bonding_handle;

      sc = sl_bt_sm_bonding_confirm(ble_data.connection_handle, 1);
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_sm_bonding_confirm() returned != 0 status=0x%04x", (unsigned int) sc);
        }

     // ble_data.bonding = true;

      break;

    case sl_bt_evt_connection_parameters_id:
      //handle connection parameter
      //       LOG_INFO("\r\nConnection parameters: interval=%d, latency=%d, timeout=%d",
      //                 (int) (evt->data.evt_connection_parameters.interval*1.25),
      //                 (int) (evt->data.evt_connection_parameters.latency),
      //                 (int) (evt->data.evt_connection_parameters.timeout*10));

      break;


    case sl_bt_evt_system_external_signal_id:
      // LOG_INFO("\r\n sl_bt_evt_system_external_signal_id");
      /***************************************************************************//**
       *
       * Write the value of an attribute in the local GATT database. Writing the value
       * of a characteristic of the local GATT database will not trigger notifications
       * or indications to the remote GATT client if the characteristic has a property
       * to indicate or notify and the client has enabled notification or indication.
       * Notifications and indications are sent to the remote GATT client using @ref
       * sl_bt_gatt_server_send_notification or @ref sl_bt_gatt_server_send_indication
       * commands.
       *
       * @param[in] attribute Attribute handle
       * @param[in] offset Value offset
       * @param[in] value_len Length of data in @p value
       * @param[in] value Value
       *
       * @return SL_STATUS_OK if successful. Error code otherwise.
       *
       ******************************************************************************/
      if ( (evt->data.evt_system_external_signal.extsignals == evtPB0Push) ||
          (evt->data.evt_system_external_signal.extsignals == evtPB0Release) )
        {

          button_buffer[0] = 0; // DOS: set the flags byte, this informs the receiver that this is an indication
          // The data for the indication follows the flags byte
          if(evt->data.evt_system_external_signal.extsignals  == evtPB0Push)

            button_buffer[1] = 1;

          if(evt->data.evt_system_external_signal.extsignals  == evtPB0Release)
            button_buffer[1] = 0;

          sc = sl_bt_gatt_server_write_attribute_value(
              gattdb_button_state,
              0,
              1,
              (const uint8_t*)&button_buffer[1]
          );
          if (sc != SL_STATUS_OK)
            {
              LOG_ERROR("sl_bt_gatt_server_write_attribute_value() returned != 0 status=0x%04x", (unsigned int)sc);
            }


          if(evt->data.evt_system_external_signal.extsignals)
            {
              if(evt->data.evt_system_external_signal.extsignals  & evtPB0Push)
                {
                  if(ble_data.connection_open)
                    {
                      displayPrintf(DISPLAY_ROW_9, "Button Pressed");
                    }
                  if((ble_data.passkey))
                    {
                      sl_bt_sm_passkey_confirm(ble_data.connection_handle, 1);
                    }
                  if((ble_data.button_indication) && (ble_data.connection_open))
                    {
                      if(ble_data.indication_in_flight == false)
                        {
                          sc = sl_bt_gatt_server_send_indication(ble_data.connection_handle,gattdb_button_state, sizeof(button_buffer), &button_buffer[0]);
                          if (sc != SL_STATUS_OK)
                            {
                              LOG_ERROR("sl_bt_gatt_server_send_indication() returned != 0 status=0x%04x", (unsigned int)sc);
                            } else { // DOS
                                ble_data.indication_in_flight = true;
                                //LOG_INFO("Btn Ind P sent"); // DOS
                            }
                        }
                      else
                        {
                          //store indication in buffer
                          // Call the write_queue function and check if it was successful
                          // DOS bool writeSuccess = write_queue(gattdb_button_state, 1, button_buffer); // DOS - why 1 byte, it's 2 bytes, correct?
                          bool writeSuccess = write_queue(gattdb_button_state, 2, &button_buffer[0]);
                          if (writeSuccess != false)
                            {
                              LOG_ERROR("write_queue() returned != 0 status=0x%04x", (unsigned int) sc);
                            } else {
                               // LOG_INFO("Btn Ind P queued"); // DOS
                            }
                        }
                    }
                }

              if(evt->data.evt_system_external_signal.extsignals  & evtPB0Release)
                {
                  if(ble_data.connection_open)
                    {
                      displayPrintf(DISPLAY_ROW_9, "Button Released" );
                      displayPrintf(DISPLAY_ROW_PASSKEY, "");
                      displayPrintf(DISPLAY_ROW_ACTION, "");
                    }

                  if((ble_data.button_indication) && (ble_data.connection_open))
                    {
                      if(ble_data.indication_in_flight == false)
                        {
                          sc = sl_bt_gatt_server_send_indication(ble_data.connection_handle,gattdb_button_state, sizeof(button_buffer), &button_buffer[0]);
                          if (sc != SL_STATUS_OK)
                            {
                              LOG_ERROR("sl_bt_gatt_server_send_indication() returned != 0 status=0x%04x", (unsigned int) sc);
                            } else {
                                ble_data.indication_in_flight = true;
                               // LOG_INFO("Btn Ind R sent"); // DOS
                            }
                        }
                      else
                        {
                          //store indication in buffer
                          // Call the write_queue function and check if it was successful
                          // DOS bool writeSuccess = write_queue(gattdb_button_state, 1, button_buffer); // DOS - why 1 byte, it's 2 bytes, correct?
                          bool writeSuccess = write_queue(gattdb_button_state, 2, &button_buffer[0]);
                          if (writeSuccess != false)
                            {
                              LOG_ERROR("write_queue() returned != 0 status=0x%04x", (unsigned int) sc);
                            } else {
                               // LOG_INFO("Btn Ind R queued"); // DOS
                            }
                        }
                    }
                }
            }

        }

      break;

    case sl_bt_evt_sm_bonded_id:
      //LOG_INFO("\r\n sl_bt_evt_sm_bonded_id");
      ble_data.connection_handle = evt->data.evt_sm_bonded.connection;
      ble_data.bonding_handle = evt->data.evt_sm_bonded.bonding;
      ble_data.security_mode = evt->data.evt_sm_bonded.security_mode;
      ble_data.passkey = false;
      displayPrintf(DISPLAY_ROW_CONNECTION, "Bonded");
      displayPrintf(DISPLAY_ROW_PASSKEY, " " );
      displayPrintf(DISPLAY_ROW_ACTION, " " );

      break;
    case sl_bt_evt_sm_bonding_failed_id:
      //LOG_INFO("\r\n sl_bt_evt_sm_bonding_failed_id");
      ble_data.connection_handle = evt->data.evt_sm_bonding_failed.connection;
      ble_data.reason = evt->data.evt_sm_bonding_failed.reason;
      displayPrintf(DISPLAY_ROW_CONNECTION, "BONDING FAILED");
      ble_data.passkey = false;
      break;

    case sl_bt_evt_gatt_server_characteristic_status_id:
      //LOG_INFO("\r\n sl_bt_evt_gatt_server_characteristic_status_id");
      if(evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_temperature_measurement)
        {     //write of CCCD for htm
          // client confirmation has been changed
          if(evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_client_config)
            {
              //  the changes happened is equal to the indication button pressed
              if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_server_indication)
                {

                  ble_data.ok_to_send_htm_indications = true;
                  displayPrintf(DISPLAY_ROW_9, "");
                  gpioLed0SetOn();

                }
              //  again the changes happened is the indication button pressed to close
              if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_server_disable)
                {

                  ble_data.ok_to_send_htm_indications = false;
                  displayPrintf(DISPLAY_ROW_TEMPVALUE, "Temp= "," " );
                  displayPrintf(DISPLAY_ROW_9, "");
                  gpioLed0SetOff();

                }
            }

          //write of CCCD for htm
          // client confirmation has been changed
          // client configuration has been successfully received
          if(evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_confirmation)
            {
              ble_data.indication_in_flight = false;
            }

        }

      if(evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_button_state)
        {
          if(evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_client_config)
            {
              //  the changes happened is equal to the indication button pressed
              if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_server_indication)
                {

                  ble_data.button_indication = true;
                  displayPrintf(DISPLAY_ROW_9, "Button Released");
                  gpioLed1SetOn();

                }
              //  again the changes happened is the indication button pressed to close
              if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_server_disable)
                {

                  ble_data.button_indication = false;

                  displayPrintf(DISPLAY_ROW_9, "");
                  gpioLed1SetOff();

                }
            }
          // client configuration has been successfully received
          if(evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_confirmation)
            {
              ble_data.indication_in_flight = false;
            }
        }

      break;

    case sl_bt_evt_gatt_server_indication_timeout_id:
      LOG_ERROR("\r\nLOG_ERROR: Indication timed-out. No acknowledgement received\r\n");
      ble_data.indication_in_flight = false;
      break;

    case sl_bt_evt_system_soft_timer_id:
      //LOG_INFO("\r\n sl_bt_evt_system_soft_timer_id");

      // DOS - why no test/check on which soft timer created the event????
      // DOS      displayUpdate();
      // DOS     handleSystemSsofttimer(evt,&ble_data);

      switch (evt->data.evt_system_soft_timer.handle)
      {

        case 0: // handle from lcd.c is 0
          displayUpdate();
          break;

        case SECOND_TIMER_HANDLE:
          handleSystemSsofttimer(&ble_data);
          break;

       } // switch

      break;

        default:
          break;

  } // end - switch
#else

  switch(SL_BT_MSG_ID(evt->header))
  {

    case sl_bt_evt_system_boot_id:
      //LOG_INFO("\r\n sl_bt_evt_system_boot_id");
      displayInit();
      displayPrintf(DISPLAY_ROW_NAME, "Client");
      // handle boot event
      ble_data.connection_open = false;
      //ble_data.ok_to_send_htm_indications = false;
      ble_data.procedure_complete_flag = false;
      ble_data.connection_open = false;
      ble_data.button_indication = true; // DOS - always initialize variables!!! state machine initially enables, so set true
      /***************************************************************************//**
       *
       * Read the Bluetooth identity address used by the device, which can be a public
       * or random static device address.
       *
       * @param[out] address Bluetooth public address in little endian format
       * @param[out] type Address type
       *     - <b>0:</b> Public device address
       *     - <b>1:</b> Static random address
       *
       * @return SL_STATUS_OK if successful. Error code otherwise.
       *
       ******************************************************************************/
      sc = sl_bt_system_get_identity_address(&ble_data.myAddress, &ble_data.myAddressType);
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_system_get_identity_address() returned != 0 status=0x%04x", (unsigned int) sc);
        }

      /***************************************************************************//**
       *
       * Set the scan mode on the specified PHYs. If the device is currently scanning
       * for advertising devices on PHYs, new parameters will take effect when
       * scanning is restarted.
       *
       * @param[in] phys PHYs for which the parameters are set.
       *     - <b>1:</b> 1M PHY
       *     - <b>4:</b> Coded PHY
       *     - <b>5:</b> 1M PHY and Coded PHY
       * @param[in] scan_mode @parblock
       *   Scan mode. Values:
       *     - <b>0:</b> Passive scanning
       *     - <b>1:</b> Active scanning
       *     - In passive scanning mode, the device only listens to advertising
       *       packets and does not transmit packets.
       *     - In active scanning mode, the device sends out a scan request packet
       *       upon receiving an advertising packet from a remote device. Then, it
       *       listens to the scan response packet from the remote device.
       *
       *   Default value: 0
       *   @endparblock
       *
       * @return SL_STATUS_OK if successful. Error code otherwise.
       *
       ******************************************************************************/
      sc = sl_bt_scanner_set_mode(sl_bt_gap_1m_phy,SCAN_PASSIVE);
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_scanner_set_mode() returned != 0 status=0x%04x", (unsigned int) sc);
        }
      /***************************************************************************//**
       *
       * Set the scanning timing parameters on the specified PHYs. If the device is
       * currently scanning for advertising devices on PHYs, new parameters will take
       * effect when scanning is restarted.
       *
       * @param[in] phys PHYs for which the parameters are set.
       *     - <b>1:</b> 1M PHY
       *     - <b>4:</b> Coded PHY
       *     - <b>5:</b> 1M PHY and Coded PHY
       * @param[in] scan_interval @parblock
       *   Scan interval is defined as the time interval when the device starts its
       *   last scan until it begins the subsequent scan. In other words, how often to
       *   scan
       *     - Time = Value x 0.625 ms
       *     - Range: 0x0004 to 0xFFFF
       *     - Time Range: 2.5 ms to 40.96 s
       * @param[in] scan_window @parblock
       *   Scan window defines the duration of the scan which must be less than or
       *   equal to the @p scan_interval
       *     - Time = Value x 0.625 ms
       *     - Range: 0x0004 to 0xFFFF
       *     - Time Range: 2.5 ms to 40.96 s
       *********************************************************************************/
      sc = sl_bt_scanner_set_timing(sl_bt_gap_1m_phy, SCAN_INTERVAL, SCAN_WINDOW);
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_scanner_set_timing() returned != 0 status=0x%04x", (unsigned int) sc);
        }
      /*
                 Set the default Bluetooth connection parameters. The values are valid for all
                 subsequent connections initiated by this device.

                 sl_bt_connection_set_parameters parameters:
                 (uint8_t connection,
                 uint16_t min_interval,  //divide 1.25
                 uint16_t max_interval,  //divide 1.25
                 uint16_t latency,       // by time interval
                 uint16_t timeout,       // divide 10
                 uint16_t min_ce_length,
                 uint16_t max_ce_length);
       */
      sc = sl_bt_connection_set_default_parameters((CONNECTION_MIN_INTERVAL/1.25), (CONNECTION_MAX_INTERVAL/1.25), (SLAVE_LATENCY/CONNECTION_TIME_INTERVAL), (SUPERVISOR_TIMEOUT/10), MIN_CE_LEN, MAX_CE_LEN);

      /***************************************************************************
       * Start the GAP discovery procedure to scan for advertising devices on the
       * specified scanning PHYs.
       *************************************************************************/
      sc = sl_bt_scanner_start(sl_bt_gap_1m_phy, sl_bt_scanner_discover_generic);
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_scanner_start() returned != 0 status=0x%04x", (unsigned int) sc);
        }

      displayPrintf(DISPLAY_ROW_CONNECTION, "Discovering");

      displayPrintf(DISPLAY_ROW_BTADDR, "%x:%x:%x:%x:%x:%x ",ble_data.myAddress.addr[0],ble_data.myAddress.addr[1],ble_data.myAddress.addr[2],ble_data.myAddress.addr[3],ble_data.myAddress.addr[4],ble_data.myAddress.addr[5] );
      displayPrintf(DISPLAY_ROW_ASSIGNMENT,"A9");

      sc = sl_bt_sm_configure(0x0F, sm_io_capability_displayyesno);
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_sm_configure() returned != 0 status=0x%04x", (unsigned int) sc);
        }
      sl_bt_sm_delete_bondings();
      break;

    case sl_bt_evt_scanner_scan_report_id:
         //  LOG_INFO("\r\n sl_bt_evt_scanner_scan_report_id");

      //Also check addr type //Do memcmp
      // evt->data.evt_scanner_scan_report.address_type == sl_bt_gatt_public_addr
      if((memcmp(&evt->data.evt_scanner_scan_report.address, &serverAddress, sizeof(serverAddress)) == 0 )
          && evt->data.evt_scanner_scan_report.address_type == sl_bt_gap_public_address &&
          evt->data.evt_scanner_scan_report.packet_type == 0)
        {
          sc = sl_bt_scanner_stop();
          if (sc != SL_STATUS_OK)
            LOG_ERROR("sl_bt_scanner_stop() returned != 0 status=0x%04x", (unsigned int) sc);

          sc = sl_bt_connection_open(evt->data.evt_scanner_scan_report.address,
                                     sl_bt_gap_public_address,
                                     sl_bt_gap_1m_phy,
                                     0);
          if (sc != SL_STATUS_OK)
            LOG_ERROR("sl_bt_connection_open() returned != 0 status=0x%04x", (unsigned int) sc);
        }

      break;

    case sl_bt_evt_connection_opened_id:
     // LOG_INFO("\r\n sl_bt_evt_connection_opened_id");

      // handle open event
      ble_data.connection_open = true;
      ble_data.connection_handle = evt->data.evt_connection_opened.connection;
      ble_data.procedure_complete_flag = false; // DOS

      displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");
      displayPrintf(DISPLAY_ROW_BTADDR2, "%x:%x:%x:%x:%x:%x",serverAddress.addr[0],serverAddress.addr[1],serverAddress.addr[2],serverAddress.addr[3],serverAddress.addr[4],serverAddress.addr[5] );

      break;

    case sl_bt_evt_connection_closed_id:
       //LOG_INFO("\r\n sl_bt_evt_connection_closed_id");
      // handle close event
      ble_data.connection_open = false;
      ble_data.procedure_complete_flag = false; // DOS

      ble_data.ok_to_send_htm_indications = false;
      displayPrintf(DISPLAY_ROW_CONNECTION, "Discovering");
      displayPrintf(DISPLAY_ROW_TEMPVALUE, " "," " );
      displayPrintf(DISPLAY_ROW_BTADDR2,"");
      displayPrintf(DISPLAY_ROW_ASSIGNMENT,"A9");
      displayPrintf(DISPLAY_ROW_9," ");
      sl_bt_sm_delete_bondings();
      memset(&ble_data, 0, sizeof(ble_data_struct_t));

      sc = sl_bt_scanner_start(sl_bt_gap_1m_phy, sl_bt_scanner_discover_generic);
      if (sc != SL_STATUS_OK)
        {
          LOG_ERROR("sl_bt_scanner_start() returned != 0 status=0x%04x", (unsigned int) sc);
        }

      break;

    case sl_bt_evt_gatt_service_id:
     // LOG_INFO("sl_bt_evt_gatt_service_id, handle=%d", evt->data.evt_gatt_service.service);
      //ble_data.connection_handle = evt->data.evt_gatt_service.connection;

      if (memcmp(&evt->data.evt_gatt_service.uuid.data[0],
                 &SERVICE_UUID[0],
                 evt->data.evt_gatt_service.uuid.len) == 0)
      {
      ble_data.client_service_handle_htm = evt->data.evt_gatt_service.service;
     // LOG_INFO(">>> Found HTM Service");
      }

      // Button service?, save service handle
      if (memcmp(&evt->data.evt_gatt_service.uuid.data[0],
                 &BUTTON_SERVICE_UUID[0],
                 evt->data.evt_gatt_service.uuid.len) == 0)
      {
      ble_data.client_service_handle_button = evt->data.evt_gatt_service.service;
    // LOG_INFO(">>> Found Button Service");
      }
      break;


    case sl_bt_evt_gatt_characteristic_id:
     // LOG_INFO("sl_bt_evt_gatt_characteristic_id, handle=%d", evt->data.evt_gatt_characteristic.characteristic);
      //ble_data.connection_handle = evt->data.evt_gatt_characteristic.connection;

      // DOS:


      if (memcmp(&evt->data.evt_gatt_characteristic.uuid.data[0],
                 &CHARACTERISTIC_UUID[0],
                 evt->data.evt_gatt_characteristic.uuid.len) == 0)
     {
      ble_data.characteristic_htm = evt->data.evt_gatt_characteristic.characteristic;
      //LOG_INFO(">>> Found HTM Char.");
     }
      // Button characteristic?, save char handle
      if (memcmp(&evt->data.evt_gatt_characteristic.uuid.data[0],
                 &BUTTON_CHARACTERISTIC_UUID[0],
                 evt->data.evt_gatt_characteristic.uuid.len) == 0)
      {
      ble_data.characteristic_button = evt->data.evt_gatt_characteristic.characteristic;
      //LOG_INFO(">>> Found Button Char.");
  }

      break;

    case sl_bt_evt_gatt_procedure_completed_id:
      //LOG_INFO("  *** sl_bt_evt_gatt_procedure_completed_id, result=%x", evt->data.evt_gatt_procedure_completed.result);

      if (evt->data.evt_gatt_procedure_completed.result == 0x110F)
        {
          //ble_data.procedure_completed = false;
          LOG_ERROR("sl_bt_gatt_procedure_completed() returned != 0 status=0x%04x", (unsigned int) evt->data.evt_gatt_procedure_completed.result);
          sc = sl_bt_sm_increase_security(ble_data.connection_handle);
        }

      if(evt->data.evt_gatt_procedure_completed.result != 0)
        {
          ble_data.procedure_complete_flag = false;
          LOG_ERROR("evt_gatt_procedure_completed() returned != 0 status=0x%04x", (unsigned int) sc);
        }
      else
        ble_data.procedure_complete_flag = true;

      break;

    case sl_bt_evt_gatt_characteristic_value_id:
     // LOG_INFO("\r\n sl_bt_evt_gatt_characteristic_value_id");
      if( (evt->data.evt_gatt_characteristic_value.characteristic == ble_data.characteristic_htm) &&
          (evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_handle_value_indication))
        {
          //LOG_INFO("\r\n htm Indication");
          ble_data.Temp_Value = FLOAT_TO_INT32(evt->data.evt_gatt_characteristic_value.value.data);

          sc = sl_bt_gatt_send_characteristic_confirmation(evt->data.evt_gatt_characteristic_value.connection);
          if (sc != SL_STATUS_OK)
            {
              LOG_ERROR("sl_bt_gatt_send_characteristic_confirmation() returned != 0 status=0x%04x", (unsigned int) sc);
            }
          displayPrintf(DISPLAY_ROW_TEMPVALUE, "Temp= %d C", ble_data.Temp_Value);
        }

      if ((evt->data.evt_gatt_characteristic_value.characteristic == ble_data.characteristic_button) &&    evt-> data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_handle_value_indication)
        {
          //LOG_INFO("\r\n button Indication");
          if(evt->data.evt_gatt_characteristic_value.value.data[1]==1)
            {
              displayPrintf(DISPLAY_ROW_9, "Button Pressed");
            }

          else if(evt->data.evt_gatt_characteristic_value.value.data[1]==0)
            {
              displayPrintf(DISPLAY_ROW_9, "Button Released");
            }

          sc = sl_bt_gatt_send_characteristic_confirmation(evt->data.evt_gatt_characteristic_value.connection);

          if (sc != SL_STATUS_OK)
            {
              LOG_ERROR("sl_bt_gatt_send_characteristic_confirmation() returned != 0 status=0x%04x", (unsigned int) sc);
            }
        }

      if ((evt->data.evt_gatt_characteristic_value.characteristic == ble_data.characteristic_button) &&   evt-> data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_read_response)
        {
         // LOG_INFO("\r\nfor button read");
          if(evt->data.evt_gatt_characteristic_value.value.data[0]==1)
            {
              displayPrintf(DISPLAY_ROW_9, "Button Pressed");
            }

          else if(evt->data.evt_gatt_characteristic_value.value.data[0]==0)
            {
              displayPrintf(DISPLAY_ROW_9, "Button Released");
            }

//          sc = sl_bt_gatt_send_characteristic_confirmation(evt->data.evt_gatt_characteristic_value.connection);
//
//          if (sc != SL_STATUS_OK)
//            {
//              LOG_ERROR("sl_bt_gatt_send_characteristic_confirmation() returned != 0 status=0x%04x", (unsigned int) sc);
//            }
        }

      break;

    case sl_bt_evt_system_external_signal_id:
      /*
       // If PB1 is pressed by itself (no PB0 pressed), send gatt_read(button_state)
       *
      // If PB1 is pressed AND PB0 is pressed, then toggle the state of indication enabled
       for button_state in the Server.

      // If PB0 is pressed AND connection_open AND we have a passkey AND we are not bonded)
       call sl_bt_sm_passkey_confirm(connection_handle, 1) (edited)

       */
      //LOG_INFO("\r\n sl_bt_evt_system_external_signal_id");
      if((ble_data.connection_open) &&
         (evt->data.evt_system_external_signal.extsignals & evtPB0Push) &&
         (ble_data.passkey) &&
         (!ble_data.bonded))
      {
       // LOG_INFO("\r\n sl_bt_sm_passkey_confirm");
        sl_bt_sm_passkey_confirm(ble_data.connection_handle, 1);
      }

      else if((ble_data.connection_open) &&
          //(evt->data.evt_system_external_signal.extsignals & evtPB0Push) &&
          (GPIO_PinInGet(gpioPortF, PB0_PIN) == 0) &&
          (evt->data.evt_system_external_signal.extsignals & evtPB1Push))
          {
        //  LOG_INFO("\r\n toggle indications");
          ble_data.button_indication = !ble_data.button_indication;
          if(ble_data.button_indication)
          {
            sc = sl_bt_gatt_set_characteristic_notification(ble_data.connection_handle,
                                                              ble_data.characteristic_button,
                                                              sl_bt_gatt_indication); // enable indications
            if (sc != SL_STATUS_OK)
           {
             LOG_ERROR("sl_bt_gatt_set_characteristic_notification() diaable returned != 0 status=0x%04x", (unsigned int) sc);
           }
          }
          else
          {
            sc = sl_bt_gatt_set_characteristic_notification(ble_data.connection_handle,
                                                              ble_data.characteristic_button,
                                                              sl_bt_gatt_disable); // disable indications
           // sc = sl_bt_gatt_set_characteristic_notification(ble_data.button_indication,gattdb_button_state,sl_bt_gatt_indication);
            if (sc != SL_STATUS_OK)
           {
             LOG_ERROR("sl_bt_gatt_set_characteristic_notification() enable returned != 0 status=0x%04x", (unsigned int) sc);
           }
          }

        }

      else if((ble_data.connection_open) &&
          (!(evt->data.evt_system_external_signal.extsignals & evtPB0Push)) &&
          //(GPIO_PinInGet(gpioPortF, PB0_PIN) == 0) &&
          (evt->data.evt_system_external_signal.extsignals & evtPB1Push))
        {
         // LOG_INFO("\r\n sl_bt_gatt_read_characteristic_value");
          sc = sl_bt_gatt_read_characteristic_value(ble_data.connection_handle, ble_data.characteristic_button);
          if (sc != SL_STATUS_OK)
            {
              LOG_ERROR("sl_bt_gatt_read_characteristic_value() returned != 0 status=0x%04x", (unsigned int) sc);
            }

        }
      break;


    case sl_bt_evt_sm_bonded_id:
    //  LOG_INFO("\r\n sl_bt_evt_sm_bonded_id");
      ble_data.connection_handle = evt->data.evt_sm_bonded.connection;
      ble_data.bonding_handle = evt->data.evt_sm_bonded.bonding;
      ble_data.security_mode = evt->data.evt_sm_bonded.security_mode;

      displayPrintf(DISPLAY_ROW_CONNECTION, "Bonded");
      displayPrintf(DISPLAY_ROW_PASSKEY, " " );
      displayPrintf(DISPLAY_ROW_ACTION, " " );
      ble_data.passkey = false;
      ble_data.bonded = true;
      break;
    case sl_bt_evt_sm_bonding_failed_id:
     // LOG_INFO("\r\n sl_bt_evt_sm_bonding_failed_id");
      ble_data.connection_handle = evt->data.evt_sm_bonding_failed.connection;
      ble_data.reason = evt->data.evt_sm_bonding_failed.reason;
      displayPrintf(DISPLAY_ROW_CONNECTION, "BONDING FAILED");
      ble_data.passkey = false;
      ble_data.bonded = false;
      break;


    case sl_bt_evt_system_soft_timer_id:
      //LOG_INFO("\r\n sl_bt_evt_system_soft_timer_id");
      switch (evt->data.evt_system_soft_timer.handle)
      {

        case 0: // handle from lcd.c is 0
          displayUpdate();
          break;

        case SECOND_TIMER_HANDLE:
          handleSystemSsofttimer(&ble_data);
          break;

       } // switch
      break;

    case sl_bt_evt_connection_parameters_id:
      ble_data.connection_handle = evt->data.evt_connection_parameters.connection;
      ble_data.security_mode = evt->data.evt_connection_parameters.security_mode;
      break;

    case sl_bt_evt_sm_confirm_passkey_id:
  //    LOG_INFO("\r\n sl_bt_evt_sm_confirm_passkey_id");
      ble_data.connection_handle = evt->data.evt_sm_confirm_passkey.connection;
      //ble_data.passkey = evt->data.evt_sm_confirm_passkey.passkey;
      ble_data.passkey = true;
      displayPrintf(DISPLAY_ROW_PASSKEY, "Passkey:%4lu", evt->data.evt_sm_confirm_passkey.passkey);
      displayPrintf(DISPLAY_ROW_ACTION, "Confirm with PB0" );

      break;

    default:
      break;

  } // end - switch

#endif
#endif
}

void TemperatuetoFloat(uint32_t temperature_in_c)
{
  uint8_t htm_temperature_buffer[5];
  uint8_t *p = htm_temperature_buffer;
  UINT8_TO_BITSTREAM(p,flag);
  uint32_t htm_temperature_flt;
  htm_temperature_flt = UINT32_TO_FLOAT(temperature_in_c*1000, -3);

  // Convert temperature to bitstream and place it in the htm_temperature_buffer
  UINT32_TO_BITSTREAM(p, htm_temperature_flt);
  // -------------------------------
  // Write our local GATT DB
  // -------------------------------
  sc = sl_bt_gatt_server_write_attribute_value
      (
          gattdb_temperature_measurement, // handle from gatt_db.h
          0, // offset
          4, // length
          (const uint8_t*)&temperature_in_c // pointer to buffer where data is
      );
  if (sc != SL_STATUS_OK)
    {
      LOG_ERROR("sl_bt_gatt_server_write_attribute_value() returned != 0 status=0x%04x", (unsigned int) sc);
    }

  if ((ble_data.ok_to_send_htm_indications) && (ble_data.connection_open))
    {
      if(ble_data.indication_in_flight == false)
        {

          sc = sl_bt_gatt_server_send_indication
              (
                  ble_data.connection_handle,
                  gattdb_temperature_measurement, // handle from gatt_db.h
                  5,
                  &htm_temperature_buffer[0]    // in IEEE-11073 format
              );
          if (sc != SL_STATUS_OK)
            {
              LOG_ERROR("sl_bt_gatt_server_send_indication() returned != 0 status=0x%04x", (unsigned int) sc);
            }
          else
            {
              //Set indication_in_flight flag
              ble_data.indication_in_flight = true;
              displayPrintf(DISPLAY_ROW_TEMPVALUE, "Temp= %d C", temperature_in_c);
              //LOG_INFO("HTM Ind sent"); // DOS
            }
        }
      else
        {
          // DOS            bool writeSuccess = write_queue(gattdb_temperature_measurement, sizeof(htm_temperature_buffer), htm_temperature_buffer);
          bool writeSuccess = write_queue(gattdb_temperature_measurement, sizeof(htm_temperature_buffer), &htm_temperature_buffer[0]);
          if (writeSuccess != false)
            {
              LOG_ERROR("write_queue() returned != 0 status=0x%04x", (unsigned int) sc);
            }
          else
            {
             // LOG_INFO("HTM Ind queued"); // DOS
            }
        }
    }
}


// -----------------------------------------------
// Private function, original from Dan Walkes. I fixed a sign extension bug.
// We'll need this for Client A9 assignment to convert health thermometer
// indications back to an integer. Convert IEEE-11073 32-bit float to signed integer.
// -----------------------------------------------

int32_t FLOAT_TO_INT32(const uint8_t *value_start_little_endian)
{
  uint8_t mantissaSignByte = 0; // these bits will become the upper 8-bits of the mantissa
  int32_t mantissa; // this holds the 24-bit mantissa value with the upper 8-bits as sign bits
  // input data format is:
  // [0] = flags byte
  // [3][2][1] = mantissa (2's complement)
  // [4] = exponent (2's complement)
  // BT value_start_little_endian[0] has the flags byte
  int8_t exponent = (int8_t)value_start_little_endian[4]; // the exponent is a signed 2’s comp value
  // sign extend the mantissa value if the mantissa is negative
  if (value_start_little_endian[3] & 0x80) { // msb of [3] is the sign of the mantissa
      mantissaSignByte = 0xFF;
  }
  // assemble the mantissa
  mantissa = (int32_t) (value_start_little_endian[1] << 0) |
      (value_start_little_endian[2] << 8) |
      (value_start_little_endian[3] << 16) |
      (mantissaSignByte << 24) ;
  // value = 10^exponent * mantissa; pow() returns a double type
  return (int32_t) (pow( (double) 10, (double) exponent) * (double) mantissa);
} // FLOAT_TO_INT32

void handleSystemSsofttimer( ble_data_struct_t *BleData)
{
  //  if(evt->data.evt_system_soft_timer.handle == 1)
  //    {
  if((BleData->indication_in_flight == false) && (get_queue_depth()>0))
    {
      // read indication

      // DOS           bool readSuccess = read_queue(&charHandle, &bufferLength, buffer);
      bool readSuccess = read_queue(&charHandle, &bufferLength, &buffer[0]);
      if (readSuccess)
        {
          LOG_ERROR("\r\nError reading circular buffer");
        }
      else
        {
          //send indication and flag inflight
          // DOS               sc = sl_bt_gatt_server_send_indication(ble_data.connection_handle, charHandle, sizeof(buffer), &buffer[0]);
          sc = sl_bt_gatt_server_send_indication(ble_data.connection_handle, charHandle, bufferLength, &buffer[0]);
          if (sc != SL_STATUS_OK)
            {
              LOG_ERROR("sl_bt_gatt_server_send_indication() returned != 0 status=0x%04x", (unsigned int) sc);
            }
          else
            {
              ble_data.indication_in_flight = true;
              //("Sent deferred Ind, length=%d", (int) bufferLength);
            }
          // Process the data read from the queue using charHandle, bufferLength, and buffer.
        }
    }
  //    }
}
