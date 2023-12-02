/***********************************************************************
 * @file        scheduler.c
 * @brief       This file contains all the schedular routines
 *              required for the project
 *
 * @date        Sep 19, 2023
 * @author      Aditi Nanaware
 * @Student     Aditi Nanaware, aditi.nanaware@colorado.edu
 *
 * @institution University of Colorado Boulder (UCB)
 * @course      ECEN 5823: IoT Embedded Firmware
 * @instructor  David Sluiter
 *
 * @assignment  Assignment 3 - Si7021 and Load Power Management Part 1
 * @due         Sep 22, 2023
 *
 * @resources   Lecture Slides
 ****************************************************************************/

#include "scheduler.h"
#include "irq.h"
#include "i2c.h"
#include "timers.h"
#include "em_common.h"

#define INCLUDE_LOG_DEBUG 1
#include "log.h"

#include "lcd.h"
#include "i2c.h"
#include "ble.h"
#include "gpio.h"

uint8_t SERVICE_UUID_BUTTON[16] = {0x89, 0x62, 0x13, 0x2d, 0x2a, 0x65, 0xec, 0x87, 0x3e, 0x43, 0xc8, 0x38, 0x01, 0x00, 0x00, 0x00};

//global variable to store events
uint32_t myevents = 0;

void schedularPushButtonEvent()
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  uint8_t pin_value= GPIO_PinInGet(PB0_PORT, PB0_PIN);
  if(pin_value)
    sl_bt_external_signal(evtPB0Release);
  else
    sl_bt_external_signal(evtPB0Push);

  CORE_EXIT_CRITICAL();
}

void schedularPushButton1Event()
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  uint8_t pin_value0= GPIO_PinInGet(PB0_PORT, PB0_PIN);
  uint8_t pin_value= GPIO_PinInGet(PB1_PORT, PB1_PIN);
  if(pin_value)
    sl_bt_external_signal(evtPB1Release);
  else
    {
    if(!pin_value0)
        sl_bt_external_signal(evtPB0Push | evtPB1Push );
    else
      sl_bt_external_signal(evtPB1Push);
    }

  CORE_EXIT_CRITICAL();
}


void schedularSetTempEvent()
{

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  // evtgettEMPERATURE is stored in variable myevents
  //myevents |= evtgetTemperature;
  sl_bt_external_signal(evtgetTemperature);
  CORE_EXIT_CRITICAL();
}


void schedularSetCOMP1Event()
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  // evtCOMP1interrupt is stored in variable myevents
  //myevents |= evtCOMP1interrupt;
  sl_bt_external_signal(evtCOMP1interrupt);

  CORE_EXIT_CRITICAL();
}

void schedularSetI2CEvent()
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  // evtI2CTransferDone is stored in variable myevents
  //myevents |= evtI2CTransferDone;
  sl_bt_external_signal(evtI2CTransferDone);
  CORE_EXIT_CRITICAL();
}

//uint32_t getNextEvent()
//{
//  //Variable to store next event
//  uint32_t nxtevt = 0;
//
//  CORE_DECLARE_IRQ_STATE;
//  CORE_ENTER_CRITICAL();
//
//  //Check if the evtgetTemperature is scheduled
//  if(evtgetTemperature & myevents)
//    {
//      //Store the event in the variable
//      nxtevt = myevents;
//      //Clear the bit for evtgetTemperature event
//      myevents &= ~evtgetTemperature;
//    }
//
//  else if(evtCOMP1interrupt & myevents)
//    {
//      //Store the event in the variable
//      nxtevt = myevents;
//      //Clear the bit for evtCOMP1interrupt event
//      myevents &= ~evtCOMP1interrupt;
//    }
//
//  else if(evtI2CTransferDone & myevents)
//    {
//      //Store the event in the variable
//      nxtevt = myevents;
//      //Clear the bit for evtI2CTransferDone event
//      myevents &= ~evtI2CTransferDone;
//    }
//  CORE_EXIT_CRITICAL();
//  //Return the event
//  return nxtevt;
//}
#ifdef DEVICE_IS_BLE_SERVER

