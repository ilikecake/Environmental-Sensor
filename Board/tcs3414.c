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
*	\brief		TCS3414 hardware driver.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		2/3/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#include "main.h"

void tcs3414_Init( void )
{
	tcs3414_WriteReg(0, 1);


	return;
}

uint8_t tcs3414_WriteReg(uint8_t RegToWrite, uint8_t RegData)
{
	uint8_t DataToSend[2];
	uint8_t DataToReceive = 0;

	if(tcs3414_IsReg(RegToWrite) == 1)
	{
		DataToSend[0] = RegToWrite | TCS3414_COMMAND_SELECT;
		DataToSend[1] = RegData;
		if(I2CSoft_RW(TCS3414_I2C_ADDR, DataToSend, &DataToReceive, 2, 0) == 0)
		{
			return 0x00;
		}
	}
	return 0xFF;
}

uint8_t tcs3414_ReadReg(uint8_t RegToRead, uint8_t *RegData)
{
	uint8_t DataToSend = 0;

	if(tcs3414_IsReg(RegToRead) == 1)
	{
		DataToSend = RegToRead | TCS3414_COMMAND_SELECT;
		if(I2CSoft_RW(TCS3414_I2C_ADDR, &DataToSend, RegData, 1, 1) == 0)
		{	
			return 0x00;
		}
	}
	return 0xFF;
}

//Returns 0 if the register is invalid, 1 if it is valid
uint8_t tcs3414_IsReg(uint8_t Reg)
{
	if( (Reg == 5) || (Reg == 6) || ((Reg > 0x0B) && (Reg < 0x10)) || (Reg > 0x17) )
	{
		//Illegal register
		return 0;
	}
	return 1;
}