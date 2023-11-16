/***********************************************************************
 * @file         i2c.c
 * @brief        source file for i2c
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

#include "i2c.h"
#include "gpio.h"
#include "em_i2c.h"
#include "sl_i2cspm.h"
#include "timers.h"
#include "lcd.h"

#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

//Instance for I2C cycle
I2C_TransferSeq_TypeDef transferSequence;

//Variable to store the command data
uint8_t cmd_data = Measure_Temperature;

//Variabel to store the Temperature/Read data
uint16_t read_data;

/*
 Data structure to be passed to I2CInit()
 @function used to set I2C_Config reg bits and other values
 */
I2CSPM_Init_TypeDef I2C_Config =
{
.port = I2C0,
.sclPort = gpioPortC,
.sclPin = 10,
.sdaPort = gpioPortC,
.sdaPin = 11,
.portLocationScl = 14,
.portLocationSda = 16,
.i2cRefFreq = 0,
.i2cMaxFreq = I2C_FREQ_STANDARD_MAX,
.i2cClhr = i2cClockHLRStandard
};

void I2CInit()
{
  I2CSPM_Init(&I2C_Config);
}

void TempSensorInit()
{
  GPIO_PinOutSet(Sensor_Enable_Port,Sensor_Enable);
}

void TempSensorDeinit()
{
  GPIO_PinOutClear(Sensor_Enable_Port,Sensor_Enable);
}

void askTemperature()
{
  //Instance to check the status of I2C cycle
  I2C_TransferReturn_TypeDef transferStatus;

  //Initialize data structure for I2C write cycle
  transferSequence.addr = SI7021_DEVICE_ADDR << 1;
  transferSequence.flags =  I2C_FLAG_WRITE;
  transferSequence.buf[0].data = &cmd_data;
  transferSequence.buf[0].len = sizeof(cmd_data);

  // config NVIC to generate an IRQ for the I2C0 module.
  // ==>> disable this NVIC interrupt when the I2C transfer has completed
  NVIC_EnableIRQ(I2C0_IRQn);

  //Check the status of I2C write cycle to request the temperature data from
  // SI7021 sensor
  transferStatus = I2C_TransferInit(I2C0, &transferSequence);

  if (transferStatus < i2cTransferDone)
    {
      LOG_ERROR("I2CSPM_Transfer: I2C bus write of cmd=F3 failed");
    }
}

void readTemperature()
{
  //Instance to check the status of I2C cycle
  I2C_TransferReturn_TypeDef transferStatus;

  //Initialize data structure for I2C read cycle
  transferSequence.addr = SI7021_DEVICE_ADDR << 1;
  transferSequence.flags =  I2C_FLAG_READ;
  transferSequence.buf[0].data = (uint8_t*)&read_data;
  transferSequence.buf[0].len = sizeof(read_data);

  // config NVIC to generate an IRQ for the I2C0 module.
  // ==>> disable this NVIC interrupt when the I2C transfer has completed
  NVIC_EnableIRQ(I2C0_IRQn);

  //Check the status of I2C read cycle to request the temperature data from
  // SI7021 sensor
  transferStatus = I2C_TransferInit(I2C0, &transferSequence);

  if (transferStatus < i2cTransferDone)
  {
   LOG_ERROR ("I2CSPM_Transfer: I2C bus read of cmd=F3 failed");
  }

  //Logic to swap the bytes to get the raw Temperature Data
  if(transferStatus == i2cTransferDone)
    {
      printTemperature();
    }

}

//  With Polling
//void read_temp_from_si7021()
//{
//  I2CInit();
//  TempSensorInit();
//  //Wait before we could start I2C cycle
//  timerWaitUs(80000);
//  askTemperature();
//  //Wait before we could read the Temperature data
//  timerWaitUs(10800);
//  readTemperature();
//  TempSensorDeinit();
//
//}

int32_t printTemperature()
{
   float temp_value;
   uint16_t firsthalf, secondhalf;

   //Swapping the bytes
   firsthalf = read_data & 0xFF00;
   firsthalf = firsthalf >> 8;

   secondhalf = read_data & 0x00FF;
   secondhalf = secondhalf << 8;

   read_data = firsthalf | secondhalf;

   //Convert Raw Temperature value into degree Celcius
   temp_value = ((read_data*MULTIPLER)/DIVIDE_BY) - OFFSET_CALIBRATION;
   //LOG_INFO(" output = %f\n \r\n",temp_value);
  // TemperatuetoFloat(temp_value);
   //displayPrintf(DISPLAY_ROW_TEMPVALUE, "Temp= %d C", temp_value);
   return (int32_t)temp_value;
}
