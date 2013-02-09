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
*	\brief		MPL115A1 digital barometer driver.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		2/8/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#include "main.h"

void MPL115A1_Init(void)
{
	MPL115A1_Sleep(0);
	MPL115A1_Deselect();
	return;
}

void MPL115A1_Sleep(uint8_t ToSleep)
{
	if(ToSleep == 1)
	{
		PORTB &= ~(1<<6);
	}
	else
	{
		PORTB |= (1<<6);
	}
	return;
}

void MPL115A1_Select(void)
{
	PORTB &= ~(1<<4);
	return;
}

void MPL115A1_Deselect(void)
{
	PORTB |= 1<<4;
	return;
}

void MPL115A1_GetCalData(uint8_t A0[], uint8_t B1[], uint8_t B2[], uint8_t C12[])
{
	SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_RISING | SPI_SAMPLE_LEADING | SPI_MODE_MASTER);
		
	MPL115A1_Select();
	
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_A0_MSB<<1));
	A0[1] = SPI_ReceiveByte();
	//printf("A0: 0x%02X", SPI_ReceiveByte());
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_A0_LSB<<1));
	A0[0] = SPI_ReceiveByte();
	//printf("%02X\n", SPI_ReceiveByte());
	
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B1_MSB<<1));
	B1[1] = SPI_ReceiveByte();
	//printf("B1: 0x%02X", SPI_ReceiveByte());
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B1_LSB<<1));
	B1[0] = SPI_ReceiveByte();
	//printf("%02X\n", SPI_ReceiveByte());
	
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B2_MSB<<1));
	B2[1] = SPI_ReceiveByte();
	//printf("B2: 0x%02X", SPI_ReceiveByte());
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B2_LSB<<1));
	B2[0] = SPI_ReceiveByte();
	//printf("%02X\n", SPI_ReceiveByte());
	
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_C12_MSB<<1));
	C12[1] = SPI_ReceiveByte();
	//printf("C12: 0x%02X", SPI_ReceiveByte());
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_C12_LSB<<1));
	C12[0] = SPI_ReceiveByte();
	//printf("%02X\n", SPI_ReceiveByte());
	
	MPL115A1_Deselect();
	return;
}

void MPL115A1_GetConversion(uint16_t *PressureData, uint16_t *TemperatureData)
{
	SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_RISING | SPI_SAMPLE_LEADING | SPI_MODE_MASTER);
	
	//Start conversions
	MPL115A1_Select();
	SPI_SendByte(MPL115AL_REG_CONVERT<<1);
	SPI_SendByte(0x00);
	MPL115A1_Deselect();
	
	DelayMS(4);
	
	MPL115A1_Select();
	SPI_SendByte(0x80 | (MPL115AL_REG_PRESSURE_MSB << 1));
	*PressureData = (SPI_ReceiveByte() << 8);
	
	SPI_SendByte(0x80 | (MPL115AL_REG_PRESSURE_LSB << 1));
	*PressureData |= (SPI_ReceiveByte());
	
	SPI_SendByte(0x80 | (MPL115AL_REG_TEMP_MSB << 1));
	*TemperatureData = (SPI_ReceiveByte() << 8);
	
	SPI_SendByte(0x80 | (MPL115AL_REG_TEMP_LSB << 1));
	*TemperatureData |= (SPI_ReceiveByte());
	
	MPL115A1_Deselect();
	return;
}


/** @} */