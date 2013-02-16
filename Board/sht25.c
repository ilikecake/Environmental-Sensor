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
*	\brief		SHT25 humidity sensor driver.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		2/7/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#include "main.h"

void SHT25_Init( void )
{
	return;
}

uint8_t SHT25_ReadUserReg(uint8_t *RegValue)
{
	uint8_t DataToSend = SHT25_READ_USER_REG;

	if(I2CSoft_RW(SHT25_I2C_ADDR, &DataToSend, RegValue, 1, 1) == 0)
	{	
		return 0x00;
	}
	return 0xFF;
}

uint8_t SHT25_WriteUserReg(uint8_t RegValue)
{
	uint8_t DataToSend[2];
	uint8_t CurrentUserReg;

	//Get current user reg value
	if(SHT25_ReadUserReg(&CurrentUserReg) != 0x00)
	{
		return 0xFF;
	}

	//Mask the reserved bits and write back the new values
	CurrentUserReg &= SHT25_UREG_RESERVED_MASK;
	RegValue &= ~(SHT25_UREG_RESERVED_MASK);

	DataToSend[0] = SHT25_READ_USER_REG;
	DataToSend[1] = (RegValue | CurrentUserReg);

	if(I2CSoft_RW(SHT25_I2C_ADDR, DataToSend, NULL, 2, 0) == 0)
	{	
		return 0x00;
	}
	return 0xFF;
}




/** @} */