#if (DEVICE_IS_BLE_SERVER == 1)
//void Temperature_State_Machine(sl_bt_msg_t *evt)
//{
//  ble_data_struct_t *ble_data = getBleDataPtr();
//
//  I2C_TransferReturn_TypeDef transferStatus;
//  static State nextState = State_WaitforUF;
//  State currentState = nextState;
//
//  //Fixed logging issues with the first "if" condition, Visweshwaran Baskaran helped in the debug
//  if((SL_BT_MSG_ID(evt->header) == sl_bt_evt_system_external_signal_id) && (ble_data->connection_open == true))
//    //&& (ble_data->ok_to_send_htm_indications == true))
//    {
//      switch(currentState)
//      {
//
//        //Initialize the Temperature Sensor and wait for 80 ms
//        case State_WaitforUF:
//          // LOG_INFO("\r\n State_WaitforUF");
//          nextState = State_WaitforUF;
//          if(evt->data.evt_system_external_signal.extsignals)
//            {
//              if(evtgetTemperature)
//                {
//
//                  //TempSensorInit();
//                  timerWaitUs_irq(80000);
//                  nextState = State_I2CWrite;
//
//                }
//            }
//
//          break;
//
//          //Initialize the I2C Request the Temperature Sensor reading
//          //Add power requirement for ble
//        case State_I2CWrite:
//          //  LOG_INFO("\r\n State_I2CWrite");
//          nextState = State_I2CWrite;
//          if(evt->data.evt_system_external_signal.extsignals)
//            {
//              if( evtCOMP1interrupt)
//                {
//
//                  I2CInit();
//                  //askTemperature();
//                  sl_power_manager_add_em_requirement(1);
//                  nextState = State_WaitforI2CComplete;
//                }
//            }
//          break;
//
//          //Check transfer status of last I2C transaction, if done disable the I2C irq
//          //Wait for 10.8 ms and remove the power requirement
//        case State_WaitforI2CComplete:
//          // LOG_INFO("\r\n State_WaitforI2CComplete");
//          nextState = State_WaitforI2CComplete;
//          if(evt->data.evt_system_external_signal.extsignals)
//            {
//              if(evtI2CTransferDone)
//
//                {
//
//                  transferStatus = gettransferStatus();
//
//                  if (transferStatus == i2cTransferDone)
//                    {
//                      NVIC_DisableIRQ(I2C0_IRQn);
//                    }
//                  else
//                    {
//                    //  LOG_ERROR("I2C_Transfer Failed: %d",transferStatus);
//                    }
//                  timerWaitUs_irq(10800);
//                  sl_power_manager_remove_em_requirement(1);
//                  nextState = State_I2CRead;
//                }
//            }
//          break;
//
//          //Read the Temperature data and add the power requirement
//        case State_I2CRead:
//          //  LOG_INFO("\r\n State_I2CRead");
//          nextState = State_I2CRead;
//          if(evt->data.evt_system_external_signal.extsignals)
//            {
//              if(evtCOMP1interrupt)
//                {
//
//                  //readTemperature();
//                  sl_power_manager_add_em_requirement(1);
//                  nextState = State_ReaDTeMP;
//
//                }
//            }
//          break;
//
//          //Check transfer status of last I2C transaction, if done disable the I2C irq
//          //Print the Temperature data
//        case State_ReaDTeMP:
//          //  LOG_INFO("\r\n State_ReaDTeMP");
//          nextState = State_ReaDTeMP;
//
//          if(evt->data.evt_system_external_signal.extsignals)
//            {
//
//              if( evtI2CTransferDone)
//                {
//
//                  transferStatus = gettransferStatus();
//
//                  if (transferStatus == i2cTransferDone)
//                    {
//                      NVIC_DisableIRQ(I2C0_IRQn);
//                    }
//                  else
//                    {
//                      LOG_ERROR("I2C_Transfer Failed: %d",transferStatus);
//                    }
//
//                  int32_t temp_in_c = printTemperature();
//                  TemperatuetoFloat(temp_in_c);
//                  // Make sure to prevent turning off GPIO from state machine
//                  // to keep the LCD going
//                  // TempSensorDeinit();
//                  sl_power_manager_remove_em_requirement(1);
//                  nextState = State_WaitforUF;
//                }
//            }
//          break;
//
//          //Log the error if event sensed is not registered
//        default:
//          break;
//      }
//    }
//}

#else

void Discovery_State_Machine(sl_bt_msg_t *evt)
{
  ble_data_struct_t *ble_data = getBleDataPtr();
  sl_status_t sc;



  static Discovery_STATE nextState = Open;


  switch(nextState)
  {
    case Open:
      //          LOG_INFO("\r\n Open");
      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_connection_opened_id)
        {
          // LOG_INFO("To open");
          uint8_t SERVICE_UUID[2] = {0x09,0x18};
          sc = sl_bt_gatt_discover_primary_services_by_uuid(ble_data->connection_handle, sizeof(SERVICE_UUID), &SERVICE_UUID[0]); // DOS: always good to use & force the compiler to calculate the address
          if (sc != SL_STATUS_OK)
            LOG_ERROR("\n\rsl_bt_gatt_discover_primary_services_by_uuid() for htm returned != 0 status=0x%04x", (unsigned int) sc);

          nextState = Services_Discovered;
        }
      break;

    case Services_Discovered:
      //          LOG_INFO("\r\n 2... Services_Discovered");
      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)
        {
          //LOG_INFO("To Services_Discovered htm");
          uint8_t CHARACTERISTIC_UUID[2] = {0x1c,0x2a};
          sc = sl_bt_gatt_discover_characteristics_by_uuid(ble_data->connection_handle, ble_data->client_service_handle_htm, sizeof(CHARACTERISTIC_UUID), &CHARACTERISTIC_UUID[0]); // DOS: always good to use & force the compiler to calculate the address
          if (sc != SL_STATUS_OK)
            LOG_ERROR("sl_bt_gatt_discover_characteristics_by_uuid() for htm returned != 0 status=0x%04x", (unsigned int) sc);


          nextState = Characteristics_Discovered;
        }
      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_connection_closed_id)
        {
          //  LOG_INFO("To0");
          nextState = Open;
          displayPrintf(DISPLAY_ROW_BTADDR2, " ");
        }
      break;

    case Characteristics_Discovered:
      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)
        {
           // LOG_INFO("To Characteristics_Discovered htm");
          //              LOG_INFO("\r\n Characteristics_Discovered");
          sc = sl_bt_gatt_set_characteristic_notification(ble_data->connection_handle, ble_data->characteristic_htm, sl_bt_gatt_indication);
          if (sc != SL_STATUS_OK)
            LOG_ERROR("sl_bt_gatt_characteristic_notification() returned != 0 status=0x%04x", (unsigned int) sc);
          nextState = Enable_Notification;
          // }
        }

      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_connection_closed_id)
        {
          //  LOG_INFO("To0");
          nextState = Open;
          displayPrintf(DISPLAY_ROW_BTADDR2, " ");
        }
      break;

    case Enable_Notification:
      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)
        {
          //  LOG_INFO("To Enable_Notification htm");
          //              LOG_INFO("\r\n Enable_Notification");
          //ble_data->ok_to_send_htm_indications = true;
          displayPrintf(DISPLAY_ROW_CONNECTION, "Handling Indications");
          nextState = Discover_Service_for_Button;
          // }
        }

      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_connection_closed_id)
        {
          //  LOG_INFO("To0");
          nextState = Open;
          displayPrintf(DISPLAY_ROW_BTADDR2, " ");
        }
      break;

    case Discover_Service_for_Button:
