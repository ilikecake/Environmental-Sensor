/*   This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file
*	\brief		Header file for the TCS3414 hardware driver.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		2/3/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#ifndef _TCS3414_H_
#define _TCS3414_H_

#include "stdint.h"
#include "config.h"

#define TCS3414_I2C_ADDR		0x39

//Register map
#define TCS3414_REG_CONTROL					0x00
#define TCS3414_REG_TIMING					0x01
#define TCS3414_REG_INTERRUPT				0x02
#define TCS3414_REG_INT_SOURCE				0x03
#define TCS3414_REG_ID						0x04
#define TCS3414_REG_GAIN					0x07
#define TCS3414_REG_LOW_THRESH_LOW_BYTE		0x08
#define TCS3414_REG_LOW_THRESH_HIGH_BYTE	0x09
#define TCS3414_REG_HIGH_THRESH_LOW_BYTE	0x0A
#define TCS3414_REG_HIGH_THRESH_HIGH_BYTE	0x0B
#define TCS3414_REG_DATA1_LOW				0x10
#define TCS3414_REG_DATA1_HIGH				0x11
#define TCS3414_REG_DATA2_LOW				0x12
#define TCS3414_REG_DATA2_HIGH				0x13
#define TCS3414_REG_DATA3_LOW				0x14
#define TCS3414_REG_DATA3_HIGH				0x15
#define TCS3414_REG_DATA4_LOW				0x16
#define TCS3414_REG_DATA4_HIGH				0x17

//Command Register
#define TCS3414_COMMAND_SELECT				0x80

//Control Register
#define TCS3414_CONTROL_ADC_VALID_MASK		0x10
#define TCS3414_CONTROL_ADC_ENABLE			0x02
#define TCS3414_CONTROL_ADC_DISABLE			0x00
#define TCS3414_CONTROL_POWER_ON			0x01
#define TCS3414_CONTROL_POWER_OFF			0x00


/** Write a register to the TCS3414. Return 0 if successful */
uint8_t tcs3414_WriteReg(uint8_t RegToWrite, uint8_t RegData);

/** Read a register from the TCS3414. Return 0 if successful */
uint8_t tcs3414_ReadReg(uint8_t RegToRead, uint8_t *RegData);

/** Returns 1 if Reg is a valid register */
uint8_t tcs3414_IsReg(uint8_t Reg);

void tcs3414_Init( void );

uint8_t tcs3414_GetData(uint16_t *RedData, uint16_t *GreenData, uint16_t *BlueData, uint16_t *ClearData);

#endif