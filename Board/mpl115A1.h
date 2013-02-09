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
*	\brief		Header file for the MPL115A1 digital barometer.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		2/8/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#ifndef _MPL115A1_H_
#define _MPL115A1_H_

#include "stdint.h"

//Register definitions (TODO: left shift these and append the proper read/write bit (0x80 for read))
#define MPL115AL_REG_PRESSURE_MSB		0x00
#define MPL115AL_REG_PRESSURE_LSB		0x01
#define MPL115AL_REG_TEMP_MSB			0x02
#define MPL115AL_REG_TEMP_LSB			0x03
#define MPL115AL_REG_CAL_A0_MSB			0x04
#define MPL115AL_REG_CAL_A0_LSB			0x05
#define MPL115AL_REG_CAL_B1_MSB			0x06
#define MPL115AL_REG_CAL_B1_LSB			0x07
#define MPL115AL_REG_CAL_B2_MSB			0x08
#define MPL115AL_REG_CAL_B2_LSB			0x09
#define MPL115AL_REG_CAL_C12_MSB		0x0A
#define MPL115AL_REG_CAL_C12_LSB		0x0B
#define MPL115AL_REG_CONVERT			0x12

void MPL115A1_Init(void);
void MPL115A1_Select(void);
void MPL115A1_Deselect(void);
void MPL115A1_Sleep(uint8_t ToSleep);

void MPL115A1_GetCalData(uint16_t *A0, uint16_t *B1, uint16_t *B2, uint16_t *C12);
void MPL115A1_GetConversion(uint16_t *PressureData, uint16_t *TemperatureData);
void MPL115A1_GetPressure(uint16_t *Pressure_kPa);

#endif
/** @} */