//      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)
//        {
        //  LOG_INFO("To Discover_Service_for_Button");
          //00000001-38c8-433e-87ec-652a2d136289

          sc = sl_bt_gatt_discover_primary_services_by_uuid(ble_data->connection_handle, sizeof(SERVICE_UUID_BUTTON), &SERVICE_UUID_BUTTON[0]); // DOS: always good to use & force the compiler to calculate the address
          if (sc != SL_STATUS_OK)
            LOG_ERROR("\n\rsl_bt_gatt_discover_primary_services_by_uuid() for button returned != 0 status=0x%04x", (unsigned int) sc);
          nextState = Discover_Char_For_Button;

          if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_connection_closed_id)
            {
              //  LOG_INFO("To0");
              nextState = Open;
              displayPrintf(DISPLAY_ROW_BTADDR2, " ");
            }
//        }
      break;

    case Discover_Char_For_Button:
      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)
        {
         // LOG_INFO("To Discover_Char_For_Button");
          //0000002-38c8-433e-87ec-652a2d136289
          uint8_t BUTTON_CHARACTERISTIC_UUID[16] = {0x89, 0x62, 0x13, 0x2d, 0x2a, 0x65, 0xec, 0x87, 0x3e, 0x43, 0xc8, 0x38, 0x02, 0x00, 0x00, 0x00};
          sc = sl_bt_gatt_discover_characteristics_by_uuid(ble_data->connection_handle, ble_data->client_service_handle_button, sizeof(BUTTON_CHARACTERISTIC_UUID), &BUTTON_CHARACTERISTIC_UUID[0]); // DOS: always good to use & force the compiler to calculate the address
          if (sc != SL_STATUS_OK)
            LOG_ERROR("sl_bt_gatt_discover_characteristics_by_uuid() for button returned != 0 status=0x%04x", (unsigned int) sc);

          nextState = SET_Notification_For_Button;

          if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_connection_closed_id)
            {
              //  LOG_INFO("To0");
              nextState = Open;
              displayPrintf(DISPLAY_ROW_BTADDR2, " ");
            }
        }
      break;

    case SET_Notification_For_Button:
      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)
      {
         // LOG_INFO("To SET_Notification_For_Button");
      sc = sl_bt_gatt_set_characteristic_notification(ble_data->connection_handle, ble_data->characteristic_button, sl_bt_gatt_notification);
      if (sc != SL_STATUS_OK)
        LOG_ERROR("sl_bt_gatt_characteristic_notification() returned != 0 status=0x%04x", (unsigned int) sc);
      nextState = EN_Notification_For_Button;
      }
      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_connection_closed_id)
        {
          //  LOG_INFO("To0");
          nextState = Open;
          displayPrintf(DISPLAY_ROW_BTADDR2, " ");
        }
      break;

    case EN_Notification_For_Button:
      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_gatt_procedure_completed_id)
        {
        //  LOG_INFO("To EN_Notification_For_Button");
          //  LOG_INFO("To4");
                        LOG_INFO("\r\n Enable_Notification");
          ble_data->button_notification = true;
          displayPrintf(DISPLAY_ROW_CONNECTION, "Handling Indications");
          nextState = Idle;
          // }
        }

      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_connection_closed_id)
        {
          //  LOG_INFO("To0");
          nextState = Open;
          displayPrintf(DISPLAY_ROW_BTADDR2, " ");
        }
      break;
    case Idle:
      if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_connection_closed_id)
        {
          //LOG_INFO("To Idle");
          //  LOG_INFO("To0");
          nextState = Open;
          displayPrintf(DISPLAY_ROW_BTADDR2, " ");
        }
      break;
  }


}
#endif
#endif
