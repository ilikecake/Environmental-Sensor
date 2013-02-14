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

int16_t MPL115A1_CAL_A0;
int16_t MPL115A1_CAL_B1;
int16_t MPL115A1_CAL_B2;
int16_t MPL115A1_CAL_C12;

void MPL115A1_Init(void)
{
	MPL115A1_Deselect();
	MPL115A1_Sleep(0);
	
	//Wait for device to initalize
	DelayMS(10);
	
	//Get calibration data from device
	MPL115A1_UpdateCalData();
	
	return;
}

void MPL115A1_UpdateCalData(void)
{
	MPL115A1_GetCalData(&MPL115A1_CAL_A0, &MPL115A1_CAL_B1, &MPL115A1_CAL_B2, &MPL115A1_CAL_C12);
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

void MPL115A1_GetCalData(int16_t *A0, int16_t *B1, int16_t *B2, int16_t *C12)
{
	//uint8_t tempData;
	
	//TODO: remove this line later?
	SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_RISING | SPI_SAMPLE_LEADING | SPI_MODE_MASTER);
		
	MPL115A1_Select();
	
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_A0_MSB<<1));
	*A0 = (SPI_ReceiveByte() << 8);
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_A0_LSB<<1));
	*A0 |= SPI_ReceiveByte();
	
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B1_MSB<<1));
	*B1 = (SPI_ReceiveByte() << 8);
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B1_LSB<<1));
	*B1 |= SPI_ReceiveByte();
	
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B2_MSB<<1));
	*B2 = (SPI_ReceiveByte() << 8);
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B2_LSB<<1));
	*B2 |= SPI_ReceiveByte();
	
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_C12_MSB<<1));
	*C12 = (SPI_ReceiveByte() << 8);
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_C12_LSB<<1));
	*C12 |= SPI_ReceiveByte();
	
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
	
	//Values are 10 bit number, left aligned.
	*PressureData = *PressureData >> 6;
	*TemperatureData = *TemperatureData >> 6;
	
	return;
}

/** Calculate the Temperature compensated barometric pressure.
*	See the MPL115A1 data sheet and Freescale application note AN3785 for details.
*/
void MPL115A1_GetPressure(int16_t *Pressure_kPa)
{
	int32_t c12x2, a1, a1x1, y1, a2x2, PComp;
	uint16_t Padc;
	uint16_t Tadc;

	//Check if the cal data has been captured.
	if((MPL115A1_CAL_A0 == 0) || (MPL115A1_CAL_B1 == 0) || (MPL115A1_CAL_B2 == 0) || (MPL115A1_CAL_C12 == 0) )
	{
		MPL115A1_UpdateCalData();
	}

	//Get temperature and pressure conversion from the device
	MPL115A1_GetConversion(&Padc, &Tadc);
	
	//These calculations are stolen from application note AN3785 from Freescale.
	//Pcomp has an 8-bit integer portion and a four bit fractional portion
	c12x2 = (((int32_t)MPL115A1_CAL_C12) * Tadc) >> 11; 	// c12x2 = c12 * Tadc
	a1 = (int32_t)MPL115A1_CAL_B1 + c12x2; 					// a1 = b1 + c12x2
	a1x1 = a1 * Padc; 										// a1x1 = a1 * Padc
	y1 = (((int32_t)MPL115A1_CAL_A0) << 10) + a1x1; 		// y1 = a0 + a1x1
	a2x2 = (((int32_t)MPL115A1_CAL_B2) * Tadc) >> 1; 		// a2x2 = b2 * Tadc
	PComp = (y1 + a2x2) >> 9; 								// PComp = y1 + a2x2

	*Pressure_kPa = (((((int32_t)PComp) * 1041) >> 14) + 800);
	return;
}

/** @} */