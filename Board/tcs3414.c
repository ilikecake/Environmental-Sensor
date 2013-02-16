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
	//Power up the TCS3401 with the default settings.
	// -Free running ADC
	// -Integration time of 12ms
	tcs3414_WriteReg(TCS3414_REG_CONTROL, TCS3414_CONTROL_POWER_ON);
	tcs3414_WriteReg(TCS3414_REG_TIMING, (TCS3414_TIMING_MODE_FREE | TCS3414_TIMING_INT_TIME_100MS));
	tcs3414_WriteReg(TCS3414_REG_CONTROL, (TCS3414_CONTROL_ADC_ENABLE | TCS3414_CONTROL_POWER_ON));

	return;
}

uint8_t tcs3414_WriteReg(uint8_t RegToWrite, uint8_t RegData)
{
	uint8_t DataToSend[2];
	uint8_t DataToReceive = 0;
	uint8_t stat;

	if(tcs3414_IsReg(RegToWrite) == 1)
	{
		DataToSend[0] = RegToWrite | TCS3414_COMMAND_SELECT;
		DataToSend[1] = RegData;
		stat = I2CSoft_RW(TCS3414_I2C_ADDR, DataToSend, &DataToReceive, 2, 0);
		return stat;
	}
	return 0xFF;
}

uint8_t tcs3414_ReadReg(uint8_t RegToRead, uint8_t *RegData)
{
	uint8_t DataToSend = 0;
	uint8_t stat;

	if(tcs3414_IsReg(RegToRead) == 1)
	{
		DataToSend = RegToRead | TCS3414_COMMAND_SELECT;
		stat = I2CSoft_RW(TCS3414_I2C_ADDR, &DataToSend, RegData, 1, 1);
		return stat;
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

uint8_t tcs3414_GetData(uint16_t *RedData, uint16_t *GreenData, uint16_t *BlueData, uint16_t *ClearData)
{
	//uint8_t i;
	uint8_t DataToSend;
	uint8_t DataToReceive[9];
	
	tcs3414_ReadReg(TCS3414_REG_CONTROL, DataToReceive);
	
	if((DataToReceive[0] & TCS3414_CONTROL_ADC_VALID_MASK) != TCS3414_CONTROL_ADC_VALID_MASK)
	{
		//ADC data is not valid
		return 0xFF;
	}
	
	//Note: the first byte received is the number of data bytes (8).
	DataToSend = 0xCF;
	if(I2CSoft_RW(TCS3414_I2C_ADDR, &DataToSend, DataToReceive, 1, 9) == 0)
	{
		*RedData = (DataToReceive[3] | (DataToReceive[4] << 8));
		*GreenData = (DataToReceive[1] | (DataToReceive[2] << 8));
		*BlueData = (DataToReceive[5] | (DataToReceive[6] << 8));
		*ClearData = (DataToReceive[7] | (DataToReceive[8] << 8));
		
		return 0x00;
	}
	return 0xFF;
}

/** @} */
