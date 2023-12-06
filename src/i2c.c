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


//Variabel to store the Gesture/Read data
uint16_t read_data[2];

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



//void TempSensorInit()
//{
//  GPIO_PinOutSet(Sensor_Enable_Port,Sensor_Enable);
//}
//
//void TempSensorDeinit()
//{
//  GPIO_PinOutClear(Sensor_Enable_Port,Sensor_Enable);
//}

 /**
 * @brief Writes a single byte to the I2C device and specified register
 *
 * @param[in] reg the register in the I2C device to write to
 * @param[in] val the 1-byte value to write to the I2C device
 * @return True if successful write operation. False otherwise.
 *bool SparkFun_APDS9960::wireWriteDataByte(uint8_t reg, uint8_t val)
 */

uint32_t write_read(uint8_t reg, uint8_t *Data)
{

  //Variable to store the command data
  uint8_t cmd_data[1];

  //Instance to check the status of I2C cycle
  I2C_TransferReturn_TypeDef transferStatus;

  //Select the reg to be read from
  cmd_data[0] = reg;

  //Initialize data structure for I2C write read cycle
  transferSequence.addr = APDS9960_DEVICE_ADDR << 1;
  transferSequence.flags =  I2C_FLAG_WRITE_READ;
  transferSequence.buf[0].data = cmd_data;
  transferSequence.buf[0].len = 1;
  transferSequence.buf[1].data = Data;
  transferSequence.buf[1].len = 1;

  // config NVIC to generate an IRQ for the I2C0 module.
  // ==>> disable this NVIC interrupt when the I2C transfer has completed
  NVIC_EnableIRQ(I2C0_IRQn);

  //Check the status of I2C write cycle to request the temperature data from
  // SI7021 sensor
  transferStatus = I2C_TransferInit(I2C0, &transferSequence);

  if (transferStatus < i2cTransferDone)
    {
      LOG_ERROR("I2CSPM_Transfer: I2C bus write of cmd=F3 failed");
      return (uint32_t)transferStatus;
    }
  //Successfull
  return (uint32_t)1;
}

/**
 * @brief Writes a block (array) of bytes to the I2C device and register
 *
 * @param[in] reg the register in the I2C device to write to
 * @param[in] val pointer to the beginning of the data byte array
 * @param[in] len the length (in bytes) of the data to write
 * @return True if successful write operation. False otherwise.
 bool SparkFun_APDS9960::wireWriteDataBlock(  uint8_t reg,
                                        uint8_t *val,
                                        unsigned int len)


 */

uint32_t write_write(uint8_t reg, uint8_t *Data)
{

  //Variable to store the command data
  uint8_t cmd_data[2];
  uint8_t data[1];

  //Instance to check the status of I2C cycle
  I2C_TransferReturn_TypeDef transferStatus;

  cmd_data[0] = reg;
  cmd_data[1] = Data;

  //Initialize data structure for I2C write read cycle
  transferSequence.addr = APDS9960_DEVICE_ADDR << 1;
  transferSequence.flags =  I2C_FLAG_WRITE;
  transferSequence.buf[0].data = cmd_data;
  transferSequence.buf[0].len = 2;
  transferSequence.buf[1].data = data;
  transferSequence.buf[1].len = 0;

  // config NVIC to generate an IRQ for the I2C0 module.
  // ==>> disable this NVIC interrupt when the I2C transfer has completed
  NVIC_EnableIRQ(I2C0_IRQn);

  //Check the status of I2C write cycle to request the temperature data from
  // SI7021 sensor
  transferStatus = I2C_TransferInit(I2C0, &transferSequence);

  if (transferStatus < i2cTransferDone)
    {
      LOG_ERROR("I2CSPM_Transfer: I2C bus write of cmd=F3 failed");
      return (uint32_t)transferStatus;
    }
  //Successfull
  return (uint32_t)1;
}


//uint32_t read_read(uint8_t reg, uint8_t *Data, uint8_t len)
//{
//
//  //Variable to store the command data
//  uint8_t cmd_data[1];
//
//  //Instance to check the status of I2C cycle
//  I2C_TransferReturn_TypeDef transferStatus;
//
//  //Initialize data structure for I2C write read cycle
//  transferSequence.addr = APDS9960_DEVICE_ADDR << 1;
//  transferSequence.flags =  I2C_FLAG_WRITE_READ;
//
//  //Select the reg to read from
//  transferSequence.buf[0].data = cmd_data;
//  transferSequence.buf[0].len = 1;
//  //select len of the data to be read
//  transferSequence.buf[1].data = Data;
//  transferSequence.buf[1].len = len;
//
//  // config NVIC to generate an IRQ for the I2C0 module.
//  // ==>> disable this NVIC interrupt when the I2C transfer has completed
//  NVIC_EnableIRQ(I2C0_IRQn);
//
//  //Check the status of I2C write cycle to request the temperature data from
//  // SI7021 sensor
//  transferStatus = I2C_TransferInit(I2C0, &transferSequence);
//
//  if (transferStatus < i2cTransferDone)
//    {
//      LOG_ERROR("I2CSPM_Transfer: I2C bus write of cmd=F3 failed");
//      return (uint32_t)transferStatus;
//    }
//  //Successfull
//  return (uint32_t)1;
//}


