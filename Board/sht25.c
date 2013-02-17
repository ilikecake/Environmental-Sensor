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
#include "stdio.h"

void SHT25_Init( void )
{
	SHT25_Reset();
	return;
}

uint8_t SHT25_Reset(void)
{
	uint8_t DataToSend;
	uint8_t stat;

	DataToSend = SHT25_RESET;
	stat = I2CSoft_RW(SHT25_I2C_ADDR, &DataToSend, NULL, 1, 0);
	
	//Sensor takes <15ms to reinitalize
	DelayMS(15);
	
	return stat;
}

uint8_t SHT25_ReadUserReg(uint8_t *RegValue)
{
	uint8_t DataToSend = SHT25_READ_USER_REG;
	uint8_t stat;

	stat = I2CSoft_RW(SHT25_I2C_ADDR, &DataToSend, RegValue, 1, 1);
	return stat;
}

uint8_t SHT25_WriteUserReg(uint8_t RegValue)
{
	uint8_t DataToSend[2];
	uint8_t CurrentUserReg;
	uint8_t stat;

	//Get current user reg value
	stat = SHT25_ReadUserReg(&CurrentUserReg);
	if(stat != SOFT_I2C_STAT_OK)
	{
		return (stat | 0x80);
	}

	//Mask the reserved bits and write back the new values
	CurrentUserReg &= SHT25_UREG_RESERVED_MASK;
	RegValue &= ~(SHT25_UREG_RESERVED_MASK);

	DataToSend[0] = SHT25_WRITE_USER_REG;
	DataToSend[1] = (RegValue | CurrentUserReg);

	stat = I2CSoft_RW(SHT25_I2C_ADDR, DataToSend, NULL, 2, 0);
	return stat;
}

//TODO: See if I can get rid of some of these variables.
//TODO: Will this ever be negative?
//TODO: Does the big buffer need to be 32 bits?
uint8_t SHT25_ReadTemp(int16_t *TempValue)
{
	int32_t BigBuffer;
	uint16_t SmallBuffer;
	uint8_t DataToSend;
	uint8_t DataToReceive[3];
	uint8_t stat;
	uint8_t i;

	DataToSend = SHT25_READ_TEMP_NOHOLD;

	//Start temperature conversion
	stat = I2CSoft_RW(SHT25_I2C_ADDR, &DataToSend, NULL, 1, 0);
	
	//Wait for response
	DelayMS(75);
	for(i=0; i<20; i++)
	{
		stat = I2CSoft_RW(SHT25_I2C_ADDR, NULL, DataToReceive, 0, 3);
		if(stat == SOFT_I2C_STAT_OK)
		{
			break;
		}
		DelayMS(10);
	}
	
	if(i >= 20)
	{
		//Device did not respond
		return SHT25_RETURN_STATUS_TIMEOUT;
	}

	SmallBuffer = (DataToReceive[0] << 8) | (DataToReceive[1]);
	
	if(SHT25_VerifyCRC(SmallBuffer, DataToReceive[2]) == 1)
	{
		BigBuffer = (17572l*(int32_t)(SmallBuffer) - 307036160l)/(65536l);
		*TempValue = (int16_t)BigBuffer;
		return SHT25_RETURN_STATUS_OK;
	}
	return SHT25_RETURN_STATUS_CRC_ERROR;
}

//TODO: See if I can get rid of some of these variables.
//TODO: Does the big buffer need to be 32 bits?
uint8_t SHT25_ReadRH(int16_t *RHValue)
{
	uint32_t BigBuffer;
	uint16_t SmallBuffer;
	uint8_t DataToSend;
	uint8_t DataToReceive[3];
	uint8_t stat;
	uint8_t i;

	DataToSend = SHT25_READ_RH_NOHOLD;

	//Start RH conversion
	stat = I2CSoft_RW(SHT25_I2C_ADDR, &DataToSend, NULL, 1, 0);
	
	//Wait for response
	DelayMS(30);
	for(i=0; i<20; i++)
	{
		stat = I2CSoft_RW(SHT25_I2C_ADDR, NULL, DataToReceive, 0, 3);
		if(stat == SOFT_I2C_STAT_OK)
		{
			break;
		}
		DelayMS(10);
	}
	
	if(i >= 20)
	{
		//Device did not respond
		return SHT25_RETURN_STATUS_TIMEOUT;
	}

	SmallBuffer = (DataToReceive[0] << 8) | (DataToReceive[1]);
	
	if(SHT25_VerifyCRC(SmallBuffer, DataToReceive[2]) == 1)
	{
		BigBuffer = ((12500l)*((uint32_t)(SmallBuffer)) - 39321600l)/(65536l);
		
		*RHValue = ((int16_t)BigBuffer);
		return SHT25_RETURN_STATUS_OK;
	}
	return SHT25_RETURN_STATUS_CRC_ERROR;
}

