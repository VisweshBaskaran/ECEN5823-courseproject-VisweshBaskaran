/***********************************************************************
 * @file         i2c.h
 * @brief        header/interface file for i2c
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
#ifndef SRC_I2C_H_
#define SRC_I2C_H_

#include <stdint.h>

//#define SI7021_DEVICE_ADDR (0x40)
#define APDS9960_DEVICE_ADDR (0x39)



#define Measure_Temperature (0xF3)

/*
@author         Aditi Nanaware
@function name  I2CInit
@brief          functionality of initializing the hardware for i2c
                is implemented in this function.
@param          none
@return         void
@resources      Lecture Slides
*/
void I2CInit();

/*
@author         Aditi Nanaware
@function name  askTemperature
@brief          functionality of write i2c cycle is implemented in this function.
@param          none
@return         void
@resources      Lecture Slides
*/
uint32_t write_read(uint8_t reg, uint8_t *Data);

/*
@author         Aditi Nanaware
@function name  read_temp_from_si7021
@brief          functionality of initializing the temperature sensor,
                i2c write cycle to request the temperature data, i2c read
                cycle to read the temperature data, de-intializing the
                temperature sensor is implemented in this function.
@param          none
@return         void
@resources      Lecture Slides
*/
uint32_t write_write(uint8_t reg, uint8_t *Data);

/*
@author         Aditi Nanaware
@function name  printTemperature
@brief          functionality of printing the current Temperature is
                implemented in this function
@param          none
@return         int32_t
@resources      Lecture Slides
*/
uint32_t read_read(uint8_t reg, uint8_t *Data, uint8_t len);

#endif /* SRC_I2C_H_ */