uint8_t SHT25_VerifyCRC(uint16_t DataValue, uint8_t CRCValue)
{
	uint32_t CRCPoly = 0b100110001000000000000000;		//Polynomial is x^8+x^5+x^4+1
	uint32_t buffer;
	uint8_t i;
	
	buffer = ((uint32_t)DataValue << 8) | (CRCValue);
	
	for(i=23; i>7; i--)
	{		
		if((buffer & (1lu<<i)) > 0)
		{
			buffer ^= CRCPoly;
		}
		CRCPoly = CRCPoly>>1;
	}
	
	if(buffer == 0)
	{
		return 1;
	}
	return 0;
}

uint8_t SHT25_ReadID(uint16_t *SNA, uint32_t *SNB, uint16_t *SNC)
{
	uint8_t DataToSend[2];
	uint8_t DataToReceive[8];
	uint8_t stat;
	
	//Initalize the serial number strings
	*SNA = 0;
	*SNB = 0;
	*SNC = 0;

	//Get first part of ID
	DataToSend[0] = SHT25_READ_ID1_ADDR1;
	DataToSend[1] = SHT25_READ_ID1_ADDR2;
	
	stat = I2CSoft_RW(SHT25_I2C_ADDR, DataToSend, DataToReceive, 2, 8);
	if(stat != SOFT_I2C_STAT_OK)
	{
		//I2C error when reading SNB
		return SHT25_RETURN_STATUS_TIMEOUT;
	}
	
	//Verify CRC of first section
	if( (SHT25_VerifyCRC(DataToReceive[0], DataToReceive[1]) == 1) && (SHT25_VerifyCRC(DataToReceive[2], DataToReceive[3]) == 1) && (SHT25_VerifyCRC(DataToReceive[4], DataToReceive[5]) == 1) && (SHT25_VerifyCRC(DataToReceive[6], DataToReceive[7]) == 1) )
	{
		*SNB = (((uint32_t)DataToReceive[0]) << 24) | (((uint32_t)DataToReceive[2]) << 16) | (((uint32_t)DataToReceive[4]) << 8) | ((uint32_t)DataToReceive[6]);
	}
	else
	{
		//CRC error
		*SNB = 0xFF;
		return SHT25_RETURN_STATUS_CRC_ERROR;
	}
	
	//Get second part of ID
	DataToSend[0] = SHT25_READ_ID2_ADDR1;
	DataToSend[1] = SHT25_READ_ID2_ADDR2;
	
	stat = I2CSoft_RW(SHT25_I2C_ADDR, DataToSend, DataToReceive, 2, 6);
	if(stat != SOFT_I2C_STAT_OK)
	{
		//I2C error when reading SNA/SNC
		return SHT25_RETURN_STATUS_TIMEOUT;
	}
	
	*SNA = (((uint16_t)DataToReceive[3]) << 8) | ((uint16_t)DataToReceive[4]);
	*SNC = (((uint16_t)DataToReceive[0]) << 8) | ((uint16_t)DataToReceive[1]);
	
	//Verify CRC of second section
	if( (SHT25_VerifyCRC(*SNA, DataToReceive[5]) == 1) && (SHT25_VerifyCRC(*SNC, DataToReceive[2]) == 1) )
	{
		return SHT25_RETURN_STATUS_OK;
	}
	else
	{
		*SNA = 0xFF;
		*SNC = 0xFF;
		return SHT25_RETURN_STATUS_CRC_ERROR;
	}
}

/** @} */